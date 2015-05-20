#ifndef TWEEPARSER_PARSER_H
#define TWEEPARSER_PARSER_H

/**
 * Example usage:
 * Twee::TweeParser parser;
 * parser.parse();
*/

namespace Twee {

    class TweeParser {
    public:
        TweeParser();
        ~TweeParser();

        int parse();

    private:
        class TweeParserImpl;
        TweeParserImpl* impl;
    };
}

#endif
