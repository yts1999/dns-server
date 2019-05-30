#include "parser.h"

int main(int argc, char **argv) {
	parseOpt(argc, argv);
	
	Parser *parser = new Parser();

	while (true) {
		parser->receive();
		parser->parse();
		parser->writeBack();
	}
}
