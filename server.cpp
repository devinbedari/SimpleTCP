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
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

// User defined headers
#include "Common.h"

using namespace std;


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

    // Parse the input
    parse(argc, argv, hostName, port);

    // Grab the socket descriptor              
    initializeSocket(hostName, port, &udpSocket);

    // Close the connection
    closeSocket(&udpSocket);

    cout << endl << "Success!";
}