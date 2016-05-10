#include "Common.h"

typedef struct addrinfo AddressInfo;

// Initialize socket, and return the file descriptor 
void initializeSocket(char* const &hostN, char* const &portN, int *socketDesc)
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

void closeSocket( int *socket )
{
    close(*socket);
}