#ifndef MESSAGE_H
#define MESSAGE_H

#include "network.h"
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <map>

#include "utils.h"

#define BUF_SIZE 4096   // 缓冲区最大容量

class Message {
// DNS 报文结构
public:
	Message();
	Message(const uint8_t *buffer, int bufferSize, const sockaddr_in *sendAddr);
//          DNS Header
//           0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                       ID                      |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                    QDCOUNT                    |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                    ANCOUNT                    |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                    NSCOUNT                    |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                    ARCOUNT                    |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	struct Header {
		uint16_t    id;       // 16 bit
		bool        qr;       // 1 bit
		uint8_t     opCode;   // 4 bit
		bool        aa;       // 1 bit
		bool        tc;       // 1 bit
		bool        rd;       // 1 bit
		bool        ra;       // 1 bit
		uint8_t     z;        // 3 bit
		uint8_t     rCode;    // 4 bit

		uint16_t    qdCount;  // 16 bit
		uint16_t    anCount;  // 16 bit
		uint16_t    nsCount;  // 16 bit
		uint16_t    arCount;  // 16 bit
	};

//          DNS Question
//            0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                                               |
//          /                     QNAME                     /
//          /                                               /
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                     QTYPE                     |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                     QCLASS                    |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

	struct Question {
		std::string QName;
		uint16_t    QType;    // 16 bit
		uint16_t    QClass;   // 16 bit

		std::string getName() const;
	};

//          DNS Resource
//            0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                                               |
//          /                                               /
//          /                      NAME                     /
//          |                                               |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                      TYPE                     |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                     CLASS                     |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                      TTL                      |
//          |                                               |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//          |                   RDLENGTH                    |
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
//          /                     RDATA                     /
//          /                                               /
//          +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

	struct Resource {
		std::string name;
		uint16_t    type;      // 16 bit
		uint16_t    clas;      // 16 bit
		uint32_t    ttl;       // 32 bit
		uint16_t    rdLength;  // 16 bit
		std::vector<uint8_t> rData;
	};

	sockaddr_in     sendAddr;
	Header          header;

	std::vector<Question> question;
	std::vector<Resource> answer;
	std::vector<Resource> authority;
	std::vector<Resource> additional;

	static std::string getDomainName(const std::string &name);
	void pac2Buf(uint8_t *buf, int &bufLen) const;
	Message(const sockaddr_in addr, const uint8_t *buf, int bufLen);

private:
	void setHeaderBuf(uint16_t &headerBuf) const;
	void dWord2Buf(uint16_t value, uint8_t *&bufPtr, int &curBufSize) const;
	void qWord2Buf(uint32_t value, uint8_t *&bufPtr, int &curBufSize) const;
	void section2Buf(uint8_t *value, uint8_t *&bufPtr, int &curBufSize) const;
	void question2Buf(uint8_t *&bufPtr, int &curBufSize) const;
	void resource2Buf(std::vector<Resource> resourceVec, uint8_t *&bufPtr, int &curBufSize) const;

	void getHeader(uint8_t *&bufPtr);
	void getQuestion(uint8_t *&bufPtr);
	void getResource(uint8_t *&bufPtr, std::vector<Resource> &resourceVec, uint16_t count);
	void getdWord(uint16_t &target, uint8_t *&bufPtr);
	void getqWord(uint32_t &target, uint8_t *&bufPtr);

	uint8_t *messagePtr;
	void handleLabel(uint8_t *&namePtr, std::string &nameBuf);
};

std::ostream &operator << (std::ostream &os,
                               const Message::Header &header);

#endif // MESSAGE_H