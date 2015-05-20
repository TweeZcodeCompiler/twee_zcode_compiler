#include "include/TweeParser.h"

#include "TweeScanner.h"

using namespace Twee;

class TweeParser::TweeParserImpl {
public:
    TweeParserImpl() : parser(scanner) { }
    int parse();

private:
    TweeScanner scanner;
    BisonParser parser;
};

TweeParser::TweeParser() {
    this->impl = new TweeParserImpl();
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
