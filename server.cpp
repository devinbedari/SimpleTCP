// C++ headers
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <csignal>

// Kernel Includes
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <sys/stat.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

// User defined headers
#include "Common.h"
#include "Header.h"
//#include "TCPDatagram.h"
//#include "TCPDatagramBuilder.h"

using namespace std;

// Define constants 
const unsigned int MSS = 1032;

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

int main ( int argc, char *argv[] )
{
    // Variable declarations
    char* hostName;                             // IP or Domain that we need to connect to 
    char* port;                                 // Port number to open socket
    char* hostDir;                              // Directory that is being hosted
    int udpSocket;			                    // Socket descriptor, and file descriptor
    SocketStorage clientInfo;                   // Incoming client datagram address structures 
    socklen_t sizeClient;                       // Size of client address in bytes
    
    // Parse the input
    parse(argc, argv, hostName, port, hostDir);

    // Grab the socket descriptor              
    initializeSocketServer(hostName, port, &udpSocket);

    /*
    // Debugging Only: From Beej Network Programming Guide
    // Set size of the sizeClient variable
    sizeClient = sizeof clientInfo;         
    char buf[30];
    // memset(&buf, 0, sizeof(buf));   // Zero out the hints
    int bytesRec;
    int flag = 1;
    cout << "Socket number: " << udpSocket << endl;

    // Test get datagram
    while(flag)
    {
        // Gets datagram
        // ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
        if ( ( bytesRec = recvfrom( udpSocket, &buf, sizeof(buf), 0, (SocketAddressGen *) &clientInfo, &sizeClient ) ) == -1 )
        {
            cerr << "Cannot receive the datagram";
            break;
        }
        else
        {
            printf("listener: packet is %d bytes long\n", bytesRec);
            buf[bytesRec] = '\0';
            cout << "Listener: packet contains: " << buf << endl;
        }
    }
    */ 

    // FIN flag
    int fin = 0;

    // Set size of the sizeClient variable
    sizeClient = sizeof clientInfo;
    
    // Buffer to send
    char buf[MSS];
    char send[MSS];

    // Initalize both to null bytes
    buf[0] = send[0] = 0;

    // Gen the packet for the data
    char *packet = NULL;
    char *incomingData = NULL;
    char *fileReq = NULL;

    // Bytes received
    int bytesSent, bytesRec;
    
    // Generate header information
    Headers sendHeaders, recvHeaders;

    // Listen for incoming requests
    while(!fin)
    {
        // Get SYN
        if ( ( bytesRec = recvfrom( udpSocket, &buf, 9, 0, (SocketAddressGen *) &clientInfo, &sizeClient ) ) == -1 )
        {
            cerr << "Could not obtain incoming SYN datagram" << endl;
            continue;
        }
        else
        {
            // Process packet for SYN
            parsePacket(buf, recvHeaders, incomingData, bytesRec-8);


            // Check connection SYN
            if((recvHeaders.flags & SYN) == SYN)
            {
                // Gen the next packet
                setHeader(genRand(), genNextNum(recvHeaders.seq_no, bytesRec-8), 1, ACK|SYN, sendHeaders);
                genPacket(packet, sendHeaders, send, 1);

                // Send responding SYN/ACK
                if( ( bytesSent = ( sendto(udpSocket, packet, 9, 0, (SocketAddressGen *)& clientInfo, sizeClient) ) ) == -1)
                {
                    // Request resend after timeout
                    cerr << "Couldn't send the response ACK" << endl;
                    continue;
                }
                else
                {
                    // Receive ACK+fileRequest
                    if ( ( bytesRec = recvfrom( udpSocket, &buf, MSS, 0, (SocketAddressGen *) &clientInfo, &sizeClient ) ) == -1 )
                    {
                        cerr << "Handshake could not be established. " << endl;
                    }
                    else
                    {
                        // Free incoming data for reuse
                        delete incomingData;
                        incomingData = NULL;

                        // Parse incoming packet
                        parsePacket(buf, recvHeaders, fileReq, bytesRec-7);

                        // Double loop here
                        
                    }
                }
            }
            else
            {
                cerr << "No syn established; closing connection." << endl;
                fin = 1;
            }              
        }
    }

    // Close the connection
    closeSocketServer(&udpSocket);
}