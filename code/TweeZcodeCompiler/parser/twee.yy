%debug

%skeleton "lalr1.cc"


%defines
%define api.namespace {Twee}
%define parser_class_name {BisonParser}
%parse-param { Twee::TweeScanner &scanner }
%lex-param   { Twee::TweeScanner &scanner }


%code requires {
	#include "Passage.h"
	#include "Body.h"
	#include <memory>

	// Forward-declare the Scanner class; the Parser needs to be assigned a 
	// Scanner, but the Scanner can't be declared without the Parser
	namespace Twee {
		class TweeScanner;
	}
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
	DOUBLE_COLON PTITLE LINEBREAK body		{$$=new Passage(*$2,*$4);}
  ;

body:
	PBODYWORD						{$$=new Body(*$1);}
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

