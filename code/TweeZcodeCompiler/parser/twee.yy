%defines "GeneratedTweeParser.h"
%require "3.0.2"
%define api.namespace {Twee}
%define parser_class_name {BisonParser}
%parse-param { Twee::TweeScanner &scanner }
%lex-param   { Twee::TweeScanner &scanner }
%define parse.error verbose
%debug

%skeleton "lalr1.cc"

%code requires {
	#include <memory>
	#include <stdio.h>
	#include <vector>

    #include "include/TweeFile.h"
    #include "include/Passage/Passage.h"
    #include "include/Passage/Body/Text.h"
    #include "include/Passage/Body/Link.h"
    #include "include/Passage/Body/FormattedText.h"
	extern TweeFile *tweeStructure; /* the result data model */

	// forward declare the Scanner class
	namespace Twee {
		class TweeScanner;
	}
	#define DEBUG_PARSER std::cout << "\t\tPARSER DEBUG: " <<
}

%code {
	void yyerror(const char *s) { printf("ERROR: %s\n", s); }
	// Prototype for the yylex function
	static int yylex(Twee::BisonParser::semantic_type * yylval, Twee::TweeScanner &scanner);

	void parser_log(std::string message) {
        std::cout << "\tParser: " << message << "\n";
    }

	TweeFile *tweeStructure; /* the result data model */

	//TODO: declare the passage list globally or look if it can be integrated into the parse tree
}


%union{
	std::string *string;
	int token;

	TweeFile *tweefile;

	Passage *passage;

	Head *head;
	Text *tag;

	Body *body;

	BodyPart *bodypart;
	Text *text;
	Link *link;
	FormattedText *formattedtext;


	//TODO: add Syntax Tree classes
}

%token
	<token> PASSAGE_START
	<token> NEWLINE

	<token> TAGS_OPEN
	<token> TAGS_CLOSE
	<string> TITLE
	<string> TAG

	<token> LINK_OPEN
	<token> LINK_CLOSE
    <token> LINK_SEPARATOR
	<token> MACRO_OPEN
	<token> MACRO_CLOSE
	<token> FORMATTING_OPEN
	<token> FORMATTING_CLOSE
    <string> TEXT_TOKEN

 //TODO: apply new Syntax Tree implementation
%type <tweefile> TweeDocument

%type <passage> passage
%type <tweefile> passages

%type <head> head
%type <head> title
%type <tag> tags

%type <body> body

%type <bodypart> bodypart
%type <text> text
%type <link> link
%type <formattedtext> formatted
%type <text> macro

%start TweeDocument

%%

TweeDocument :
    passages
    {
    //TODO: make a TweeDocument out of the passages
    parser_log("make a TweeDocument out of the passages");
    tweeStructure = $1;
    }
  ;
passages :
    passages passage
    {
    parser_log("pass the TweeFile upwards");
    $$ = $1;
    parser_log("add the passage to the twee file");
    *$$ += *$2;
    }
	|passage
	{
    parser_log("create a TweeFile");
    $$ = new TweeFile();
    parser_log("add passage to TweeFile");
    *$$ += *$1;
    }
  ;
passage :
    head body
    {
    parser_log("make a passage out of the Head and Body objects");
    $$ = new Passage(*$1, *$2);
    }
  ;

head :
    PASSAGE_START title
    {
    parser_log("pass Head object of title upwards");
    $$ = $2;
    }
  ;

title :
    TITLE NEWLINE
    {
    parser_log("generate Head object");
    $$ = new Head(*$1);
    }
    |TITLE TAGS_OPEN tags TAGS_CLOSE NEWLINE
    {
    parser_log("generate Head object, add the list of token strings to it");
    //TODO: incorporate tags
    $$ = new Head(*$1);
    }
  ;

tags :
    tags TAG
    {
    //TODO: TAG token vector: tags TAG production
    parser_log("add the TAG-Token's string to the tags list");
    }
    |TAG
    {
    //TODO: TAG token vector: tags TAG production
    parser_log("generate tags list");
    parser_log("add TAG to tags list");
    }
  ;

body :
    body bodypart
    {
    //TODO: add the bodypart to the bodypart list
    parser_log("pass body upwards");
    $$ = $1;
    parser_log("pass it upwards");
    *$$ += $2;
    }
    |bodypart
    {
    parser_log("generate body");
    $$ = new Body();
    parser_log("add bodypart to body");
    *$$ += $1;
    }
  ;

bodypart :
    text
    {
    parser_log("pass the Text object up");
    $$ = $1;
    }
    |link
    {
    parser_log("pass the Link object up");
    $$ = $1;
    }
    |macro
    {
    parser_log("pass the Macro object up");
    $$ = $1;
    }
    |formatted
    {
    parser_log("pass the FormattedText object up");
    $$ = $1;
    }
  ;

text :
    TEXT_TOKEN
    {
    parser_log("generate Text object");
    $$ = new Text(*$1);
    }
  ;

link :
    LINK_OPEN TEXT_TOKEN LINK_CLOSE
    {
    parser_log("generate Link object, name == destination");
    $$ = new Link(*$2);
    }
    |LINK_OPEN TEXT_TOKEN LINK_SEPARATOR TEXT_TOKEN LINK_CLOSE
    {
    parser_log("generate Link object, separate name & destination");
    $$ = new Link(*$4, *$2);
    }
  ;

macro :
    MACRO_OPEN TEXT_TOKEN MACRO_CLOSE
    {
    //TODO: data model: implement macro
    parser_log("generate Macro object");
    $$ = new Text(*$2);
    }
  ;

formatted:
    FORMATTING_OPEN TEXT_TOKEN FORMATTING_CLOSE
    {
    //TODO:check if F_OPEN and F_CLOSE are the same
    //TODO:implement bold,underlined, italic-differenciate function
    parser_log("make new Formatted Text");
    $$ = new FormattedText(*$2);
    $$->setIsBold(true);
    }
    |FORMATTING_OPEN formatted FORMATTING_CLOSE
    {
    //TODO:check if F_OPEN and F_CLOSE are the same
    //TODO:implement bold,underlined, italic-differenciate function
    $$ = $2;
    $$->setIsBold(!$$->isBold());
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