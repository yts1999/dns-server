#include "sockope.h"

SocketOperator::SocketOperator() {
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
			std::cout << "<Local binding Success, Start listening..." << std::endl;
		}
	}
}

SocketOperator::~SocketOperator() {
	close(hostSocket);
}

int SocketOperator::recvMessage(uint8_t *buffer, sockaddr_in &senderAddr) {
	memset(buffer, 0, sizeof(buffer));
	socklen_t addrLen = sizeof(sockaddr_in);
	sockaddr_in clientAddr;

	int recvLen = recvfrom(hostSocket, (char *)buffer, MAX_MESSAGE,
	                       0, (sockaddr *)&clientAddr, &addrLen);
	if (recvLen <= 0) {
		std::cout << "Connention Error" << std::endl;
		exit(0);
	} else {

	}
	senderAddr = clientAddr;
	return recvLen;
}

void SocketOperator::sendMessage(const Message &message) {
	uint8_t buffer[MAX_MESSAGE];
	int bufSize;
	message.pac2Buf(buffer, bufSize);
	if (sendto(hostSocket, buffer, bufSize, 0,
	    (const sockaddr*)&message.sendAddr, sizeof(sockaddr_in)) < 0) {
			std::cout << "Send Error" << std::endl;
			exit(0);
		}
}

void SocketOperator::sendBuffer(uint8_t *buffer, int bufferSize, sockaddr_in addr) {
	if (sendto(hostSocket, buffer, bufferSize, 0,
	    (const sockaddr *)&addr, sizeof(addr)) < 0) {
			std::cout << "Send Buffer Error" << std::endl;
			exit(0);
		} else {
			std::cout << "Send buffer seccess!" << std::endl;
		}
}
