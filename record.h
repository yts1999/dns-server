#ifndef RECORD_H
#define RECORD_H

#include "network.h"
#include <chrono>
#include <map>

#include "message.h"

using ID = uint16_t;
using TimePoint = std::chrono::system_clock::time_point;
<<<<<<< HEAD
const std::chrono::seconds timeOut = std::chrono::seconds(5);
=======
>>>>>>> 13dbe758e95a9a5d65e018c60067539434277908

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
<<<<<<< HEAD
	void eraseTimeout();
=======
>>>>>>> 13dbe758e95a9a5d65e018c60067539434277908
	std::vector<std::pair<ID, Record>> table;

private:
	ID curNumber;
};

#endif