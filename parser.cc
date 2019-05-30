#include "parser.h"

Parser::Parser() {
	table = new HostsTable();
	sockOperator = new SocketOperator();
	records = new RecordTable();
	buffer = new uint8_t[BUF_SIZE];
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
			std::string domainName = Message::getDomainName(query->QName);		
			std::cout << "Query name:   | " << domainName << " --> ";

			DoubleWordIP ipReturn;
			if (table->findHost(domainName, ipReturn)) {
				std::cout << "Domain name found" << std::endl;
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
		std::cout << "Message type: | [RESPONSE ]" << std::endl;
		return RESPONSE;
	} else if (isStdQuery()) {
		std::cout << "Message type: | [STD_QUERY]" << std::endl;
		return STD_QUERY;
	} else {
		std::cout << "Message type: | [  OTHER  ]" << std::endl;
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
	if (answerIP == (u_int32_t)0x00000000) {
		message.header.rCode = 3;
		return;
	}
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
	records->eraseTimeout();
	uint16_t recordID = records->insertRecord(message);
	uint16_t *IDPtr = (uint16_t *)buffer;
	*IDPtr = htons(recordID);
	sockOperator->sendBuffer(sockOperator->hostSocket, buffer, bufferSize, serverAddr);
}

void Parser::recvServer() {
	RecordTable::Record record;
	if (!records->findRecord(message.header.id, record)) {
		std::cout << "Record Not Found" << std::endl;
		exit(0);
	} else {

		uint16_t *IDPtr = (uint16_t *)buffer;
		*IDPtr = htons(record.id);

		DoubleWordIP ipRecvd;		
		ipRecvd = *(DoubleWordIP *)(buffer + bufferSize - 4);

		if (!STATIC) {

		if (table->getTableSize() < MAX_TABLE) {
			if (ipRecvd != (uint32_t)0x00000000)
				table->insertName(record.domainName, ipRecvd);
		} else {
			if (ipRecvd != (uint32_t)0x00000000) {
				table->eraseName();
				table->insertName(record.domainName, ipRecvd);
			}
		}

		}

		sockOperator->sendBuffer(sockOperator->hostSocket, buffer, bufferSize, record.sendAddr);
	}
}

void Parser::writeBack() {
	if (!STATIC) {
	
	table->writeFile();
	table->writeFreqFile();

	}
}



