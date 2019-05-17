#include "include/parser.h"

int main() {
	Parser *parser = new Parser();
	while (true) {
		parser->receive();
		parser->parse();
	}
}