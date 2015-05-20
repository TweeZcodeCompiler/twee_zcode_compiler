#include "include/TweeParser.h"

#include "TweeScanner.h"

using namespace Twee;

class TweeParser::TweeParserImpl {
public:
    TweeParserImpl(std::istream* input) : scanner(input), parser(scanner) { }
    Passage parse();

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

Passage TweeParser::parse() {
    return impl->parse();
}


/* impl */

Passage TweeParser::TweeParserImpl::parse() {
    parser.parse();
    return *tweeStructure;
}
