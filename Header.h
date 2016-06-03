#include <cstdlib>
#include <cstring>
#include <string>

#include <iostream>

#include <stdint.h>
#include <arpa/inet.h>

typedef struct tcpHeader
{
	uint16_t seq_no;
	uint16_t ack_no;
	uint16_t cwnd;
	uint16_t flags;
} Headers;

enum
{
    ACK = 1 << 0,
    SYN = 1 << 1,
    FIN = 1 << 2
};

void setHeader(uint16_t seq, uint16_t ack, uint16_t winSize, uint16_t flag, Headers &mod);
void getFlags(uint16_t flag, bool &ACK, bool &SYN, bool &FIN);
uint16_t genRand();
uint16_t genNextNum (uint16_t prev);
void genPacket(char*& packet, Headers& headerVal, char* payload);
void parsePacket(char*& packet, Headers& headerVal, char*& payload);