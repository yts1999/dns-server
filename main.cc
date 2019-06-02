#include "include/parser.h"

int main(int argc, char **argv) {
	parseOpt(argc, argv);
	
	Parser *parser = new Parser();
// 循环驱动事件
	while (true) {
		parser->receive();
		parser->parse();
		parser->writeBack();
	}
}