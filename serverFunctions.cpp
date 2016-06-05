#include "serverFunctions.h"

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
int splitFile(int fd, char** &splitBuffer, unsigned int packetSplitValue)
{
	// Flag to indicate EOF
	int flag = 1;

	// Grab the fileSize
	unsigned int fileSize = getFileSize(fd);

	// Grab number of packets to split
	unsigned int splitNumber = (fileSize/packetSplitValue) + 1; 

	// Assign an array of packets of packetSplitValue size
	splitBuffer = new char* [splitNumber];

	// Packet incrementer
	unsigned int i = 0;
	// Read until the end of the descriptor
	do
	{
		// Calculate bytes remaining
		unsigned int remaining = (fileSize - (i*packetSplitValue));
		// What is the packet size?
		unsigned int  pktSize = (remaining > packetSplitValue) ? packetSplitValue : remaining;

		// Assign a size to store the packetSplitValue characters, and headers
		splitBuffer[i] = new char [pktSize+8];
		memset(splitBuffer[i], 0, 8*sizeof(char));
		unsigned int check = read(fd, splitBuffer[i]+8, pktSize);

		// Check the return value
		if(check > 0)
		{
			// Seek forward by packetSplitValue bytes 
			if (lseek(fd, (i+1)*pktSize, SEEK_DATA) == -1)
			{
				std::cerr << "Fatal Error: Could not migrate to next sector" << std::endl;
				exit(0);
			}
		}
		// Error in reaad
		else
		{
			std::cerr << "Fatal Error: Could not read from file descriptor" << std::endl;
			exit(0);
		}
 		
 		// Assign next packet
		i++;

		// Stopping condition
		if(pktSize != packetSplitValue)
			flag = 0;

	} while(flag);

	// Return number of packets
	return i;
}

/*
// Debugging Only: Unit test
using namespace std;

// Unit Test: comment out when done
int main(int argc, char const *argv[])
{
	int FileDes = open(argv[1], O_RDONLY);
	if (FileDes == -1)
	{
		cerr << "Fatal Error: Could not opend file. Are you sure you have the right permissions?" << endl;
		exit(0);
	}

	// Buffer to store packets
	char **buf;
	
	// Generate "Header"
	char hd[8];
	strcpy(hd,"hellooo");
	hd[7] = 'o';

	unsigned int iterator = splitFile(FileDes, buf, 10, hd);
	
	for (int j = 0; j < iterator ; j++)
	{
		cout << "Packet " << j << ": " << endl << buf[j] << endl;
	}

	return 0;
}
*/