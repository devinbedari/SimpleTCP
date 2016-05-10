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

void initializeSocket(char* const &hostN, char* const &portN, int *socketDesc);
void closeSocket( int *socket );