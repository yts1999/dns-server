#include "include/parser.h"

Parser::Parser() {
// 报文处理逻辑初始化
	table = new HostsTable();
	sockOperator = new SocketOperator();
	records = new RecordTable();
	buffer = new uint8_t[BUF_SIZE];
	setServerAddr();
}

void Parser::receive() {
// 从 socket 处理模块中接受一条报文
	sockaddr_in clientAddr;
	int bufferLen;
	bufferLen = sockOperator->recvMessage(buffer, clientAddr);
	bufferSize = bufferLen;
	message = Message(clientAddr, buffer, bufferSize);
}

void Parser::setServerAddr() {
// 初始化远端服务器 IP 地址
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	if (!inet_pton(AF_INET, NAME_SERVER_IP.c_str(), (void *)&(serverAddr.sin_addr))) {
		std::cout << "Server IP Error" << std::endl;
		exit(0);
	}
}

void Parser::parse() {
// 报文处理逻辑
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
//	进入标准询问处理逻辑
	bool bad = 0;
	for (auto query = message.question.begin(); query != message.question.end(); ++query) {

		if (query->QType == 1 && query->QClass == 1) {
		// 标准 IPv4 处理逻辑
			std::string domainName = Message::getDomainName(query->QName);
			std::cout << "Query name:   | " << domainName << " --> ";

			QuadWordIP ipReturn;
			if (table->findHost(domainName, ipReturn)) {
				std::cout << "Domain name found" << std::endl;
				prepAnswerMsg(ipReturn, query->QName);
			} else {
				std::cout << "Cannot find locally, to name server..." << std::endl;
				send2Server();
				bad = 1;
				break;
			}
		} else if (query->QType == 28 && query->QClass == 1) {
		// IPv6 处理逻辑
			std::string domainName = Message::getDomainName(query->QName);
			std::cout << "Query name:   | " << domainName << " --> ";

			HexWordIP ipReturn;
				
			if (table->findHost6(domainName, ipReturn)) {
				std::cout << "Domain name found" << std::endl;
				prepAnswerMsg6(ipReturn, query->QName);
			} else {
				std::cout << "Cannot find locally, to name server..." << std::endl;
				send2Server();
				bad = 1;
				break;
			}
		}
		else {
		// 其它类型的询问处理逻辑
			std::cout << "Query unacceptable, to name server..." << std::endl;
			send2Server();
			return;
		}
	}
	// 有无法处理的查询则转发至远端服务器
	if (!bad) sockOperator->sendMessage(message);
}

MsgType Parser::getMsgType() {
// 通过 DNS 报文首部判断报文类型
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

void Parser::prepAnswerMsg(QuadWordIP answerIP, std::string &name) {
// 将一条应答字段推入应答报文
	if (answerIP == (uint32_t)0x0) {
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
	answerBuffer.rdLength = 4;
	answerBuffer.rData.assign((uint8_t *)&answerIP, (uint8_t *)(&answerIP + 1));
	message.answer.push_back(answerBuffer);
}

void Parser::prepAnswerMsg6(HexWordIP answerIP, std::string &name) {
// IPv6 地址查询的应答逻辑
	if (answerIP == (__uint128_t)0x0) {
		message.header.rCode = 3;
		return;
	}
	message.header.qr = true;
	message.header.aa = true;
	message.header.ra = true;
	message.header.anCount++;
	Message::Resource answerBuffer;
	answerBuffer.name = name;
	answerBuffer.type = 28;
	answerBuffer.clas = 1;
	answerBuffer.ttl  = 120;
	answerBuffer.rdLength = 16;
	answerBuffer.rData.assign((uint8_t *)&answerIP, (uint8_t *)(&answerIP + 1));
	message.answer.push_back(answerBuffer);
}

void Parser::send2Server() {
// 将报文转发至远端服务器
	records->eraseTimeout();
	uint16_t recordID = records->insertRecord(message);
	uint16_t *IDPtr = (uint16_t *)buffer;
	*IDPtr = htons(recordID);
	sockOperator->sendBuffer(sockOperator->hostSocket, buffer, bufferSize, serverAddr);
}

void Parser::recvServer() {
// 收到远端服务器的应答报文之后的处理逻辑
	RecordTable::Record record;
	if (!records->findRecord(message.header.id, record)) {
		std::cout << "Record Not Found" << std::endl;
		exit(0);
	} else {

		uint16_t *IDPtr = (uint16_t *)buffer;
		*IDPtr = htons(record.id);
		if (!STATIC //&& message.header.nsCount == 0
		            //&& message.header.arCount == 0
				    && message.header.anCount  > 0) {
		// 写入本地 cache
			for (int i = 0; i < message.header.anCount; i++)
				if (message.answer[i].type == 1 && message.answer[i].clas == 1) {
				// IPv4 记录写入
					uint8_t *dataArr = message.answer[i].rData.data();
					QuadWordIP ipRecvd = *(QuadWordIP *)dataArr;
					if (ipRecvd == (uint32_t)0x0)
						continue;
					if (table->getTableSize() < MAX_TABLE) {
						table->insertName(record.domainName, ipRecvd);
					}
					else {
						table->eraseName();
						table->insertName(record.domainName, ipRecvd);
					}
					break;
				}
				else if (message.answer[i].type == 28 && message.answer[i].clas == 1) {
				// IPv6 记录写入
					uint8_t *dataArr = message.answer[i].rData.data();
					HexWordIP ipRecvd = *(HexWordIP *)dataArr;
					if (ipRecvd == (__uint128_t)0x0)
						continue;
					if (table->getTableSize6() < MAX_TABLE) {
						table->insertName6(record.domainName, ipRecvd);
					}
					else {
						table->eraseName6();
						table->insertName6(record.domainName, ipRecvd);
					}
					break;
				}
		}

		sockOperator->sendBuffer(sockOperator->hostSocket, buffer, bufferSize, record.sendAddr);
	}
}

void Parser::writeBack() {
// 将记录表写回文件
	if (!STATIC) {

	table->writeFile();
	table->writeFreqFile();
	table->writeFile6();
	table->writeFreqFile6();

	}
}
