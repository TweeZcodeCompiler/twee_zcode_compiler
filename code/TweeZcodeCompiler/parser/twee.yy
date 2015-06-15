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
}

%code {
    TweeFile *tweeStructure; /* the result data model */

    //TODO: use this function to pass parse errors to the exceptions, also look for Twee::BisonParser::error
	void yyerror(const char *s) { printf("ERROR: %s\n", s); }

	// Prototype for the yylex function
	static int yylex(Twee::BisonParser::semantic_type * yylval, Twee::TweeScanner &scanner);

    //TODO: implement plog for logging
	void logDebug(std::string message) {
        std::cout << "Parser: " << message << "\n";
    }

    //TODO: make a formattedText function
    /*to get the bold, italic etc. values of the FORMATTING_OPEN
     * and FORMATTING_CLOSE tokens
     */

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
    logDebug("TweeDocument -> passages");
    tweeStructure = $1;
    }
  ;
passages :
    passages passage
    {
    logDebug("passages -> passages passage");
    logDebug("pass passages:type(TweeFile) to top:passages:type(TweeFile)");
    $$ = $1;
    logDebug("add the passage:type(Passage) to the passages:type(TweeFile)");
    *$$ += *$2;
    }
	|passage
	{
	logDebug("passages -> passage");
    logDebug("create a passages:type(TweeFile)");
    $$ = new TweeFile();
    logDebug("add passage:type(Passage) to passages:type(TweeFile)");
    *$$ += *$1;
    }
  ;
passage :
    head body
    {
    logDebug("passage -> head body");
    logDebug("create a passage:type(Passage) out of the head:type(Head) and body::type(Body) objects");
    $$ = new Passage(*$1, *$2);
    }
  ;

head :
    PASSAGE_START title
    {
    logDebug("head -> PASSAGE_START title");
    logDebug("pass title:type(Head) to head:type(Head)");
    $$ = $2;
    }
  ;

title :
    TITLE NEWLINE
    {
    logDebug("title -> TITLE NEWLINE");
    logDebug("create title:type(Head) out of token:TITLE");
    $$ = new Head(*$1);
    }
    |TITLE TAGS_OPEN tags TAGS_CLOSE NEWLINE
    {
    logDebug("title -> TITLE TAGS_OPEN tags TAGS_CLOSE NEWLINE");
    logDebug("create title:type(Head) out of token:TITLE");
    //TODO: incorporate tags
    $$ = new Head(*$1);
    }
  ;

tags :
    tags TAG
    {
    logDebug("tags -> tags TAG");
    //TODO: TAG token vector: tags TAG production
    }
    |TAG
    {
    logDebug("tags -> TAG");
    //TODO: TAG token vector: tags TAG production
    }
  ;

body :
    body bodypart
    {
    logDebug("body -> body bodypart");
    logDebug("pass body:type(Body) to top:body:type(Body)");
    $$ = $1;
    logDebug("add bodypart:type(BodyPart) to top:body:type(Body)");
    *$$ += $2;
    }
    |bodypart
    {
    logDebug("body -> bodypart");
    logDebug("create top:body:type(Body)");
    $$ = new Body();
    logDebug("add bodypart:type(BodyPart) to top:body:type(Body)");
    *$$ += $1;
    }
  ;

bodypart :
    text
    {
    logDebug("bodypart -> text");
    logDebug("pass text:type(text) to bodypart:type(BodyPart)");
    $$ = $1;
    }
    |link
    {
    logDebug("bodypart -> link");
    logDebug("pass link:type(Link) to bodypart:type(BodyPart)");
    $$ = $1;
    }
    |macro
    {
    logDebug("bodypart -> macro");
    //TODO: implement Macro:BodyType
    logDebug("pass macro:type(--Text--) to bodypart:type(BodyPart)");
    $$ = $1;
    }
    |formatted
    {
    logDebug("bodypart -> formatted");
    //TODO: implement FormattedText parsing
    logDebug("pass formatted:type(--Text--) to bodypart:type(BodyPart)");
    $$ = $1;
    }
  ;

text :
    TEXT_TOKEN
    {
    logDebug("text -> TEXT_TOKEN");
    logDebug("create top:text:type(Text)");
    $$ = new Text(*$1);
    }
  ;

link :
    LINK_OPEN TEXT_TOKEN LINK_CLOSE
    {
    logDebug("link -> LINK_OPEN TEXT_TOKEN LINK_CLOSE");
    logDebug("create top:link:type(Link) with 2:token:TEXT_TOKEN & 2:token:TEXT_TOKEN");
    $$ = new Link(*$2);
    }
    |LINK_OPEN TEXT_TOKEN LINK_SEPARATOR TEXT_TOKEN LINK_CLOSE
    {
    logDebug("link -> LINK_OPEN TEXT_TOKEN LINK_SEPARATOR TEXT_TOKEN LINK_CLOSE");
    logDebug("create top:link:type(Link) with 4:token:TEXT_TOKEN & 2:token:TEXT_TOKEN");
    $$ = new Link(*$4, *$2);
    }
  ;

macro :
    MACRO_OPEN TEXT_TOKEN MACRO_CLOSE
    {
    //TODO: data model: implement macro
    logDebug("macro -> MACRO_OPEN TEXT_TOKEN MACRO_CLOSE");
    logDebug("create top:macro:type(--Text--) with 2:token:TEXT_TOKEN");
    $$ = new Text(*$2);
    }
  ;

formatted:
    FORMATTING_OPEN TEXT_TOKEN FORMATTING_CLOSE
    {
    //TODO:check if F_OPEN and F_CLOSE are the same
    logDebug("formatted -> FORMATTING_OPEN TEXT_TOKEN FORMATTING_CLOSE");
    logDebug("create top:formatted:type(--Text--) with 2:token:TEXT_TOKEN");
    $$ = new FormattedText(*$2, true, false, false);
    }
    |FORMATTING_OPEN formatted FORMATTING_CLOSE
    {
    //TODO:check if F_OPEN and F_CLOSE are the same
    logDebug("FORMATTING_OPEN formatted FORMATTING_CLOSE");
    logDebug("pass formatted:type(--Text--) up to top:formatted:type(--Text--)");
    $$ = $2;
    }
  ;

%%

// We have to implement the error function
//TODO: use this function to pass parse errors to the exceptions, also look for yyerror
void Twee::BisonParser::error(const std::string& msg) {
	std::cerr << "Error: " << msg << std::endl;
}


// Now that we have the Parser declared, we can declare the Scanner and implement
// the yylex function

#include "TweeScanner.h"
static int yylex(Twee::BisonParser::semantic_type * yylval, Twee::TweeScanner &scanner) {
	return scanner.yylex(yylval);
}