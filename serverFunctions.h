#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include "TCPDatagram.h"

unsigned int getFileSize(int inFileDes);
int splitFile(int fd, TCPDatagram* &packets, unsigned int maxPacketSize);