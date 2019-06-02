#include "include/record.h"

RecordTable::RecordTable() {
// 查询记录表初始化
	curNumber = 0;
}

uint16_t RecordTable::insertRecord(Message &message) {
// 在查询记录表中插入一条记录
	Record recordBuf = {
		.id = message.header.id,
		.sendAddr = message.sendAddr,
		.domainName = message.question.begin()->getName(),
		.timePoint = std::chrono::system_clock::now()
	};
	table.push_back(std::make_pair(
		++curNumber,
		recordBuf
	));
	return curNumber;
}

uint16_t RecordTable::getCurNumber() {
	return curNumber;
}

bool RecordTable::findRecord(ID id, RecordTable::Record &recordBuffer) {
// 在记录表中根据 ID 字段查询记录
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

void RecordTable::eraseTimeout() {
// 清除记录表中超时的记录
	std::vector<std::pair<ID, Record>>::iterator iter;
	for (iter = table.begin(); iter != table.end();) {
		TimePoint recordTime = iter->second.timePoint;
		if (std::chrono::system_clock::now() - recordTime > timeOut) {
			iter = table.erase(iter);
		} else {
			iter++;
		}
	}
}