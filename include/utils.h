#ifndef UTILS_H
#define UTILS_H

#include <string>

void toLower(std::string &str);
void parseOpt(int argc, char **argv);
extern bool STATIC;
extern int debugOption;

#endif // UTILS_H