#ifndef TWEEPARSER_PARSER_H
#define TWEEPARSER_PARSER_H

/**
 * Example usage:
 * Twee::TweeParser parser;
 * parser.parse();
*/

#include <iostream>
#include "TweeFile.h"
#include "ParseException.h"

namespace Twee {

    class TweeParser {
    public:
        TweeParser(std::istream* input);
        ~TweeParser();

        TweeFile* parse();

    private:
        class TweeParserImpl;
        TweeParserImpl* impl;
    };
}

#endif
