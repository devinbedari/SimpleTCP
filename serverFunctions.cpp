#include "serverFunctions.h"
#include "TCPDatagram.h"

// Gets buffer size
unsigned int getFileSize(int inFileDes)
{
	// Result is stored in a stat buffer
	struct stat buf;
	fstat(inFileDes, &buf);
	int size = buf.st_size;
	return size;
}

// Splits the file descriptor (fd) into an array of cstrings; returns number of packets split
int splitFile(int fd, TCPDatagram* &packets, unsigned int maxPacketSize)
{
	// Grab the fileSize
	unsigned int fileSize = getFileSize(fd);

	int maxDataSize = maxPacketSize-8; // subtract 8 bytes for header

	// Grab number of packets to split
	unsigned int splitNumber = (fileSize/maxDataSize) + 1; 

	// Assign an array of packets of maxDataSize size
	packets = new TCPDatagram[splitNumber];

	char* temp = new char[maxDataSize];

	// Read until the end of the descriptor
	for (unsigned int i = 0; i < splitNumber; i++) {

		unsigned int dataSize = i*maxDataSize < fileSize ? maxDataSize : fileSize-(i*maxDataSize);

		// Assign a size to store the maxDataSize characters, and headers
		memset(temp, 0, sizeof(char)*dataSize);

		unsigned int bytesRead = read(fd, temp, dataSize);

		packets[i].data.append(temp, bytesRead);
		packets[i].windowSize = bytesRead;
//		std::cout << "file read: " << temp << std::endl;

		// Check the return value
		if(bytesRead <= 0) {
			std::cerr << "Fatal Error: Could not read from file descriptor" << std::endl;
			exit(0);
		}
	}

	delete temp;

	// Return number of packets
	return splitNumber;
}
