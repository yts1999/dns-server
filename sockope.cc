#include "include/sockope.h"

SocketOperator::SocketOperator() {
// socket 处理模块初始化
	hostSocket = socket(PF_INET, SOCK_DGRAM, UDP_PROTO);
	if (hostSocket >= 0) {
		sockaddr_in hostAddr;
		hostAddr.sin_family = AF_INET;
		hostAddr.sin_port = htons(PORT);
		hostAddr.sin_addr.s_addr = 0;//inet_addr("127.0.0.1");

		if (bind(hostSocket, (sockaddr *)&hostAddr, sizeof(sockaddr_in))) {
			std::cout << "Local binding Error" << std::endl;
			exit(1);
		} else {
			std::cout << "------Local binding Success, Start listening...------" << std::endl;
		}
	}
// 接收 socket 建立
	serverSocket = socket(PF_INET, SOCK_DGRAM, UDP_PROTO);
	if (serverSocket >= 0) {
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(PORT);
		serverAddr.sin_addr.s_addr = inet_addr(NAME_SERVER_IP.c_str());
			std::cout << "------------Server socket create Success------------" << std::endl;
	}
}

SocketOperator::~SocketOperator() {
// 关闭 socket
	close(hostSocket);
	close(serverSocket);
}

int SocketOperator::recvMessage(uint8_t *buffer, sockaddr_in &senderAddr) {
// 从 socket 中接收一条报文
	memset(buffer, 0, BUF_SIZE);
	socklen_t addrLen = sizeof(sockaddr_in);
	sockaddr_in clientAddr;

	int recvLen = recvfrom(hostSocket, (char *)buffer, MAX_MESSAGE, 
	                       0, (sockaddr *)&clientAddr, &addrLen);
	if (recvLen <= 0) {
		std::cout << "Connection Error" << std::endl;
		exit(0);
	} else {

	}
	senderAddr = clientAddr;
	return recvLen;

}

void SocketOperator::sendMessage(const Message &message) {
// 将一条报文置入缓冲区，并发送到指定地址处
	uint8_t buffer[MAX_MESSAGE];
	memset(buffer, 0, BUF_SIZE);
	int bufSize;
	message.pac2Buf(buffer, bufSize);

	if (sendto(hostSocket, buffer, bufSize, 0, 
	    (const sockaddr*)&message.sendAddr, sizeof(sockaddr_in)) < 0) {
			std::cout << "Send Error" << std::endl;
			exit(0);
		}
}

void SocketOperator::sendBuffer(SOCKET &socket, uint8_t *buffer, int bufferSize, sockaddr_in addr) {
// 将缓冲区直接发送到指定地址
	if (sendto(socket, buffer, bufferSize, 0,
	    (const sockaddr *)&addr, sizeof(addr)) < 0) {
			std::cout << "Send Buffer Error" << std::endl;
			exit(0);
		} else {
//			std::cout << "Send buffer seccess!" << std::endl;
		}
}


