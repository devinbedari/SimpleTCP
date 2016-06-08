// C++ headers
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>

// Kernel Includes
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <unordered_set>
#include <list>

// User defined headers
#include "Common.h"
#include "Header.h"
#include "TCPDatagram.h"
#include "TCPDatagramBuilder.h"

using namespace std;

typedef struct addrinfo AddressInfo;

// Variable declarations
char* hostName;                             // IP or Domain that we need to connect to 
char* port;                                 // Port number to open socket
char* fileName;                             // File requested from server
int udpSocket;                              // Socket descriptor, and file descriptor
SocketStorage serverInfo;                   // Incoming client datagram address structures 
socklen_t sizeServer;                       // Size of client address in bytes

// Generate the address structures
AddressInfo *bindAddress, *p;

enum TCPConnectionState { START, SYN_SENT, ESTABLISHED, FIN_SENT, CLOSED };

TCPConnectionState currentState = START;
uint16_t cumulativeAckNum = 0;
uint16_t currentSeqNum = 0;
int packetWindow = 0;
bool established = false;
list<TCPDatagram> packetQueue; // queue of packets to send and acknowledge. Sends packets at packetQueue.front() first

// Handles parsing and errors
void parse( int argcount, char *argval[], char* &host_name, char* &port_n, char* &fileReq)
{
    int Port;
    // Display help message if the number of arguements are incorrect
    if ( argcount != 4 )
    {
        // We print argv[0] assuming it is the program name
        cout << "Usage: " << argval[0] << " <hostname> <port> <file_to_download>" << endl;
        exit(0);
    }
    // Parse values
    else 
    {    
        // argv[1] is the hostname
        host_name = argval[1];
        if (host_name == NULL) 
        {
            cerr << "Hostname is invalid" << endl;
            exit(0);
        }
        // argv[2] is the port number
        port_n = argval[2];
        Port = atoi(argval[2]);
        if(Port < 0 || Port > 65535)
        {
            cerr << "Error: port number is invalid" << endl;
            exit(1);
        }
        // argv[3] is the file to send
        fileReq = argval[3];
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

uint16_t lastAckSent = 1; // start a 1 so first SYN (which has acknum 0) doesn't trigger a "retransmission" print

void sendprint(TCPDatagram packet, bool retransmission) {
    cout << "Sending packet ";
    cout << packet.ackNum;
    if (retransmission) cout << " Retransmission";
    if (packet.SYN) cout << " SYN";
    if (packet.FIN) cout << " FIN";
    cout << endl;
}

void sendPackets() {
    for (list<TCPDatagram>::iterator iter = packetQueue.begin(); iter != packetQueue.end(); ) {
        TCPDatagram packet = *iter;
        string str = packet.toString();

        if (sendto(udpSocket, str.c_str(), str.length(), 0, p->ai_addr, p->ai_addrlen) < 0) {
            // Request resend after timeout
            cerr << "Couldn't send the response ACK" << endl;
            break; // don't continue because we don't want to send out of order
        }
        sendprint(packet, (lastAckSent == packet.ackNum));
        lastAckSent = packet.ackNum;

        // if the packet is an empty ACK, erase it from the queue so we don't expect an ACK for this ACK
        if (packet.ACK && packet.windowSize == 0)
            iter = packetQueue.erase(iter); // remember: erasing an element will move iterator to next element
        else
            iter++; // only iterate if no element is erased
    }
}

int saveFd;
TCPDatagram lastWrittenPacket;
int totalWrote = 0;

void packetReceived (TCPDatagram packet) {

    cout << "Receiving packet " << packet.sequenceNum << endl;

    // removed all acknowledged packets
    while (!packetQueue.empty() && packet.ACK && packet.ackNum > packetQueue.front().sequenceNum) {
        packetQueue.pop_front(); // packet acknowledged, removed from queue
    }

    bool arrivedInOrder = packet.sequenceNum == nextSeqNum(lastWrittenPacket);

    if (packet.sequenceNum == cumulativeAckNum) {
        cumulativeAckNum = nextSeqNum(packet);
    }

    switch (currentState) {

        case SYN_SENT:
            if (packet.SYN && packet.ACK) {
                cumulativeAckNum = nextSeqNum(packet);
                lastWrittenPacket = packet;

                TCPDatagram fileReq;
                fileReq.SYN = false;
                fileReq.FIN = false;
                fileReq.ACK = true;
                fileReq.ackNum = cumulativeAckNum;
                fileReq.data = fileName;
                fileReq.windowSize = fileReq.data.length();
                assignSequenceNum(fileReq);
                packetQueue.push_back(fileReq);

                sendPackets();
        
                saveFd = open("received.data", O_CREAT|O_RDWR|O_TRUNC, 0666);

                currentState = ESTABLISHED;
            }
            break;

        case ESTABLISHED:
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

                if (arrivedInOrder) {
                    write(saveFd, packet.data.c_str(), packet.data.length());
                    lastWrittenPacket = packet;
                    totalWrote++;
                }

                //if ((rand()%20) == 0) break;
//                cout << "client receieved data: " << packet.data << endl;
                // wait until timeout to send ack
                TCPDatagram ack;
                ack.ACK = true;
                ack.ackNum = cumulativeAckNum;
                ack.windowSize = 0;
                assignSequenceNum(ack);
                packetQueue.push_back(ack);

                sendPackets();
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
    parse(argc, argv, hostName, port, fileName); 

    // Initialize the connection
    initializeSocketClient(hostName, port, &udpSocket, bindAddress, p);

    currentSeqNum = genRand(5463);

    TCPDatagram packet;
    packet.SYN = true;
    packet.FIN = false;
    packet.ACK = false;
    packet.windowSize = 0;
    packet.ackNum = cumulativeAckNum;
    assignSequenceNum(packet);
    packetQueue.push_back(packet);

    sendPackets();

    currentState = SYN_SENT;

    char buf[MSS];

    while (currentState != CLOSED) {
        int bytesRec;
        if ( (bytesRec = recvfrom( udpSocket, &buf, MSS, 0, (SocketAddressGen *) &serverInfo, &sizeServer )) > 0 ) {

            TCPDatagramBuilder* builder = new TCPDatagramBuilder(buf, bytesRec);

            if (builder->isComplete()) {
                packetReceived(*(builder->getDatagram()));
            } else {
                cerr << "PACKET INCOMPLETE" << endl;
            }

            delete builder;
        } else {
            cerr << "Could not receive" << endl;
            continue;
        }
    }

    // Close the connection
    closeSocketClient(bindAddress, &udpSocket);

    return 0;

}