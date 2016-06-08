#include <cstdlib>
#include <cstring>
#include <string>

#include <iostream>

#include <stdint.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// tcpHeader structure
typedef struct tcpHeader
{
	uint16_t seq_no;
	uint16_t ack_no;
	uint16_t cwnd;
	uint16_t flags;
} Headers;

// Flag enumerators
enum
{
    ACK = 0b100,
    SYN = 0b010,
    FIN = 0b001,
};

void setHeader(uint16_t seq, uint16_t ack, uint16_t winSize, uint16_t flag, Headers &mod);
void getFlags(uint16_t flag, bool &ACKFlag, bool &SYNFlag, bool &FINFlag);
void genPacket(char*& packet, Headers& headerVal, char* payload, unsigned int pktMSS);//, unsigned int pktMSS);
void parsePacket(char* packet, Headers& headerVal, char*& payload, unsigned int pktMSS);//, unsigned int pktMSS);
uint16_t genRand(int seed);
uint16_t genNextNum (uint16_t prev, uint16_t incBytes);
