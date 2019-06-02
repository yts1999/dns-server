#include "include/message.h"

Message::Message() {

}

Message::Message(const sockaddr_in addr, const uint8_t *buf, int bufLen) :
	sendAddr(addr) {
// 构造函数从缓冲区中解析 DNS 报文到结构体
	answer.clear();
	authority.clear();
	additional.clear();
	uint8_t *ptrBuffer = (uint8_t *)buf;
	messagePtr = ptrBuffer;
	getHeader(ptrBuffer);
	//if (header.qr == 0) {
	getQuestion(ptrBuffer);
	getResource(ptrBuffer, answer, header.anCount);
	getResource(ptrBuffer, authority, header.nsCount);
	getResource(ptrBuffer, additional, header.arCount);
	//}
}

std::string Message::getDomainName(const std::string &name)  {
// 将域名从数字分割形式转换为点分形式
	const char *nameStr = name.c_str();
	std::string buffer;
	while (*nameStr != '\0') {
		int sectionLen = *nameStr;
		nameStr++;
		buffer.append(nameStr, sectionLen);
		nameStr+= sectionLen;
		if (*nameStr != '\0') {
			buffer.append(1, '.');
		}

	}
	toLower(buffer);
	return buffer;
}

std::string Message::Question::getName() const {
	return getDomainName(QName);
}

void Message::getHeader(uint8_t *&bufPtr) {
// 从缓冲区中将 DNS 报文首部提取至结构体
	uint16_t *headerPtr = (uint16_t *)bufPtr;
	header.id          = ntohs(*(headerPtr    ));
	uint16_t headerBuf = ntohs(*(headerPtr + 1));

	header.qr     = (headerBuf & 0x8000) >> 15;
	header.opCode = (headerBuf & 0x7800) >> 11;
	header.aa     = (headerBuf & 0x0400) >> 10;
	header.tc     = (headerBuf & 0x0200) >>  9;
	header.rd     = (headerBuf & 0x0100) >>  8;
	header.ra     = (headerBuf & 0x0080) >>  7;
	header.z      = (headerBuf & 0x0070) >>  4;
	header.rCode  = (headerBuf & 0x000F) >>  0;

	header.qdCount     = ntohs(*(headerPtr + 2));
	header.anCount     = ntohs(*(headerPtr + 3));
	header.nsCount     = ntohs(*(headerPtr + 4));
	header.arCount     = ntohs(*(headerPtr + 5));

	bufPtr += 12;
}

//    Pointer form:
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    | 1  1|                OFFSET                   |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

void Message::handleLabel(uint8_t *&namePtr, std::string &nameBuf) {
// 处理压缩域名
	while (*namePtr != 0)
		//判断标签类型
		if ((*namePtr & 0xc0) == 0xc0) { //ptr
			uint8_t *ptr = namePtr;
			uint16_t offset = 0;
			getdWord(offset, ptr);
			offset ^= 0xc000;
			uint8_t *nextnamePtr = messagePtr + offset;
			// 递归处理
			handleLabel(nextnamePtr, nameBuf);
			namePtr += 2;
			return;
		}
		else {
			uint8_t sectionSize = *namePtr;
			nameBuf.append((char *)namePtr, sectionSize + 1);
			namePtr += (sectionSize + 1);
		}
	nameBuf.append(1, '\0');
	namePtr++;
}

void Message::getQuestion(uint8_t *&bufPtr) {
	for (int count = 0; count < header.qdCount; count++) {
//  将问题字段从缓冲区中提取至结构体
//  parse compressed domain name
		Question questionBuf;
		std::string nameBuf = "";
		handleLabel(bufPtr, nameBuf);
		questionBuf.QName = nameBuf;
		getdWord(questionBuf.QType, bufPtr);
		getdWord(questionBuf.QClass, bufPtr);
		question.push_back(questionBuf);
	}
}

void Message::getResource(uint8_t *&bufPtr, std::vector<Message::Resource> &resourceVec, uint16_t count) {
	for (int _count = 0; _count < count; _count++) {
//  将资源字段从缓冲区中提取至结构体
		Resource resourceBuf;
		std::string nameBuf = "";
		handleLabel(bufPtr, nameBuf);
		resourceBuf.name = nameBuf;
		getdWord(resourceBuf.type, bufPtr);
		getdWord(resourceBuf.clas, bufPtr);
		getqWord(resourceBuf.ttl,  bufPtr);
		getdWord(resourceBuf.rdLength, bufPtr);
		resourceBuf.rData.assign(bufPtr, bufPtr + resourceBuf.rdLength);
		bufPtr += resourceBuf.rdLength;
		resourceVec.push_back(resourceBuf);
	}
}

void Message::getdWord(uint16_t &target, uint8_t *&bufPtr) {
// 将 2 字节的信息从结构体中提取至缓冲区
	uint16_t *dWordPtr = (uint16_t *)bufPtr;
	target = ntohs(*dWordPtr);
	bufPtr += sizeof(uint16_t);
}

void Message::getqWord(uint32_t &target, uint8_t *&bufPtr) {
// 将 4 字节的信息从结构体中提取至缓冲区
	uint32_t *qWordPtr = (uint32_t *)bufPtr;
	target = ntohs(*qWordPtr);
	bufPtr += sizeof(uint32_t);
}

std::ostream &operator << (std::ostream &os,
                               const Message::Header &header)
{
// 重载 DNS 报文首部的输出流运算符
    return os << "ID: "
        << std::right << std::hex << header.id << std::dec
        << " QR: " << header.qr
        << " OPCODE: " << (uint16_t) header.opCode
        << " AA: " << header.aa
        << " TC: " << header.tc
        << " RD: " << header.rd
        << " RA: " << header.ra
        << " ZERO: " << (uint16_t) header.z
        << " RCODE: " << (uint16_t) header.rCode
		<< std::endl
		<< " qdCount: " << header.qdCount
		<< " anCount: " << header.anCount
		<< " nsCount: " << header.nsCount
		<< " arCount: " << header.arCount
        << std::endl;
}
