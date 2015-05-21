#include "include/TweeParser.h"

#include "TweeScanner.h"
#include "include/ParseException.h"

using namespace Twee;

class TweeParser::TweeParserImpl {
public:
    TweeParserImpl(std::istream* input) : scanner(input), parser(scanner) { }
    Passage* parse();

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

Passage* TweeParser::parse() {
    return impl->parse();
}


/* impl */

Passage* TweeParser::TweeParserImpl::parse() {
    parser.parse();
    if(!tweeStructure) {
        throw new ParseException();
    }

    // TODO: return a copy of the passage. global will be changed on next parse run.
    return tweeStructure;
}
