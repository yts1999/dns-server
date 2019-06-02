#include "include/table.h"

#include "include/utils.h"

bool Property::operator < (const Property &b) const {
// 属性的比较函数
	return ((ip && b.ip) || (! ip && ! b.ip)) ? frequent < b.frequent : ip && ! b.ip;
}

bool Property6::operator < (const Property6 &b) const {
// IPv6 地址属性的比较函数
	return ((ip && b.ip) || (! ip && ! b.ip)) ? frequent < b.frequent : ip && ! b.ip;
}

HostsTable::HostsTable() {
// 记录表初始化（读入 txt 文件）
	readFile();
	readFreqFile();
	readFile6();
	readFreqFile6();
}

HostsTable::~HostsTable() {
	if (!STATIC) {

	writeFile();
	writeFreqFile();
	writeFile6();
	writeFreqFile6();

	}
}

void HostsTable::readFile() {
// 读入地址-IP记录表
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
			propertySet.insert(std::pair<Property, std::string>(propBuf, domainName));
		}
		file.close();
	} else {
		std::cout << "host file error" << std::endl;
	}
}

void HostsTable::readFreqFile() {
// 读入 LRU 算法查询频次记录表
	std::ifstream file(FREQ_FILE);
	std::string domainName;
	QuadWordIP buf;
	int freqBuf;
	if (file.is_open()) {
		while (file >> domainName >> freqBuf) {
			toLower(domainName);
			std::map<std::string, Property>::iterator iter = idMap.find(domainName);
			if (iter != idMap.end()) {
				propertySet.erase(std::pair<Property, std::string>(iter->second, iter->first));
				iter->second.frequent = freqBuf;
				propertySet.insert(std::pair<Property, std::string>(iter->second, iter->first));
			}
		}
		file.close();
	} else {
		std::cout << "freq file error" << std::endl;
	}
}

void HostsTable::writeFile() {
// 写回域名-IP记录表
	std::ofstream file(HOST_FILE);
	QuadWordIP ip;
	if (file.is_open()) {
		std::map<std::string, Property>::iterator iter = idMap.begin();
		for (; iter != idMap.end(); ++iter) {
			ip = iter->second.ip;
			char str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, (void *)&ip, str, INET_ADDRSTRLEN);
			file << str << " " << iter->first << std::endl;
		}
		file.close();
	} else {
		std::cout << "host file error" << std::endl;
	}
}

void HostsTable::writeFreqFile() {
// 写回查询频次记录表
	std::ofstream file(FREQ_FILE);
	if (file.is_open()) {
		std::map<std::string, Property>::iterator iter = idMap.begin();
		for (; iter != idMap.end(); ++iter) {
			file << iter->first << " " << iter->second.frequent << std::endl;
		}
		file.close();
	} else {
		std::cout << "freq file error" << std::endl;
	}
}

void HostsTable::insertName(std::string domainName, QuadWordIP ip) {
// 在表中插入一条记录
	Property propBuf = {
		.ip = ip,
		.frequent = 1
	};
	idMap.insert(std::pair<std::string, Property>(domainName, propBuf));
	propertySet.insert(std::pair<Property, std::string>(propBuf, domainName));
}

bool HostsTable::findHost(const std::string &domainName, QuadWordIP &hostIP) {
// 在表中根据域名查询 IP 地址 
	std::map<std::string, Property>::iterator iter = idMap.find(domainName);
	if (iter != idMap.end()) {
		propertySet.erase(std::pair<Property, std::string>(iter->second, iter->first));
		iter->second.frequent++;
		propertySet.insert(std::pair<Property, std::string>(iter->second, iter->first));
		hostIP = iter->second.ip;
		return true;
	} else {
		return false;
	}
}

void HostsTable::eraseName() {
// 从表中删除一条记录
	if (! propertySet.empty()) {
		std::set<std::pair<Property, std::string> >::iterator iter = propertySet.begin();
		if (iter->first.ip != 0) {
			idMap.erase(iter->second);
			propertySet.erase(iter);
		}
	}
}

//---------------IPv6 table---------------
void HostsTable::readFile6() {
	std::ifstream file(HOST6_FILE);
	std::string domainName;
	std::string ip;
	if (file.is_open()) {

		while (file >> ip >> domainName) {
			toLower(domainName);
			in6_addr hostAddr;
			inet_pton(AF_INET6, ip.c_str(), (void *)&hostAddr);
			Property6 propBuf = {
				.ip = *(__uint128_t *)&hostAddr.__u6_addr,//__in6_u,
				.frequent = 0
			};
			idMap6.insert(std::pair<std::string, Property6>(domainName, propBuf));
			propertySet6.insert(std::pair<Property6, std::string>(propBuf, domainName));
		}
		file.close();
	} else {
		std::cout << "host file error" << std::endl;
	}
}

void HostsTable::readFreqFile6() {
	std::ifstream file(FREQ6_FILE);
	std::string domainName;
	HexWordIP buf;
	int freqBuf;
	if (file.is_open()) {
		while (file >> domainName >> freqBuf) {
			toLower(domainName);
			std::map<std::string, Property6>::iterator iter = idMap6.find(domainName);
			if (iter != idMap6.end()) {
				propertySet6.erase(std::pair<Property6, std::string>(iter->second, iter->first));
				iter->second.frequent = freqBuf;
				propertySet6.insert(std::pair<Property6, std::string>(iter->second, iter->first));
			}
		}
		file.close();
	} else {
		std::cout << "freq file error" << std::endl;
	}
}

void HostsTable::writeFile6() {
	std::ofstream file(HOST6_FILE);
	HexWordIP ip;
	if (file.is_open()) {
		std::map<std::string, Property6>::iterator iter = idMap6.begin();
		for (; iter != idMap6.end(); ++iter) {
			ip = iter->second.ip;
			char str[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, (void *)&ip, str, INET6_ADDRSTRLEN);
			file << str << " " << iter->first << std::endl;
		}
		file.close();
	} else {
		std::cout << "host file error" << std::endl;
	}
}

void HostsTable::writeFreqFile6() {
	std::ofstream file(FREQ6_FILE);
	if (file.is_open()) {
		std::map<std::string, Property6>::iterator iter = idMap6.begin();
		for (; iter != idMap6.end(); ++iter) {
			file << iter->first << " " << iter->second.frequent << std::endl;
		}
		file.close();
	} else {
		std::cout << "freq file error" << std::endl;
	}
}

void HostsTable::insertName6(std::string domainName, HexWordIP ip) {
	Property6 propBuf = {
		.ip = ip,
		.frequent = 1
	};
	idMap6.insert(std::pair<std::string, Property6>(domainName, propBuf));
	propertySet6.insert(std::pair<Property6, std::string>(propBuf, domainName));
}

bool HostsTable::findHost6(const std::string &domainName, HexWordIP &hostIP) {
	std::map<std::string, Property6>::iterator iter = idMap6.find(domainName);
	if (iter != idMap6.end()) {
		propertySet6.erase(std::pair<Property6, std::string>(iter->second, iter->first));
		iter->second.frequent++;
		propertySet6.insert(std::pair<Property6, std::string>(iter->second, iter->first));
		hostIP = iter->second.ip;
		return true;
	} else {
		return false;
	}
}

void HostsTable::eraseName6() {
	if (! propertySet6.empty()) {
		std::set<std::pair<Property6, std::string> >::iterator iter = propertySet6.begin();
		if (iter->first.ip != 0) {
			idMap6.erase(iter->second);
			propertySet6.erase(iter);
		}
	}
}
