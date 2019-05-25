#include "table.h"

#include "utils.h"

HostsTable::HostsTable() {
	readFile();
}

void HostsTable::readFile() {
	std::ifstream file(HOST_FILE);
	std::string domainName;
	std::string ip;
	if (file.is_open()) {
		while (file >> ip >> domainName) {
			toLower(domainName);
			in_addr hostAddr;
			inet_pton(AF_INET, ip.c_str(), (void *)&hostAddr);
			idMap.insert(std::pair<std::string, DoubleWordIP>(domainName, hostAddr.s_addr));
		}
	} else {
		std::cout << "host file error" << std::endl;
	}
}

bool HostsTable::findHost(const std::string &domainName, DoubleWordIP &hostIP) {
	std::map<std::string, DoubleWordIP>::iterator iter;
	iter = idMap.find(domainName);
	if (iter != idMap.end()) {
		hostIP = iter->second;
		return true;
	} else {
		return false;
	}
}