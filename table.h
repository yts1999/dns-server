#ifndef TABLE_H
#define TABLE_H

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>

#include "network.h"

const std::string HOST_FILE = "dnsrelay.txt"; 

using DoubleWordIP = uint32_t;
using IdMap = std::map<std::string, DoubleWordIP>;

class HostsTable {
public:
	HostsTable();
	void readFile();

	bool findHost(const std::string &domainName, DoubleWordIP &hostIP);
private:
	IdMap idMap;
};

#endif // TABLE_H