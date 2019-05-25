#ifndef RECORD_H
#define RECORD_H

#include "network.h"
#include <chrono>
#include <map>

#include "message.h"

using ID = uint16_t;
using TimePoint = std::chrono::system_clock::time_point;
const std::chrono::seconds timeOut = std::chrono::seconds(5);

class RecordTable {
public:	
	RecordTable();
	~RecordTable();
	uint16_t getCurNumber();
	uint16_t insertRecord(Message &message);

	struct Record {
		uint16_t id;
		sockaddr_in sendAddr;
		TimePoint timePoint;
	};
	bool findRecord(ID id, Record &recordBuffer);
	void eraseTimeout();
	std::vector<std::pair<ID, Record>> table;

private:
	ID curNumber;
};

#endif