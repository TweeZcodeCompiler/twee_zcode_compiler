#ifndef TWEEPARSER_TWEESCANNER_H
#define TWEEPARSER_TWEESCANNER_H

// Only include FlexLexer.h if it hasn't been already included
#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

// Override the interface for yylex since we namespaced it
#undef YY_DECL
#define YY_DECL int Twee::TweeScanner::yylex()

// Include Bison for types / tokens
#include "GeneratedTweeParser.h"
#include <fstream>


namespace Twee {
	class TweeScanner : public yyFlexLexer {
		public:
            TweeScanner(std::istream *input) : yyFlexLexer(input, 0){}

			// save the pointer to yylval so we can change it, and invoke scanner
			int yylex(Twee::BisonParser::semantic_type * lval){yylval = lval; return yylex();}

		private:
			// Scanning function created by Flex; make this private to force usage
			// of the overloaded method so we can get a pointer to Bison's yylval
			int yylex();

			// point to yylval (provided by Bison in overloaded yylex)
			Twee::BisonParser::semantic_type * yylval;
	};
}

#endif
