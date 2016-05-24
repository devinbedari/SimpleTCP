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

using namespace std;

typedef struct addrinfo AddressInfo;

// Handles parsing and errors
void parse( int argcount, char *argval[], char* &host_name, char* &port_n)
{
    int Port;
    // Display help message if the number of arguements are incorrect
    if ( argcount != 3 )
    {
        // We print argv[0] assuming it is the program name
        cout << "Usage: " << argval[0] << " <hostname> <port>" << endl;
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
    }
}

int main ( int argc, char *argv[] )
{
    // Variable declarations
    char* hostName;                             // IP or Domain that we need to connect to 
    char* port;                                 // Port number to connect to
    int udpSocket;                              // File descriptor for the socket

    // Generate the address structures
    AddressInfo *bindAddress, *p;

    // Parse the input
    parse(argc, argv, hostName, port); 

    // Initialize the connection
    initializeSocketClient(hostName, port, &udpSocket, bindAddress, p);

    // Debugging Only: Test send datagram:
    // Create the message buffer to send
    char messageToSend[30];
    int flag = 1;
    int bytesSent;
    strcpy(messageToSend, "Hello, World!");
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

    // Close the connection
    closeSocketClient(bindAddress, &udpSocket);

    return 0;

}