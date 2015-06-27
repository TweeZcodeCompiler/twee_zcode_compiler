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
    #include <plog/Log.h>
    #include <plog/Appenders/ConsoleAppender.h>
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

	std::string *macro;
    std::string *ifmacro;
	std::string *expression;


	BodyPart *bodypart;
	Text *text;
	Link *link;
	FormattedText *formattedtext;


	//TODO: add Syntax Tree classes
}

%token
	<token> PASSAGE_START
	<token> NEWLINE
    <token> EOF_TOKEN

	<token> TAGS_OPEN
	<token> TAGS_CLOSE

	<string> TITLE
	<string> TAG

	<string> VARIABLE

	<token> LINK_OPEN
	<token> LINK_CLOSE
    <token> LINK_SEPARATOR

	<token> MACRO_OPEN
	<token> MACRO_CLOSE

	<token> MACRO_IF
	<token> MACRO_ELSE
	<token> MACRO_ENDIF

	<token> MACRO_PRINT
	<token> MACRO_DISPLAY
	<token> EXPR_RANDOM
	<token> EXPR_VISITED
	<token> EXPR_PREVIOUS
	<token> EXPR_TURNS

	<token> EXPR_OPEN
	<token> EXPR_CLOSE

	<token> EXPR_ADD
	<token> EXPR_SUB
	<token> EXPR_MUL
	<token> EXPR_DIV
	<token> EXPR_MOD

	<token> EXPR_GT
	<token> EXPR_GTE
	<token> EXPR_LT
	<token> EXPR_LTE

	<token> EXPR_IS
	<token> EXPR_EQ
	<token> EXPR_NEQ

	<token> EXPR_AND
	<token> EXPR_OR

	<token> EXPR_TO
	<token> EXPR_ASSGN

	<string> INT

	<string> FORMATTING_OPEN
	<string> FORMATTING_CLOSE
	<string> FORMATTING
    <token> FORMATTING_COMMENT_OPEN
    <token> FORMATTING_COMMENT_CLOSE
    <token> FORMATTING_ERROR_STYLING
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

%type <macro> macro
%type <expression> expression
%type <ifmacro> ifmacro

%start TweeDocument

%%

TweeDocument :
    passages
    {
    LOG_DEBUG << "Parser: TweeDocument -> passages";
    tweeStructure = $1;
    }
  ;

passages :
    passages passage
    {
    LOG_DEBUG << "Parser: passages -> passages passage: " << "pass passages:type(TweeFile) to top:passages:type(TweeFile)" <<"\n"<< "add the passage:type(Passage) to the passages:type(TweeFile)";
    $$ = $1;

    *$$ += *$2;
    delete $2;
    }
	|passage
	{
	LOG_DEBUG << "Parser: passages -> passage: "<< "create a passages:type(TweeFile)"<< "add passage:type(Passage) to passages:type(TweeFile)";

    $$ = new TweeFile();

    *$$ += *$1;
    delete $1;
    delete $1;
    }
  ;
passage :
    head body
    {
    LOG_DEBUG << "Parser: passage -> head body: "<< "create a passage:type(Passage) out of the head:type(Head) and body::type(Body) objects";


    $$ = new Passage(*$1, *$2);
    delete $1;
    delete $2;
    delete $1;
    delete $2;
    }
  ;

head :
    PASSAGE_START title
    {
    LOG_DEBUG << "Parser: head -> PASSAGE_START title: "<< "pass title:type(Head) to head:type(Head)";
        $$ = $2;
    }
  ;

title :
    TITLE NEWLINE
    {
    LOG_DEBUG << "Parser: title -> TITLE NEWLINE: "<< "create title:type(Head) out of token:TITLE";
        $$ = new Head(*$1);
    delete $1;
    delete $1;
    }
    |TITLE TAGS_OPEN tags TAGS_CLOSE NEWLINE
    {
    LOG_DEBUG << "Parser: title -> TITLE TAGS_OPEN tags TAGS_CLOSE NEWLINE: "<< "create title:type(Head) out of token:TITLE";
        //TODO: incorporate tags
    $$ = new Head(*$1);
    delete $1;
    }
  ;

tags :
    tags TAG
    {
    LOG_DEBUG << "Parser: tags -> tags TAG";
    //TODO: TAG token vector: tags TAG production
    }
    |TAG
    {
    LOG_DEBUG << "Parser: tags -> TAG";
    //TODO: TAG token vector: tags TAG production
    }
  ;

body :
    bodypart
    {
    LOG_DEBUG << "Parser: body -> bodypart: "<< "create top:body:type(Body)"<< "add bodypart:type(BodyPart) to top:body:type(Body)";
        $$ = new Body();
        *$$ += *$1;
    delete $1;
    }
    |body bodypart
    {
    LOG_DEBUG << "Parser: body -> body bodypart: "<< "pass body:type(Body) to top:body:type(Body)"<< "add bodypart:type(BodyPart) to top:body:type(Body)";
        $$ = $1;
        *$$ += *$2;
    delete $2;
    }
  ;

bodypart :
    text
    {
    LOG_DEBUG << "Parser: bodypart -> text: "<< "pass text:type(text) to bodypart:type(BodyPart)";
        $$ = $1;
    }
    |NEWLINE
    {
    LOG_DEBUG << "Parser: bodypart -> NEWLINE: "<< "create top:text:type(Text) with a \"\\n\"";
        $$ = new Text("\n");
    }
    |link
    {
    LOG_DEBUG << "Parser: bodypart -> link: "<< "pass link:type(Link) to bodypart:type(BodyPart)";
        $$ = $1;
    }
    |macro
    {
    LOG_DEBUG << "Parser: bodypart -> macro: "<< "pass macro:type(--Text--) to bodypart:type(BodyPart)";
    //TODO: implement Macro:BodyType
    $$ = new Text(*$1);
    delete $1;
    }
    |formatted
    {
    LOG_DEBUG << "Parser: bodypart -> formatted: "<< "pass formatted:type(--Text--) to bodypart:type(BodyPart)";
    //TODO: implement FormattedText parsing
        $$ = $1;
    }
  ;

text :
    TEXT_TOKEN
    {
    LOG_DEBUG << "Parser: text -> TEXT_TOKEN: "<< "create top:text:type(Text)";
        $$ = new Text(*$1);
    delete $1;
    }
  ;

link :
    LINK_OPEN TEXT_TOKEN LINK_CLOSE
    {
    LOG_DEBUG << "Parser: link -> LINK_OPEN TEXT_TOKEN LINK_CLOSE: "<< "create top:link:type(Link) with 2:token:TEXT_TOKEN & 2:token:TEXT_TOKEN";
        $$ = new Link(*$2);
    delete $2;
    }
    |LINK_OPEN TEXT_TOKEN LINK_SEPARATOR TEXT_TOKEN LINK_CLOSE
    {
    LOG_DEBUG << "Parser: link -> LINK_OPEN TEXT_TOKEN LINK_SEPARATOR TEXT_TOKEN LINK_CLOSE: "<< "create top:link:type(Link) with 4:token:TEXT_TOKEN & 2:token:TEXT_TOKEN";
        $$ = new Link(*$4, *$2);
    delete $4;
    delete $2;
    }
  ;

macro :
    MACRO_OPEN TEXT_TOKEN MACRO_CLOSE
    {
    //TODO: data model: implement macro
    LOG_DEBUG << "Parser: macro -> MACRO_OPEN TEXT_TOKEN MACRO_CLOSE: "<< "create top:macro:type(--Text--) with 2:token:TEXT_TOKEN";
    *$$ = std::string("just some text inside a macro");
    }
    |MACRO_OPEN ifmacro MACRO_CLOSE
    {
    $$ = $2;
    delete $2;
    }
    |MACRO_OPEN expression MACRO_CLOSE
    {
    $$ = $2;
    delete $2;
    }
  ;

ifmacro :
    MACRO_IF expression
    {
    LOG_DEBUG << "Parser: macro -> MACRO_OPEN TEXT_TOKEN MACRO_CLOSE: "<< "create top:macro:type(--Text--) with 2:token:TEXT_TOKEN";
    //TODO: implement ifmacro
    *$$ = std::string("ifmacro");
    }
  ;

expression :
    VARIABLE EXPR_ASSGN INT
    {
    *$$ = std::string("matched an expression assignment");
    }
    |VARIABLE EXPR_IS INT
    {
    *$$ = std::string("matched an expression assignment");
    }
  ;
formatted:
    FORMATTING_OPEN TEXT_TOKEN FORMATTING_CLOSE
    {
    //TODO:check if F_OPEN and F_CLOSE are the same
    //TODO:check if anything needs to be deleted here
    LOG_DEBUG << "Parser: formatted -> FORMATTING_OPEN TEXT_TOKEN FORMATTING_CLOSE: "<< "create top:formatted:type(--Text--) with 2:token:TEXT_TOKEN";
        $$ = new FormattedText(*$2, true, false, false);
    delete $2;
    }
    |FORMATTING_OPEN formatted FORMATTING_CLOSE
    {
    //TODO:check if F_OPEN and F_CLOSE are the same
    //TODO:check if anything needs to be deleted here
    LOG_DEBUG << "Parser: formatted -> FORMATTING_OPEN formatted FORMATTING_CLOSE: "<< "pass formatted:type(--Text--) up to top:formatted:type(--Text--)";
        $$ = $2;
    }
    |FORMATTING TEXT_TOKEN FORMATTING
    {
    //TODO:check if FORMATTING($1) and FORMATTING($3) are the same
    LOG_DEBUG << "Parser: formatted -> FORMATTING TEXT_TOKEN FORMATTING: "<< "pass formatted:type(--Text--) up to top:formatted:type(--Text--)";

    $$ = new FormattedText(*$2, true, false, false);
    delete $1;
    delete $2;
    delete $3;
    }
    |FORMATTING formatted FORMATTING
    {
    //TODO:check if FORMATTING($1) and FORMATTING($3) are the same
    //TODO:check if anything needs to be deleted here
    LOG_DEBUG << "Parser: formatted -> FORMATTING formatted FORMATTING: "<< "pass formatted:type(--Text--) up to top:formatted:type(--Text--)";
        $$ = $2;
    }
  ;

%%

// We have to implement the error function
//TODO: use this function to pass parse errors to the exceptions, also look for yyerror
void Twee::BisonParser::error(const std::string& msg) {
	LOG_ERROR  << "Error: " << msg ;
}


// Now that we have the Parser declared, we can declare the Scanner and implement
// the yylex function

#include "TweeScanner.h"
static int yylex(Twee::BisonParser::semantic_type * yylval, Twee::TweeScanner &scanner) {
	return scanner.yylex(yylval);
}