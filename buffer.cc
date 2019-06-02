#include "include/message.h"

void Message::dWord2Buf(uint16_t value, uint8_t *&bufPtr, int &curBufSize) const {
// 从结构体中提取 2 字节信息到字节流
	uint16_t *dWordPtr = (uint16_t *)bufPtr;
	*dWordPtr = (uint16_t)(htons(value));
	curBufSize += sizeof(uint16_t);
	bufPtr += sizeof(uint16_t);
	if (curBufSize > BUF_SIZE) {
		std::cout << "Buffer Overflow" << std::endl;
		exit(0);
	}
}

void Message::qWord2Buf(uint32_t value, uint8_t *&bufPtr, int &curBufSize) const {
// 从结构体中提取 4 字节信息到字节流
	uint32_t *qWordPtr = (uint32_t *)bufPtr;
	*qWordPtr = (uint32_t)(htonl(value));
	curBufSize += (int)sizeof(uint32_t);
	bufPtr += sizeof(uint32_t);
	if (curBufSize > BUF_SIZE) {
		std::cout << "Buffer Overflow" << std::endl;
		exit(0);
	}
}

void Message::setHeaderBuf(uint16_t &headerBuf) const {
// 将报文首部提出至字节流 
	headerBuf |= (header.qr     <<  15);
	headerBuf |= (header.opCode <<  11);
	headerBuf |= (header.aa     <<  10);
	headerBuf |= (header.tc     <<   9);
	headerBuf |= (header.rd     <<   8);
	headerBuf |= (header.ra     <<   7);
	headerBuf |= (header.z      <<   4);
	headerBuf |= (header.rCode  <<   0);
}

void Message::question2Buf(uint8_t *&bufPtr, int &curBufSize) const {
// 从结构体中将问题字段提取至缓冲区
	for (auto iter = question.begin(); iter != question.end(); iter++) {
		const char *nameStr = iter->QName.c_str();
		size_t nameLen = iter->QName.size();
		memcpy((void *)bufPtr, (void *)nameStr, nameLen);
		curBufSize += nameLen;
		bufPtr += nameLen;
		dWord2Buf(iter->QType, bufPtr, curBufSize);
		dWord2Buf(iter->QClass, bufPtr, curBufSize);
	}
}

void Message::resource2Buf(std::vector<Resource> resourceVec, 
                           uint8_t *&bufPtr, int &curBufSize) const {
// 提取资源字段到缓冲区
	if (resourceVec.size() == 0) {
		return;
	}
	for (auto iter = resourceVec.begin(); iter != resourceVec.end(); iter++) {
		const char *nameStr = iter->name.c_str();
		size_t nameLen = iter->name.size();
		curBufSize += nameLen;
		
		//memcpy((void *)bufPtr, (void *)nameStr, nameLen);
		strcpy((char *)bufPtr, iter->name.c_str());
		bufPtr += nameLen;

		dWord2Buf(iter->type, bufPtr, curBufSize);
		dWord2Buf(iter->clas, bufPtr, curBufSize);
		qWord2Buf(iter->ttl, bufPtr, curBufSize);
		dWord2Buf((uint16_t)(iter->rData.size()), bufPtr, curBufSize);

		uint8_t *dataArr = iter->rData.data();
		size_t dataSize = iter->rData.size();
		
		curBufSize += dataSize;
		memcpy((void *)bufPtr, (void *)dataArr, dataSize);
		bufPtr += dataSize;
	}
}

void Message::pac2Buf(uint8_t *buf, int &bufLen) const {
// 从结构体中将 DNS 报文的全部字段提取到缓冲区
	uint16_t headerBuf = 0;
	bufLen = 0;
	setHeaderBuf(headerBuf);
	dWord2Buf(header.id, buf, bufLen);
	dWord2Buf(headerBuf, buf, bufLen);
	dWord2Buf((uint16_t)question.size(), buf, bufLen);
	dWord2Buf((uint16_t)answer.size(), buf, bufLen);
	dWord2Buf((uint16_t)authority.size(), buf, bufLen);
	dWord2Buf((uint16_t)additional.size(), buf, bufLen);
	question2Buf(buf, bufLen);
	
	resource2Buf(answer, buf, bufLen);
	resource2Buf(authority, buf, bufLen);
	resource2Buf(additional, buf, bufLen);
}