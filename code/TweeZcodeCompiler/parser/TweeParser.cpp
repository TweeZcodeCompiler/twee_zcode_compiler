#include "include/TweeParser.h"

#include "TweeScanner.h"

using namespace Twee;

class TweeParser::TweeParserImpl {
public:
    TweeParserImpl(std::istream* input) : scanner(input), parser(scanner) { }
    int parse();

private:
    TweeScanner scanner;
    BisonParser parser;
};

TweeParser::TweeParser(std::istream* input) {
    this->impl = new TweeParserImpl(input);
}

TweeParser::~TweeParser() {
    delete this->impl;
}

int TweeParser::parse() {
    return impl->parse();
}


/* impl */

int TweeParser::TweeParserImpl::parse() {
    return parser.parse();
}
