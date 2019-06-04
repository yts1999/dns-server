#ifndef TABLE_H
#define TABLE_H

#include <map>
#include <set>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <chrono>
#include <climits>

#include "network.h"

// 本地对照表/LRU算法查询频次记录表
const std::string HOST_FILE = "dnsrelay.txt";
const std::string FREQ_FILE = "namefreq.txt";

//---------------IPv6 table---------------
const std::string HOST6_FILE = "dnsrelay6.txt";
const std::string FREQ6_FILE = "namefreq6.txt";

#define MAX_TABLE    100000		// 域名-IP 查询表最大容量
#define MIN_TTL		 5			// 最小生存时间

using QuadWordIP = uint32_t;
using HexWordIP = __uint128_t;

struct Property {
// 包含IP和查询频次
	QuadWordIP ip;
	int frequent;
	time_t timeoutTime;
	bool operator < (const Property &b) const;
};

//---------------IPv6 table---------------
struct Property6 {
	HexWordIP ip;
	int frequent;
	time_t timeoutTime;
	bool operator < (const Property6 &b) const;
};

struct cmp {
	bool operator () (std::pair<std::string, Property> a, std::pair<std::string, Property> b) {
		return ((a.second.ip && b.second.ip) || (! a.second.ip && ! b.second.ip)) ? a.second.timeoutTime < b.second.timeoutTime : a.second.ip && ! b.second.ip;
	}
};

struct cmp6 {
	bool operator () (std::pair<std::string, Property6> a, std::pair<std::string, Property6> b) {
		return ((a.second.ip && b.second.ip) || (! a.second.ip && ! b.second.ip)) ? a.second.timeoutTime < b.second.timeoutTime : a.second.ip && ! b.second.ip;
	}
};

class HostsTable {
// 本地记录表存储结构
public:
	HostsTable();
	~HostsTable();
	void readFile();
	void readFreqFile();
	void insertName(std::string name, QuadWordIP ip, time_t timeoutTime);
	int getTableSize() { return idMap.size(); }
	void eraseName();
	void writeFile();
	void writeFreqFile();
	bool findHost(const std::string &domainName, QuadWordIP &hostIP, time_t &timeoutTime);

	//---------------IPv6 table---------------
	void readFile6();
	void readFreqFile6();
	void insertName6(std::string name, HexWordIP ip, time_t timeoutTime);
	int getTableSize6() { return idMap6.size(); }
	void eraseName6();
	void writeFile6();
	void writeFreqFile6();
	bool findHost6(const std::string &domainName, HexWordIP &hostIP, time_t &timeoutTime);
private:
	std::map<std::string, Property> idMap;
	std::set<std::pair<Property, std::string> > propertySet;
	std::set<std::pair<std::string, Property>, cmp> timeoutSet;

	//---------------IPv6 table---------------
	std::map<std::string, Property6> idMap6;
	std::set<std::pair<Property6, std::string> > propertySet6;
	std::set<std::pair<std::string, Property6>, cmp6> timeoutSet6;
};

#endif // TABLE_H
