#include <string>

void toLower(std::string &str) {
	for (std::string::iterator iter = str.begin();
		 iter != str.end(); ++iter) {
			 *iter = tolower(*iter);
		 }
}