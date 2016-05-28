#include "TCPDatagram.h"

// Default constructor
TCPDatagram::TCPDatagram() {
	// Initalize all pointers to NULL
	this->path = NULL;
	this->host = NULL;
	this->getReq = NULL;
	this->protocolVersion = NULL;

	// Error code to 0
	//this->error = 0;
}

// Destructor
TCPDatagram::~TCPDatagram() {
	this->clear();
}

// Parameterized Constructor
TCPDatagram::TCPDatagram(char* fpath, char* fhost) {
	// Set values
	this->path = new char [std::strlen(fpath)];
	std::strcpy(this->path, fpath);
	this->host = new char [std::strlen(fhost)];
	std::strcpy(this->host, fhost);
	this->protocolVersion = new char[9];
	std::strcpy(this->protocolVersion, "HTTP/1.0");

}

char* TCPDatagram::genReq() {
	std::string get = "";
	get += "GET ";
	get += this->path;
	get += " ";
	get += this->protocolVersion;
	get += "\r\nHost: ";
	get += this->host;
	get += "\r\n\r\n";
	this->getReq = new char [get.length()+1];
	std::strcpy(this->getReq, get.c_str());
	return this->getReq;
}

// Clear members
void TCPDatagram::clear() {
	// Delete all mallocd pointers, set error to 0,and protocolVersion to HTTP/1.0
	delete this->path;
	delete this->host;
	delete this->protocolVersion;
	delete this->getReq;
	// this->error = 0;
	this->path = NULL;
	this->host = NULL;
	this->getReq = NULL;
	this->protocolVersion = NULL;
}

// Accessors
void TCPDatagram::setPath(char* buf) { this->path = buf; }
char* TCPDatagram::getPath() { return this->path; }

void TCPDatagram::setHost(char* buf) { this->host = buf; }
char* TCPDatagram::getHost() {return this->host; }

void TCPDatagram::setProtocolVersion(char* buf) { this->protocolVersion = buf; }
char* TCPDatagram::getProtocolVersion() {return this->protocolVersion; }
