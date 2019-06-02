#ifndef TABLE_H
#define TABLE_H

#include <map>
#include <set>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>

#include "network.h"

// 本地对照表/LRU算法查询频次记录表
const std::string HOST_FILE = "dnsrelay.txt";
const std::string FREQ_FILE = "namefreq.txt";

//---------------IPv6 table---------------
const std::string HOST6_FILE = "dnsrelay6.txt";
const std::string FREQ6_FILE = "namefreq6.txt";

#define MAX_TABLE    100000		// 域名-IP 查询表最大容量


using QuadWordIP = uint32_t;
using HexWordIP = __uint128_t;

struct Property {
// 包含IP和查询频次
	QuadWordIP ip;
	int frequent;
	bool operator < (const Property &b) const;
};

//---------------IPv6 table---------------
struct Property6 {
	HexWordIP ip;
	int frequent;
	bool operator < (const Property6 &b) const;
};

class HostsTable {
// 本地记录表存储结构
public:
	HostsTable();
	~HostsTable();
	void readFile();
	void readFreqFile();
	void insertName(std::string name, QuadWordIP ip);
	int getTableSize() { return idMap.size(); }
	void eraseName();
	void writeFile();
	void writeFreqFile();
	bool findHost(const std::string &domainName, QuadWordIP &hostIP);

	//---------------IPv6 table---------------
	void readFile6();
	void readFreqFile6();
	void insertName6(std::string name, HexWordIP ip);
	int getTableSize6() { return idMap6.size(); }
	void eraseName6();
	void writeFile6();
	void writeFreqFile6();
	bool findHost6(const std::string &domainName, HexWordIP &hostIP);
private:
	std::map<std::string, Property> idMap;
	std::set<std::pair<Property, std::string> > propertySet;

	//---------------IPv6 table---------------
	std::map<std::string, Property6> idMap6;
	std::set<std::pair<Property6, std::string> > propertySet6;
};

#endif // TABLE_H
