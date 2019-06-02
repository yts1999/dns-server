#include <string>
#include <iostream>

bool STATIC = true;
int debugOption = 0;

void toLower(std::string &str) {
// 将域名转换为小写字母
	for (std::string::iterator iter = str.begin();
		 iter != str.end(); ++iter) {
			 *iter = tolower(*iter);
		 }
}

void helper() {
// 显示使用说明
	std::cout << "Usage: ./dnsrelay [--update]" << std::endl
	          << "                  [--debug ]" << std::endl
			  << "--update : update local cache dynamicly using LRU Algorithm" << std::endl
			  << "--debug  : output the header of the Message" << std::endl;
}

void parseOpt(int argc, char **argv) {
// 处理命令行参数
	if (argc > 3) {
		helper();
		exit(0);
	} else {
		for (int i = 1; i < argc; i++) {
			std::string argStr = std::string(argv[i]);
			if (argStr.compare(std::string("--update")) == 0) {
				STATIC = false;
			} else if (argStr.compare(std::string("--debug")) == 0) {
				debugOption = 1;
			} else {
				helper();
				exit(0);
			}
		}
	}
}

