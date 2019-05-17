#include "include/parser.h"

Parser::Parser() {
	table = new HostsTable();
	sockOperator = new SocketOperator();
	records = new RecordTable();
	buffer = new uint8_t(BUF_SIZE);
	setServerAddr();
}

void Parser::receive() {
	sockaddr_in clientAddr;
	int bufferLen;
	bufferLen = sockOperator->recvMessage(buffer, clientAddr);
	bufferSize = bufferLen;
	message = Message(clientAddr, buffer, bufferSize);
}

void Parser::setServerAddr() {
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	if (!inet_pton(AF_INET, NAME_SERVER_IP.c_str(), (void *)&(serverAddr.sin_addr))) {
		std::cout << "Server IP Error" << std::endl;
		exit(0);
	}
}

void Parser::parse() {
	MsgType type = getMsgType();
	switch (type) {
	case RESPONSE:
		recvServer();
		break;
	case OTHER:
		send2Server();
		break;
	case STD_QUERY:
		parseQuery();
	//	send2Server();
		break;
	} 
}

void Parser::parseQuery() {
//	Message bufferMessage = message;
	bool bad = 0;
	for (auto query = message.question.begin(); query != message.question.end(); ++query) {
		
		if (!(query->QType == 1 && query->QClass == 1)) {
			std::cout << "Query unacceptable, to name server..." << std::endl;
			send2Server();
			return;
		} else {
		std::cout << "Type: " << query->QType << " Class: " << query->QClass << std::endl;
			std::string domainName = Message::getDomainName(query->QName);
			DoubleWordIP ipReturn;
			if (table->findHost(domainName, ipReturn)) {
				prepAnswerMsg(ipReturn, query->QName);
			} else {
				std::cout << "cannot find locally, to name server..." << std::endl;
				send2Server();
				bad = 1;
				break;
			}
		}
	}
	if (!bad) sockOperator->sendMessage(message);
}

MsgType Parser::getMsgType() {
	if (isResponse()) {
		return RESPONSE;
	} else if (isStdQuery()) {
		return STD_QUERY;
	} else {
		return OTHER;
	}
}

bool Parser::isResponse() {
	return message.header.qr == 1;
}

bool Parser::isStdQuery() {
	return (
		message.header.qr == 0 &&
		message.header.opCode == 0
	);
}

void Parser::prepAnswerMsg(DoubleWordIP answerIP, std::string &name) {
	std::cout << "IP: " << answerIP << std::endl;
	message.header.qr = true;
	message.header.aa = true;
	message.header.ra = true;
	message.header.anCount++;
	Message::Resource answerBuffer;
	answerBuffer.name = name;
	answerBuffer.type = 1;
	answerBuffer.clas = 1;
	answerBuffer.ttl  = 120;
	answerBuffer.rdLength++;
	answerBuffer.rData.assign((uint8_t *)&answerIP, (uint8_t *)(&answerIP + 1));
	message.answer.push_back(answerBuffer);
}

void Parser::send2Server() {
	uint16_t recordID = records->insertRecord(message);
	uint16_t *IDPtr = (uint16_t *)buffer;
	*IDPtr = htons(recordID);
	sockOperator->sendBuffer(buffer, bufferSize, serverAddr);
}

void Parser::recvServer() {
	RecordTable::Record record;
		std::cout << "Parsing server message..." << std::endl;
	if (!records->findRecord(message.header.id, record)) {
		std::cout << "Record Not Found" << std::endl;
		exit(0);
	} else {
		std::cout << "Receieved ID: " << message.header.id << std::endl;
		uint16_t *IDPtr = (uint16_t *)buffer;
		*IDPtr = htons(record.id);
		
		sockOperator->sendBuffer(buffer, bufferSize, record.sendAddr);
	}
}



