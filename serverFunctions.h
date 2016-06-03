#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

unsigned int getFileSize(int inFileDes);
int splitFile(int fd, char** &splitBuffer, unsigned int packetSplitValue);