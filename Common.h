// C++ headers
#include <cstdlib>
#include <cstring>
#include <iostream>

// Kernel Includes
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <sys/stat.h> 
#include <netdb.h>
#include <unistd.h>

typedef struct addrinfo AddressInfo;
typedef struct sockaddr_in SocketAddress;
typedef struct sockaddr_storage SocketStorage;
typedef struct sockaddr SocketAddressGen;

void initializeSocketServer(char* const &hostN, char* const &portN, int *socketDesc);
void initializeSocketClient(char* const &hostN, char* const &portN, int *socketDesc, AddressInfo* &bindPort, AddressInfo* &ptr);
void closeSocketServer( int *socket );
void closeSocketClient( AddressInfo* address, int* socketDesc );
void closeSocket( int *socket );

const unsigned int MSS = 1032; // maximum segment size
