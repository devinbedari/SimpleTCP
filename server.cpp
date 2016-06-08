// C++ headers
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <csignal>
#include <string>

// Kernel Includes
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <sys/stat.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <unordered_set>
#include <list>

// User defined headers
#include "Common.h"
#include "Header.h"
#include "serverFunctions.h"
#include "TCPDatagram.h"
#include "TCPDatagramBuilder.h"

using namespace std;

// Variable declarations
char* hostName;                             // IP or Domain that we need to connect to 
char* port;                                 // Port number to open socket
char* hostDir;                              // Directory that is being hosted
int udpSocket;                              // Socket descriptor, and file descriptor
SocketStorage clientInfo;                   // Incoming client datagram address structures 
socklen_t sizeClient;                       // Size of client address in bytes

enum TCPConnectionState { START, SYN_RECEIVED, ESTABLISHED, FIN_SENT, CLOSED };

TCPConnectionState currentState = START;
uint16_t currentAckNum = 0;
uint16_t currentSeqNum = 0;
int packetWindow = 0;
int controlWindow = 1; // arbitrary value for now
bool established = false;
int ssthresh = 15;
list<TCPDatagram> packetQueue; // queue of packets to send and acknowledge. Sends packets at packetQueue.front() first

// Handles parsing and errors
void parse( int argcount, char *argval[], char* &host_name, char* &port_n, char* &host_dir)
{
    // Display help message if the number of arguements are incorrect
    if ( argcount != 4 )
    {
        // We print argv[0] assuming it is the program name
        cout << "Usage: "<< argval[0] << " <hostname> <port> <hosting_directory>" << endl;
        exit(0);
    }
    // Parse values
    else 
    {
        // argv[1] is the hostname
        host_name = argval[1];

        // argv[2] is the port number
        port_n = argval[2];
        int Port = atoi(argval[2]);
        if(Port < 0 || Port > 65535)
        {
            cerr << "Error: port number is invalid" << endl;
            exit(1);
        }

        host_dir = argval[3];
    }
}

// calculates the next sequence number
uint16_t nextSeqNum(TCPDatagram packet) {
    return (packet.sequenceNum + (packet.windowSize > 0 ? packet.windowSize : 1)) % 30720;
}

// assigns a sequence number
void assignSequenceNum (TCPDatagram &packet) {
    packet.sequenceNum = currentSeqNum;
    currentSeqNum = nextSeqNum(packet);
}

int dataPacketsSent = 0;
int acksReceived = 0;

void sendprint(TCPDatagram packet, bool retransmission) {
    cout << "Sending packet ";
    cout << packet.sequenceNum << " " << controlWindow << " " << ssthresh;
    if (retransmission) cout << " Retransmission";
    if (packet.SYN) cout << " SYN";
    if (packet.FIN) cout << " FIN";
    cout << endl;
}

void sendPackets() {
    int resendcount = dataPacketsSent - acksReceived;
    dataPacketsSent = 0;
    for (list<TCPDatagram>::iterator iter = packetQueue.begin(); iter != packetQueue.end(); ) {
        TCPDatagram packet = *iter;
        packet.ACK = true;
        packet.ackNum = currentAckNum; // assign ack number just before sending
        string str = packet.toString();

        if (sendto(udpSocket, str.c_str(), str.length(), 0, (SocketAddressGen *)& clientInfo, sizeClient) < 0) {
            // Request resend after timeout
            cerr << "Couldn't send" << endl;
            break; // don't continue because we don't want to send out of order
        }

        bool retransmission = false;
        if (packet.windowSize > 0) {
            dataPacketsSent++;
            if (dataPacketsSent <= resendcount) {
                retransmission = true;
            }
        }

        sendprint(packet, retransmission);

        // if the packet is an empty ACK, erase it from the queue so we don't expect an ACK for this ACK
        if (packet.ACK && packet.windowSize == 0)
            iter = packetQueue.erase(iter); // remember: erasing an element will move iterator to next element
        else
            iter++; // only iterate if no element is erased


        if (dataPacketsSent >= controlWindow) {
            break;
        }
    }

    acksReceived = 0;
}

int FileDes;
bool finishedReading = false;

void readAndRefillQueue() {

    int maxDataSize = MSS-8; // subtract 8 bytes for header
    char* buf = new char[maxDataSize];
    int bytesRead;

    while (((int) packetQueue.size()) < controlWindow) {
        bytesRead = read(FileDes, buf, maxDataSize);

        if (bytesRead <= 0) {
            finishedReading = true;
            close(FileDes);
            break;
        }

        TCPDatagram pkt;
        pkt.data.append(buf, bytesRead);
        pkt.windowSize = bytesRead;
        assignSequenceNum(pkt);
        packetQueue.push_back(pkt);
    }

    free(buf);
}

void packetReceived (TCPDatagram packet) {

    cout << "Receiving packet " << packet.ackNum << endl;

    // removed acknowledged packets
    while (!packetQueue.empty() && packetQueue.front().sequenceNum != packet.ackNum) {
        packetQueue.pop_front();
        acksReceived++;
    }

    if (currentAckNum == packet.sequenceNum) {
        currentAckNum = nextSeqNum(packet);
    }

    switch (currentState) {

        case START:
            if (packet.SYN) {
                currentAckNum = nextSeqNum(packet);

                currentSeqNum = genRand(34567);

                TCPDatagram packet;
                packet.SYN = true;
                packet.FIN = false;
                packet.ACK = true;
                packet.windowSize = 0;
                assignSequenceNum(packet);
                packetQueue.push_back(packet);

                sendPackets();

                currentState = SYN_RECEIVED;
            }
            break;

        case SYN_RECEIVED:
            if (packet.FIN) {
                // send FIN/ACK
                TCPDatagram fin;
                fin.SYN = false;
                fin.FIN = true;
                fin.ACK = true;
                fin.windowSize = 0;
                assignSequenceNum(fin);
                packetQueue.push_back(fin);

                sendPackets();

                currentState = CLOSED;
            } else {

                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 500000;

                if (setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
                    cerr << "Error";
                }

                // Generate the file location
                string fileLocation = "";
                fileLocation += hostDir;
                if (fileLocation[fileLocation.length()-1] != '/')
                    fileLocation += '/'; // append '/' if necessary
                fileLocation += packet.data;

                // cerr << "Opening file: " << fileLocation << endl;
                FileDes = open(fileLocation.c_str(), O_RDONLY);
                if (FileDes == -1)
                {
                    cerr << "Fatal Error: Could not opend file. Are you sure you have the right permissions?" << endl;
                    exit(0);
                }

                readAndRefillQueue();

                sendPackets();

                currentState = ESTABLISHED;
            }
            break;

        case ESTABLISHED:
            if (packet.FIN) {
                TCPDatagram fin;
                fin.SYN = false;
                fin.FIN = true;
                fin.ACK = true;
                fin.windowSize = 0;
                assignSequenceNum(fin);
                packetQueue.push_back(fin);

                sendPackets();

                currentState = CLOSED;
            } else if (acksReceived >= dataPacketsSent) {

                if (!finishedReading) readAndRefillQueue();

                if (packetQueue.empty()) {
                    // no more data, close connection using a FIN packet
                    TCPDatagram fin;
                    fin.SYN = false;
                    fin.FIN = true;
                    fin.ACK = true;
                    fin.windowSize = 0;
                    assignSequenceNum(fin);
                    packetQueue.push_back(fin);

                    //cerr << endl << "eiControl Window Size: " << controlWindow << endl;
                    sendPackets();

                    // Increase/Decrease window size:
                    //changeWindow(controlWindow, ssthresh, true);


                    currentState = FIN_SENT;
                } else {
                    // still more packets, continue transmitting
                    sendPackets();
                    // Increase/Decrease window size:
                    changeWindow(controlWindow, ssthresh, true);
                }
            }
            break;

        // ----------------- IM ACTUALLY NOT SURE HOW CONNECTION FIN WORKS, SO CHECK EVERYTHING BELOW THIS ------------------
        case FIN_SENT:
            if (packet.FIN) { // FIN/ACK received
                currentState = CLOSED;
            }
            break;

        case CLOSED:
            // TODO
            break;

        default:
            cerr << "Server: unknown state" << endl;
            exit(0);
    }
}

int main ( int argc, char *argv[] )
{
    // Parse the input
    parse(argc, argv, hostName, port, hostDir);

    // Grab the socket descriptor              
    initializeSocketServer(hostName, port, &udpSocket);

    // Set size of the sizeClient variable
    sizeClient = sizeof clientInfo;

    char buf[MSS];

    while (currentState != CLOSED) {
        int bytesRec;
        if ( (bytesRec = recvfrom( udpSocket, &buf, MSS, 0, (SocketAddressGen *) &clientInfo, &sizeClient )) > 0 ) {

            TCPDatagramBuilder* builder = new TCPDatagramBuilder(buf, bytesRec);

            if (builder->isComplete()) {
                packetReceived(*(builder->getDatagram()));
            } else {
                cerr << "PACKET INCOMPLETE" << endl;
            }

            delete builder;
        } else {
            cerr << "Timeout!!" << endl;
            changeWindow(controlWindow, ssthresh, false);
            sendPackets();
            continue;
        }
    }
    // Close the connection
    closeSocketServer(&udpSocket);
}