#include "message.h"
#include <cstring>

void Message::dWord2Buf(unsigned short value, unsigned char *&bufPtr, int &curBufSize) const {
	unsigned short *dWordPtr = (unsigned short *)bufPtr;
	*dWordPtr = (unsigned short)(htons(value));
	curBufSize += sizeof(unsigned short);
	bufPtr += sizeof(unsigned short);
	if (curBufSize > BUF_SIZE) {
		std::cout << "Buffer Overflow" << std::endl;
		exit(0);
	}
}

void Message::qWord2Buf(unsigned int value, unsigned char *&bufPtr, int &curBufSize) const {
	unsigned int *qWordPtr = (unsigned int *)bufPtr;
	*qWordPtr = (unsigned int)(htonl(value));
	curBufSize += (int)sizeof(unsigned int);
	bufPtr += sizeof(unsigned int);
	if (curBufSize > BUF_SIZE) {
		std::cout << "Buffer Overflow" << std::endl;
		exit(0);
	}
}

void Message::setHeaderBuf(unsigned short &headerBuf) const {
	headerBuf |= (header.qr     <<  15);
	headerBuf |= (header.opCode <<  11);
	headerBuf |= (header.aa     <<  10);
	headerBuf |= (header.tc     <<   9);
	headerBuf |= (header.rd     <<   8);
	headerBuf |= (header.ra     <<   7);
	headerBuf |= (header.z      <<   4);
	headerBuf |= (header.rCode  <<   0);
}

void Message::question2Buf(unsigned char *&bufPtr, int &curBufSize) const {
	for (auto iter = question.begin(); iter != question.end(); iter++) {
		const char *nameStr = iter->QName.c_str();
		size_t nameLen = iter->QName.size();
		memcpy((void *)bufPtr, (void *)nameStr, nameLen);
		curBufSize += nameLen;
		bufPtr += nameLen;
		dWord2Buf(iter->QClass, bufPtr, curBufSize);
		dWord2Buf(iter->QType, bufPtr, curBufSize);
	}
}

void Message::resource2Buf(std::vector<Resource> resourceVec,
                           unsigned char *&bufPtr, int &curBufSize) const {
	for (auto iter = resourceVec.begin(); iter != resourceVec.end(); iter++) {
		const char *nameStr = iter->name.c_str();
		size_t nameLen = iter->name.size();
		curBufSize += nameLen;
		bufPtr += nameLen;
		memcpy((void *)bufPtr, (void *)nameStr, nameLen);

		dWord2Buf(iter->type, bufPtr, curBufSize);
		dWord2Buf(iter->clas, bufPtr, curBufSize);
		qWord2Buf(iter->ttl, bufPtr, curBufSize);
		dWord2Buf((unsigned short)(iter->rData.size()), bufPtr, curBufSize);

		unsigned char *dataArr = iter->rData.data();
		size_t dataSize = iter->rData.size();
		bufPtr += dataSize;
		curBufSize += dataSize;
		memcpy((void *)bufPtr, (void *)dataArr, dataSize);
	}
}

void Message::pac2Buf(unsigned char *buf, int &bufLen) const {
	unsigned short headerBuf = 0;
	bufLen = 0;
	setHeaderBuf(headerBuf);
	dWord2Buf(header.id, buf, bufLen);
	dWord2Buf(headerBuf, buf, bufLen);
	dWord2Buf((unsigned short)question.size(), buf, bufLen);
	dWord2Buf((unsigned short)answer.size(), buf, bufLen);
	dWord2Buf((unsigned short)authority.size(), buf, bufLen);
	dWord2Buf((unsigned short)additional.size(), buf, bufLen);
	question2Buf(buf, bufLen);
	resource2Buf(answer, buf, bufLen);
	resource2Buf(authority, buf, bufLen);
	resource2Buf(additional, buf, bufLen);
}
