#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <locale>
#include <stdlib.h>     /* atoi */
#include "TCPDatagram.h"

enum BuilderState { SEQ_NUM, ACK_NUM, WINDOW_SIZE, FLAGS, DATA, DONE };

class TCPDatagramBuilder {

protected:
	// Protected datamembers
	TCPDatagram* datagram;
	BuilderState currentState;
	bool complete; // is datagram complete?

	// process input stream
	void process();

public:
	std::string currentString; // input stream

	// Default Constructor
	TCPDatagramBuilder();
	// Constructor to initialize a base datagram
	TCPDatagramBuilder(TCPDatagram* datagram);
	// Constructor with an initial string to parse
	TCPDatagramBuilder(char* initialString);
	// Destructor
	~TCPDatagramBuilder();

	// add buffer to input stream and process
	void feed(char* buffer);

	TCPDatagram* getDatagram();
	bool isComplete();
};
