#include "Header.h"

void setHeader(uint16_t seq, uint16_t ack, uint16_t winSize, uint16_t flag, Headers &mod)
{
	mod.seq_no = seq;
	mod.ack_no = ack;
	mod.cwnd = winSize;
	mod.flags = flag;
}

// Generate a random starting sequence number
uint16_t genRand(int seed) 
{ 
	srand(seed);
    return (rand() % 0x7800);
} 

// Generate the next sequence number for n bytes received
uint16_t genNextNum (uint16_t prev, uint16_t incBytes) 
{ 
	return (prev+incBytes % 0x7800);
} 

void genPacket(char*& packet, Headers& headerVal, char* payload, unsigned int pktMSS)
{
	// Store network byte ordering
	uint16_t s;
	uint16_t a;
	uint16_t c;
	uint16_t f;

	// Convert to network byte ordering
	s = htons(headerVal.seq_no);
	a = htons(headerVal.ack_no);
	c = htons(headerVal.cwnd);
	f = htons(headerVal.flags);

	// Malloc the payload
	packet = new char[pktMSS + 8];
	
	// Copy data to the buffer
	memset(packet, 0, (pktMSS+8));
	memcpy(packet, &s, sizeof(uint16_t));
	memcpy((packet+2), &a, sizeof(uint16_t));
	memcpy((packet+4), &c, sizeof(uint16_t));
	memcpy((packet+6), &f, sizeof(uint16_t));
	strncpy((packet+8), payload, pktMSS);
}

void parsePacket(char* packet, Headers& headerVal, char*& payload, unsigned int pktMSS)
{
	// Store network byte ordering
	uint16_t s;
	uint16_t a;
	uint16_t c;
	uint16_t f;

	// Copy over the network byte order
	memcpy(&s, ( packet ), sizeof(uint16_t));
	memcpy(&a, (packet+2), sizeof(uint16_t));
	memcpy(&c, (packet+4), sizeof(uint16_t));
	memcpy(&f, (packet+6), sizeof(uint16_t));

	// Convert to uint16_t
	headerVal.seq_no = ntohs(s);
	headerVal.ack_no = ntohs(a);
	headerVal.cwnd = ntohs(c);
	headerVal.flags = ntohs(f);

	// Copy over the payload
	payload = new char[pktMSS];
	strncpy(payload, packet+8, pktMSS);
}

void getFlags(uint16_t flag, bool &ACKFlag, bool &SYNFlag, bool &FINFlag)
{
	ACKFlag = flag & ACK;
	SYNFlag = flag & SYN;
	FINFlag = flag & FIN;
}