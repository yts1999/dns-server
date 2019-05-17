#ifndef PARSER_H
#define PARSER_H

#include <string>

#include "message.h"
#include "sockope.h"
#include "table.h"
#include "record.h"

enum MsgType {
	STD_QUERY,
	RESPONSE,
	OTHER
};

class Parser {
public:
	Parser();
	void parse();
	void receive();
private:
	Message message;
	uint8_t *buffer;
	int bufferSize;
	bool isResponse();
	bool isStdQuery();
	MsgType getMsgType();
	void parseQuery();
	void prepAnswerMsg(DoubleWordIP answerIP, std::string &name);
	void send2Server();
	void recvServer();
	void setServerAddr();

	HostsTable *table;
	SocketOperator *sockOperator;
	RecordTable *records;

	sockaddr_in serverAddr;
};

#endif