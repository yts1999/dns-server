#include "include/record.h"

RecordTable::RecordTable() {
	curNumber = 0;
}

uint16_t RecordTable::insertRecord(Message &message) {
	Record recordBuf = {
		id : message.header.id,
		sendAddr : message.sendAddr,
		timePoint : std::chrono::system_clock::now()
	};
	table.push_back(std::make_pair(
		++curNumber,
		recordBuf
	));
	std::cout << "Insert record ID: " << curNumber << std::endl;
	return curNumber;
}

uint16_t RecordTable::getCurNumber() {
	return curNumber;
}

bool RecordTable::findRecord(ID id, RecordTable::Record &recordBuffer) {
	std::cout << std::endl;
	std::vector<std::pair<ID, Record>>::iterator iter;
	bool good = false;
	for (iter = table.begin(); iter != table.end(); ++iter) {
		if (iter->first == id) {
			good = true;
			recordBuffer = iter->second;
			table.erase(iter);
			break;
		}
	}
	return good;
}