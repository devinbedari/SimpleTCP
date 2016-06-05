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

// Define constants 
const unsigned int ssthresh = 30720;

// Variable declarations
char* hostName;                             // IP or Domain that we need to connect to 
char* port;                                 // Port number to open socket
char* hostDir;                              // Directory that is being hosted
int udpSocket;                              // Socket descriptor, and file descriptor
SocketStorage clientInfo;                   // Incoming client datagram address structures 
socklen_t sizeClient;                       // Size of client address in bytes

enum TCPConnectionState { START, SYN_RECEIVED, ESTABLISHED, FIN_RECEIVED, CLOSED };

TCPConnectionState currentState = START;
int currentAckNum = 0;
int currentSeqNum = 0;
int packetWindow = 0;
int controlWindow = 10; // arbitrary value for now
bool established = false;
list<TCPDatagram> packetQueue; // queue of packets to send and acknowledge. Sends packets at packetQueue.front() first
unordered_set<unsigned int> expectedAckNums; // ack numbers that we expect from the client, to ensure the client is sending valid acks

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
int nextSeqNum(TCPDatagram packet) {
    return packet.sequenceNum + (packet.windowSize > 0 ? packet.windowSize : 1);
}

// assigns a sequence number
void assignSequenceNum (TCPDatagram &packet) {
    packet.sequenceNum = currentSeqNum;
    currentSeqNum = nextSeqNum(packet);
}

void sendPackets() {
    expectedAckNums.clear(); // clear set

    int packetsSent = 0;
    for (list<TCPDatagram>::iterator iter = packetQueue.begin(); iter != packetQueue.end(); ++iter) {
        (*iter).ackNum = currentAckNum; // assign ack number just before sending
        string str = (*iter).toString();

        if (sendto(udpSocket, str.c_str(), str.length(), 0, (SocketAddressGen *)& clientInfo, sizeClient) < 0) {
            // Request resend after timeout
            cerr << "Couldn't send the response ACK" << endl;
            break; // don't continue because we don't want to send out of order
        }
        int expectedAckNum = nextSeqNum(*iter);
        expectedAckNums.insert(expectedAckNum); // add sequence number to set
        cout << "sent sequence num: " << (*iter).sequenceNum << endl;
        if (++packetsSent > controlWindow) break;
    }
}

void packetReceived (TCPDatagram packet) {

    cout << "received ack num: " << packet.ackNum << endl;

    // if connection is open, and ack number doesn't match any of the expected ack numbers, drop the packet
    if (currentState != START && expectedAckNums.count(packet.ackNum) == 0) {
        return;
    }

    // removed all acknowledged packets
    while (!packetQueue.empty() && packet.ACK && packet.ackNum > packetQueue.front().sequenceNum) {
        packetQueue.pop_front(); // packet acknowledged, removed from queue
    }

    if (currentAckNum == packet.sequenceNum) {
        currentAckNum = nextSeqNum(packet);
    }

    switch (currentState) {

        case START:
            if (packet.SYN) {
                currentAckNum = packet.sequenceNum+1;

                currentSeqNum = genRand();

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
                currentState = FIN_RECEIVED;
            } else {

                // Generate the file location
                string fileLocation = "";
                fileLocation += hostDir;
                if (fileLocation[fileLocation.length()-1] != '/')
                    fileLocation += '/'; // append '/' if necessary
                fileLocation += packet.data;

                // cerr << "Opening file: " << fileLocation << endl;
                int FileDes = open(fileLocation.c_str(), O_RDONLY);
                if (FileDes == -1)
                {
                    cerr << "Fatal Error: Could not opend file. Are you sure you have the right permissions?" << endl;
                    exit(0);
                }

                TCPDatagram* packets;

                // Open folder and package the file into TCP packets
                int numOfPackets = splitFile(FileDes, packets, MSS);

                for (int i = 0; i < numOfPackets; i++) {
                    assignSequenceNum(packets[i]);
                    packetQueue.push_back(packets[i]);
                }

                sendPackets();

                currentState = ESTABLISHED;
            }
            break;

        case ESTABLISHED:
            if (packet.FIN) {
                currentState = FIN_RECEIVED;
            } else {
                if (packetQueue.empty()) {
                    // no more data, close connection using a FIN packet
                    TCPDatagram fin;
                    fin.SYN = false;
                    fin.FIN = true;
                    fin.ACK = true;
                    fin.windowSize = 0;
                    assignSequenceNum(fin);
                    packetQueue.push_back(fin);

                    sendPackets();
                } else {
                    // still more packets, continue transmitting
                    sendPackets();
                }
            }
            break;

        case FIN_RECEIVED:
            // TODO
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
                cout << "PACKET INCOMPLETE" << endl;
            }

            delete builder;
        } else {
            cerr << "Could not obtain incoming SYN datagram" << endl;
            continue;
        }
    }

    // Close the connection
    closeSocketServer(&udpSocket);
}