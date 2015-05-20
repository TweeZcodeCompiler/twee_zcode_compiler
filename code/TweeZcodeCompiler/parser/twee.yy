%debug

%skeleton "lalr1.cc"


%defines
%define api.namespace {Twee}
%define parser_class_name {BisonParser}
%parse-param { Twee::TweeScanner &scanner }
%lex-param   { Twee::TweeScanner &scanner }
%define parse.error verbose


%code requires {
	#include "Passage.h"
	#include "Body.h"
	#include <memory>
	#include <stdio.h>
	
	// Forward-declare the Scanner class; the Parser needs to be assigned a 
	// Scanner, but the Scanner can't be declared without the Parser
	namespace Twee {
		class TweeScanner;
	}
	#define DEBUG_PARSER std::cout << "\t\tPARSER DEBUG: " <<
}

%code {
	void yyerror(const char *s) { printf("ERROR: %s\n", s); }
	// Prototype for the yylex function
	static int yylex(Twee::BisonParser::semantic_type * yylval, Twee::TweeScanner &scanner);
}


%union{
	std::string *string;
	int token;
	Passage *passage;
	Body *body;
}

%token 
	<token> LINEBREAK
	<token> DOUBLE_COLON
	<string> PTITLE
	<string> PBODYWORD


%type <body> body
%type <passage> S

%start S

%%

S : 
	DOUBLE_COLON PTITLE LINEBREAK body			{
												$$=new Passage(*$2,*$4);
												DEBUG_PARSER "Made a new Passage from" << '\n';
												DEBUG_PARSER "\t PTITLE: " << *$2 << '\n';
												DEBUG_PARSER "\t Body: " << $4->getContent() << '\n';
												}
  ;

body:
	PBODYWORD									{
												$$=new Body(*$1);
												DEBUG_PARSER "Made a new Body from" << '\n';
												DEBUG_PARSER "\t PBODYWORD: " << *$1 << '\n';
												}
	|body LINEBREAK PBODYWORD					{
												*$1 += *$3;
												DEBUG_PARSER "\t Added" << '\n';
												DEBUG_PARSER "\t PBODYWORD: " << *$3 << '\n';
												DEBUG_PARSER "\t to the Body Object " << '\n';
												$$=$1;
												DEBUG_PARSER "Passed a Body object up the syntax tree" << '\n';

												}
    ;
	
%%


// We have to implement the error function
void Twee::BisonParser::error(const std::string& msg) {
	std::cerr << "Error: " << msg << std::endl;
}


// Now that we have the Parser declared, we can declare the Scanner and implement
// the yylex function

#include "TweeScanner.h"
static int yylex(Twee::BisonParser::semantic_type * yylval, Twee::TweeScanner &scanner) {
	return scanner.yylex(yylval);
}

