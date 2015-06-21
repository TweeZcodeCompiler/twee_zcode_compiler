
#include "include/TweeFile.h"
#include "include/TweeParser.h"

#include "TweeScanner.h"
#include "include/ParseException.h"
#include <stdio.h>

using namespace Twee;
using namespace std;

class TweeParser::TweeParserImpl {
public:
    TweeParserImpl(std::istream* input) : scanner(input), parser(scanner) { }
    unique_ptr<TweeFile> parse();

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

unique_ptr<TweeFile> TweeParser::parse() {
    return impl->parse();
}

/* impl */

unique_ptr<TweeFile> TweeParser::TweeParserImpl::parse() {
    parser.parse();

    if(!tweeStructure) {
        throw new ParseException();
    }

    unique_ptr<TweeFile> tweeFilePtr(tweeStructure);
    tweeStructure = nullptr;

    return tweeFilePtr;
}