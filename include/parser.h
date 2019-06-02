#ifndef PARSER_H
#define PARSER_H

#include <string>

#include "message.h"
#include "sockope.h"
#include "table.h"
#include "record.h"
#include "utils.h"

enum MsgType {
// 报文基本类型
	STD_QUERY,
	RESPONSE,
	OTHER
};

class Parser {
// 报文处理逻辑实现
public:
	Parser();
	void parse();
	void receive();
	void writeBack();
private:
	Message message;
	uint8_t *buffer;
	int bufferSize;
	bool isResponse();
	bool isStdQuery();
	MsgType getMsgType();
	void parseQuery();
	void prepAnswerMsg(QuadWordIP answerIP, std::string &name);
	void prepAnswerMsg6(HexWordIP answerIP, std::string &name);
	void send2Server();
	void recvServer();
	void setServerAddr();

	HostsTable *table;
	SocketOperator *sockOperator;
	RecordTable *records;

	sockaddr_in serverAddr;
};

#endif
