#include "Header.h"

void setHeader(uint16_t seq, uint16_t ack, uint16_t winSize, uint16_t flag, Headers &mod)
{
	mod.seq_no = seq;
	mod.ack_no = ack;
	mod.cwnd = winSize;
	mod.flags = flag;
}

uint16_t genRand()
{
	return (rand() % 0xFFFF);
}

// Wrap around, otherwise show the next value
uint16_t genNextNum (uint16_t prev)
{
	if(prev == 0xFFFF)
	{
		return 0x0;
	}
	else
	{
		return (prev + 1);
	}
}

void genPacket(char*& packet, Headers& headerVal, char* payload)
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
	packet = new char[strlen(payload)+8];
	
	// Copy data to the buffer
	memset(packet, 0, (strlen(payload)+8));
	memcpy(packet, &s, sizeof(uint16_t));
	memcpy((packet+2), &a, sizeof(uint16_t));
	memcpy((packet+4), &c, sizeof(uint16_t));
	memcpy((packet+6), &f, sizeof(uint16_t));
	strncpy((packet+8), payload, strlen(payload));
}

void parsePacket(char*& packet, Headers& headerVal, char*& payload)
{
	// Store network byte ordering
	uint16_t s;
	uint16_t a;
	uint16_t c;
	uint16_t f;

	// Copy over the network byte order
	memcpy(&s, packet, sizeof(uint16_t));
	memcpy(&a, (packet+2), sizeof(uint16_t));
	memcpy(&c, (packet+4), sizeof(uint16_t));
	memcpy(&f, (packet+6), sizeof(uint16_t));

	// Convert to uint16_t
	headerVal.seq_no = ntohs(s);
	headerVal.ack_no = ntohs(a);
	headerVal.cwnd = ntohs(c);
	headerVal.flags = ntohs(f);

	// Copy over the payload
	payload = new char[strlen(packet+8)];
	strcpy(payload, packet+8);
}