#include "table.h"

#include "utils.h"

HostsTable::HostsTable() {
	readFile();
	readFreqFile();
}

HostsTable::~HostsTable() {
	if (!STATIC) {
	
	writeFile();
	writeFreqFile();
	
	}
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
			Property propBuf = {
				.ip = hostAddr.s_addr,
				.frequent = 0
			};
			
			idMap.insert(std::pair<std::string, Property>(domainName, propBuf));
		}
	} else {
		std::cout << "host file error" << std::endl;
	}
}

void HostsTable::readFreqFile() {
	std::ifstream file(HOST_FILE);
	std::string domainName;
	DoubleWordIP buf;
	int freqBuf;
	if (file.is_open()) {
		while (file >> domainName >> freqBuf) {
			toLower(domainName);
			if (findHost(domainName, buf) == true) {
				idMap[domainName].frequent = freqBuf;
			}
		}
	}
}

void HostsTable::writeFile() {
	std::ofstream file(HOST_FILE);
	DoubleWordIP ip;
	if (file.is_open()) {
		std::map<std::string, Property>::iterator iter = idMap.begin();
		for (; iter != idMap.end(); ++iter) {
			ip = iter->second.ip;
			char str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, (void *)&ip, str, sizeof(sockaddr_in));
			file << str << " " << iter->first << std::endl;
		}
	} else {
		std::cout << "host file error" << std::endl;
	}
}

void HostsTable::writeFreqFile() {
	std::ofstream file(FREQ_FILE);
	if (file.is_open()) {
		std::map<std::string, Property>::iterator iter = idMap.begin();
		for (; iter != idMap.end(); ++iter) {
			file << iter->first << " " << iter->second.frequent << std::endl;
		}
	} else {
		std::cout << "freq file error" << std::endl;
	}
}

void HostsTable::insertName(std::string domainName, DoubleWordIP ip) {
	Property propBuf = {
		.ip = ip,
		.frequent = 1
	};
	idMap.insert(std::pair<std::string, Property>(domainName, propBuf));
}

bool HostsTable::findHost(const std::string &domainName, DoubleWordIP &hostIP) {
	std::map<std::string, Property>::iterator iter;
	iter = idMap.find(domainName);
	if (iter != idMap.end()) {
		iter->second.frequent++;
		hostIP = iter->second.ip;
		return true;
	} else {
		return false;
	}
}

void HostsTable::eraseName() {
	std::map<std::string, Property>::iterator minPtr = idMap.begin(),
	                                          curPtr = idMap.begin();
	int minFrequent = idMap.begin()->second.frequent;
	for (; curPtr != idMap.end(); ++curPtr) {
		if (curPtr->second.frequent < minFrequent && curPtr->second.ip != 0) {
			minPtr = curPtr;
			minFrequent = curPtr->second.frequent;
		}
	}
	idMap.erase(minPtr);
}
