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

using namespace std;

#define NPACK 16;

// Handles parsing and errors
void parse( int argcount, char *argval[], char* &host_name, char* &port_n)
{
    // Display help message if the number of arguements are incorrect
    if ( argcount != 3 )
    {
        // We print argv[0] assuming it is the program name
        cout << "Usage: "<< argval[0] << " <hostname> <port>" << endl;
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
    }
}

int main ( int argc, char *argv[] )
{
    // Variable declarations
    char* hostName;                             // IP or Domain that we need to connect to 
    char* port;                                 // Port number to open socket
    int udpSocket;			                    // Socket descriptor, and file descriptor
    SocketStorage clientInfo;                   // Incoming client datagram address structures 
    socklen_t sizeClient;                       // Size of client address in bytes
    
    // Parse the input
    parse(argc, argv, hostName, port);

    // Grab the socket descriptor              
    initializeSocketServer(hostName, port, &udpSocket);

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

    // Close the connection
    closeSocketServer(&udpSocket);
    cout << endl << "Success!";
}