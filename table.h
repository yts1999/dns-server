#ifndef TABLE_H
#define TABLE_H

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>

#include "network.h"

const std::string HOST_FILE = "dnsrelay.txt"; 
const std::string FREQ_FILE = "namefreq.txt";

#define MAX_TABLE    200


using DoubleWordIP = uint32_t;

struct Property {
	DoubleWordIP ip;
	int frequent;
};

using IdMap = std::map<std::string, Property>;

class HostsTable {
public:
	HostsTable();
	~HostsTable();
	void readFile();
	void readFreqFile();
	void insertName(std::string name, DoubleWordIP ip);
	int getTableSize() { return idMap.size(); }
	void eraseName();
	void writeFile();
	void writeFreqFile();
	bool findHost(const std::string &domainName, DoubleWordIP &hostIP);
private:
	IdMap idMap;
	
};

#endif // TABLE_H