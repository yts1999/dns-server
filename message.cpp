#include "message.h"

Message::Message() {

}

Message::Message(const sockaddr_in addr, const unsigned char *buf, int bufLen) :
	sendAddr(addr) {
	unsigned char *ptrBuffer = (unsigned char *)buf;
	getHeader(ptrBuffer);
	if (header.qr == 0) {
		getQuestion(ptrBuffer);
		getResource(ptrBuffer, answer, header.anCount);
		getResource(ptrBuffer, authority, header.nsCount);
		getResource(ptrBuffer, additional, header.arCount);
		std::cout << "resource get!" << std::endl;
	}
}

std::string Message::getDomainName(const std::string &name)  {
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

void Message::getHeader(unsigned char *&bufPtr) {
	unsigned short *headerPtr = (unsigned short *)bufPtr;
	header.id          = ntohs(*(headerPtr    ));
	unsigned short headerBuf = ntohs(*(headerPtr + 1));

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
	std::cout << header;
}

void Message::getQuestion(unsigned char *&bufPtr) {
	for (int count = 0; count < header.qdCount; count++) {
		// not parse compressed domain name
		Question questionBuf;
		std::string nameBuf = "";
		char *namePtr = (char *)bufPtr;
		while (*namePtr != 0) {
			unsigned char sectionSize = *namePtr;
		//	std::cout << "Name size: " << (int)sectionSize << std::endl;
			nameBuf.append(namePtr, sectionSize + 1);
			namePtr += (sectionSize + 1);
		}
		nameBuf.append(1, '\0');
		bufPtr = (unsigned char *)namePtr + 1;
		questionBuf.QName = nameBuf;
		getdWord(questionBuf.QType, bufPtr);
		getdWord(questionBuf.QClass, bufPtr);
		question.push_back(questionBuf);
	}
	std::cout << "Question set done!" << std::endl;
}

void Message::getResource(unsigned char *&bufPtr, std::vector<Message::Resource> &resourceVec, unsigned short count) {
	for (int _count = 0; _count < count; _count++) {
		Resource resourceBuf;
		std::string nameBuf = "";
		char *namePtr = (char *)bufPtr;
		while (namePtr != 0) {
			unsigned char sectionSize = *namePtr;
			nameBuf.append(namePtr, sectionSize + 1);
			namePtr += sectionSize + 1;
		}
		nameBuf.append(1, '\0');
		bufPtr = (unsigned char *)namePtr + 1;

		getdWord(resourceBuf.type, bufPtr);
		getdWord(resourceBuf.clas, bufPtr);
		getqWord(resourceBuf.ttl,  bufPtr);
		getdWord(resourceBuf.rdLength, bufPtr);

		resourceBuf.rData.assign(bufPtr, bufPtr + resourceBuf.rdLength);
		bufPtr += resourceBuf.rdLength;
		resourceVec.push_back(resourceBuf);
	}
}

void Message::getdWord(unsigned short &target, unsigned char *&bufPtr) {
	unsigned short *dWordPtr = (unsigned short *)bufPtr;
	target = ntohs(*dWordPtr);
	bufPtr += sizeof(unsigned short);
}

void Message::getqWord(unsigned int &target, unsigned char *&bufPtr) {
	unsigned int *qWordPtr = (unsigned int *)bufPtr;
	target = ntohs(*qWordPtr);
	bufPtr += sizeof(unsigned int);
}

std::ostream &operator << (std::ostream &os,
                               const Message::Header &header)
    {
        return os << "ID: "
            << std::right << std::hex << header.id << std::dec
            << " QR: " << header.qr
            << " OPCODE: " << (unsigned short) header.opCode
            << " AA: " << header.aa
            << " TC: " << header.tc
            << " RD: " << header.rd
            << " RA: " << header.ra
            << " ZERO: " << (unsigned short) header.z
            << " RCODE: " << (unsigned short) header.rCode
			<< std::endl
			<< " qdCount: " << header.qdCount
			<< " anCount: " << header.anCount
			<< " nsCount: " << header.nsCount
			<< " arCount: " << header.arCount
            << std::endl;
    }
