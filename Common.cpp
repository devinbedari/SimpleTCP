#include "Common.h"

// Initialize socket, and return the file descriptor 
void initializeSocketServer(char* const &hostN, char* const &portN, int *socketDesc)
{   
    // Socket addresses; I don't know if I can move these into initialize()
    AddressInfo hints, *servInfo, *p;    // getaddrinfo structs

    // Socket Procedures:
    memset(&hints, 0, sizeof(hints));   // Zero out the hints
    hints.ai_family = AF_INET;          // Assuming IPv4
    hints.ai_socktype = SOCK_DGRAM;     // UDP socket

    // Find the address info of the hostname
    if ((getaddrinfo(hostN, portN, &hints, &servInfo) != 0)) 
    {
        std::cerr << "Call to getaddrinfo failed; check hostname and/or port number and try again." << std::endl;
        exit(1);
    }

    // Loop through servInfo results (from the previous procedure)
    // Loop until connection has been established
    for(p = servInfo; p != NULL; p = p->ai_next) 
    {
        if ((*socketDesc = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            // cerr << "Socket could not establish file descriptor" << endl;
            continue;
        }

        // Bind the socket
        if (bind(*socketDesc, p->ai_addr, p->ai_addrlen) == -1) 
        {
            // cerr << "Could not connect to socket" << endl;
            close(*socketDesc);
            continue;
        }

        break; // If we get here, we must have connected successfully
    }

    // What if we couldn't connect with any host in the servinfo list?
    if (p == NULL) 
    {
        std::cerr << "Failed to connect to server" << std::endl ;
        exit(1);
    } 

    // All done with this structure; clean information
    freeaddrinfo(servInfo); 
}

// Close socket connection
void closeSocketServer( int *socket )
{
    close(*socket);
}


// Initialize socket, and return the file descriptor 
void initializeSocketClient(char* const &hostN, char* const &portN, int *socketDesc, AddressInfo* &bindPort, AddressInfo* &ptr)
{   
    // Socket addresses; I don't know if I can move these into initialize()
    AddressInfo hints;    // getaddrinfo structs

    // Socket Procedures:
    memset(&hints, 0, sizeof(hints));   // Zero out the hints
    hints.ai_family = AF_INET;          // Assuming IPv4
    hints.ai_socktype = SOCK_DGRAM;     // UDP socket

    // Find the address info of the hostname
    if ((getaddrinfo(hostN, portN, &hints, &bindPort) != 0)) 
    {
        std::cerr << "Call to getaddrinfo failed; check hostname and/or port number and try again." << std::endl;
        exit(1);
    }

    // Loop through servInfo results (from the previous procedure)
    // Loop until connection has been established
    for(ptr = bindPort; ptr != NULL; ptr = ptr->ai_next) 
    {
        if ((*socketDesc = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1) 
        {
            std::cerr << "Socket could not establish file descriptor" << std::endl;
            continue;
        }
        break; // If we get here, we must have connected successfully
    }

    // What if we couldn't connect with any host in the servinfo list?
    if (ptr == NULL) 
    {
        std::cerr << "Failed to connect to server" << std::endl ;
        exit(1);
    } 
}

// Free any address structures allocated
void closeSocketClient( AddressInfo *address, int *socketDesc )
{
    // All done with this structure; clean information
    freeaddrinfo(address); 
    close(*socketDesc);
}

// Change the window size accorging to the client/server value
void changeWindow (int &windowSize, int &threshold , bool success)
{
    if(success)
    {
        if(windowSize <= threshold)
        {
            windowSize *= 2;
        }
        else
        {
            windowSize += 1;
        }
    }
    else
    {
        windowSize = 1;
        threshold = threshold/2;
    }
}