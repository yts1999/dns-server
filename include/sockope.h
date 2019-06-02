#ifndef SOCKOPE_H
#define SOCKOPE_H

#include <iostream>
#include <cstring>

#include "message.h"
#include "network.h"

const std::string NAME_SERVER_IP = "10.3.9.4";  // DNS 远程服务器

#define SOCKET int
#define UDP_PROTO        0
#define PORT             53
#define MAX_MESSAGE      4096

class SocketOperator {
// 处理Socket连接及报文接受和发送
public:
	SocketOperator();
	~SocketOperator();
	int recvMessage(uint8_t *buffer, sockaddr_in &senderAddr);
	void sendMessage(const Message &message);
	void sendBuffer(SOCKET &socket, uint8_t *buffer, int bufferSize, sockaddr_in addr);
//	void sendServerPac(const Message &message);
	SOCKET hostSocket;
	SOCKET serverSocket;
private:

	sockaddr_in serverAddr;
};

#endif // SOCKOPE_H
