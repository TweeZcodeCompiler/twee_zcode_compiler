#ifndef TWEEPARSER_PARSER_H
#define TWEEPARSER_PARSER_H

/**
 * Example usage:
 * Twee::TweeParser parser;
 * parser.parse();
*/

#include <iostream>
#include "Passage.h"
#include "ParseException.h"

namespace Twee {

    class TweeParser {
    public:
        TweeParser(std::istream* input);
        ~TweeParser();

        Passage* parse();

    private:
        class TweeParserImpl;
        TweeParserImpl* impl;
    };
}

#endif
