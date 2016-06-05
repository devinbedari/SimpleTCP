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

// User defined headers
#include "Common.h"
#include "Header.h"

using namespace std;

// Define constants 
const unsigned int MSS = 1032;

typedef struct addrinfo AddressInfo;

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

int main ( int argc, char *argv[] )
{
    // Variable declarations
    char* hostName;                             // IP or Domain that we need to connect to 
    char* port;                                 // Port number to connect to
    char* fileName;                             // File requested from server
    int udpSocket;                              // File descriptor for the socket
    SocketStorage serverInfo;                   // Incoming client datagram address structures 
    socklen_t sizeServer;                       // Size of client address in bytes

    // Generate the address structures
    AddressInfo *bindAddress, *p;

    // Parse the input
    parse(argc, argv, hostName, port, fileName); 

    // Initialize the connection
    initializeSocketClient(hostName, port, &udpSocket, bindAddress, p);
    
    //To close the while loop
    // int fin = 0;

    // Buffer to send
    char buf[MSS];
    char send[MSS];

    // Initalize both to null bytes
    buf[0] = send[0] = 0;

    // Bytes received
    int bytesSent, bytesRec;
    
    // Get payload
    char *packet = NULL;
    char *incomingData = NULL;
    
    // Generate header information
    Headers sendHeaders, recvHeaders;

    //Generate SYN packet
    setHeader(genRand(), 0, 1, SYN, sendHeaders);
    genPacket(packet, sendHeaders, send, 1);

    // Send SYN
    if( (bytesSent = (sendto(udpSocket, packet, 9, 0, p->ai_addr, p->ai_addrlen))) == -1 )
    {
        cerr << "Error in sending SYN" << endl;
    }
    else
    {
        // Get SYN/ACK
        if((bytesRec = (recvfrom(udpSocket, &buf, 9, 0, (SocketAddressGen *) &serverInfo, &sizeServer ))) == -1 )
        {
            cerr << "Error in getting SYN/ACK" << endl;
        }
        else
        {
            // Process packet for SYN/ACK
            parsePacket(buf, recvHeaders, incomingData, bytesRec-8);

            //Send File Request+ACK
            if ( (recvHeaders.flags & (SYN|ACK)) == (SYN|ACK) )
            {   
                //Remove the old packet
                delete packet;
                packet = NULL;

                // Gen file request
                setHeader(recvHeaders.ack_no, genNextNum(recvHeaders.seq_no, bytesRec-8), 1, ACK, sendHeaders);
                genPacket(packet, sendHeaders, fileName, strlen(fileName));

                // Send ACK for SYN, and also request file
                if( (bytesSent = (sendto(udpSocket, packet, (strlen(fileName)+8), 0, p->ai_addr, p->ai_addrlen))) == -1 )
                {
                    cerr << "Could not send request for file. " << endl;
                }
                else
                {

                }

            }
        }
    }

    /*
    // Debugging Only: Unit test send datagram:
    // Create the message buffer to send
    char messageToSend[30];
    int flag = 1;
    int bytesSent;
    strcpy(messageToSend, "Hello, World!");-
    for (size_t i = 0; flag != 0; i++) 
    {
        // ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
        if ( (bytesSent = (sendto(udpSocket, messageToSend, strlen(messageToSend), 0, p->ai_addr, p->ai_addrlen))) == -1) 
        {
            cerr << "Could not send datagram to server" << endl;
            exit(1);
        }
        cout << "Sending packet: " << i << endl;
        cout << "Datagram of " << bytesSent << " bytes sent" << endl;
        if (bytesSent == 13)
            flag = 0;
    }
    */

    // Close the connection
    closeSocketClient(bindAddress, &udpSocket);

    return 0;

}