%defines "GeneratedTweeParser.h"
%require "3.0.2"
%define api.namespace {Twee}
%define parser_class_name {BisonParser}
%parse-param { Twee::TweeScanner &scanner }
%lex-param   { Twee::TweeScanner &scanner }
%define parse.error verbose
%define parse.trace

%skeleton "lalr1.cc"

%code requires {
	#include <memory>
	#include <stdio.h>
	#include <vector>

    #include "include/TweeFile.h"
    #include "include/Passage/Passage.h"
    #include "include/Passage/Body/Text.h"
    #include "include/Passage/Body/Newline.h"
    #include "include/Passage/Body/Link.h"
    #include "include/Passage/Body/Macros/Display.h"
    #include "include/Passage/Body/Macros/Print.h"
    #include "include/Passage/Body/Macros/IfMacro.h"
    #include "include/Passage/Body/Macros/ElseIfMacro.h"
    #include "include/Passage/Body/Macros/ElseMacro.h"
    #include "include/Passage/Body/Macros/EndIfMacro.h"
    #include "include/Passage/Body/Macros/SetMacro.h"
    #include "include/Passage/Body/Expressions/Expression.h"
    #include "include/Passage/Body/Expressions/Const.h"
    #include "include/Passage/Body/Expressions/UnaryOperation.h"
    #include "include/Passage/Body/Expressions/BinaryOperation.h"
    #include "include/Passage/Body/Expressions/Variable.h"

    #include <plog/Log.h>
    #include <plog/Appenders/ConsoleAppender.h>
	extern TweeFile *tweeStructure; /* the result data model */

	// forward declare the Scanner class
	namespace Twee {
		class TweeScanner;
	}

	#define YYDEBUG 1
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
	int integer;

	TweeFile *tweefile;

	Passage *passage;

	Head *head;
	Text *tag;
    Newline *newline;

	Body *body;

	Macro *macro;
	Print *print;
	SetMacro *setmacro;
    IfMacro *ifmacro;
    ElseIfMacro *elseifmacro;
    ElseMacro *elsemacro;
    EndIfMacro *endifmacro;


	Expression *expression;
	Variable *variable;
	Const<int> *intconst;

	BodyPart *bodypart;
	Text *text;
	Link *link;
    BinaryOperation *binaryOperation;
    
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

    <integer> EXPR_INT
    <string> EXPR_VAR

    <token> LINK_OPEN
    <token> LINK_CLOSE
    <token> LINK_SEPARATOR
    <token> LINK_SMALL_CLOSE
    <token> LINK_EXPRESSION_OPEN

	<token> MACRO_OPEN
	<token> MACRO_CLOSE
	
	<token> MACRO_IF
	<token> MACRO_ELSE
	<token> MACRO_ELSE_IF
	<token> MACRO_ENDIF
	
	<token> MACRO_PRINT
	<token> MACRO_SET
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
    <token> EXPR_NEQ

	<token> EXPR_AND
	<token> EXPR_OR
	<token> EXPR_NOT
	
	<token> EXPR_TO

	<string> FORMATTING_OPEN
	<string> FORMATTING_CLOSE
	<string> FORMATTING
    <token> FORMATTING_COMMENT_OPEN
    <token> FORMATTING_COMMENT_CLOSE
    <token> FORMATTING_ERROR_STYLING
    <string> TEXT_TOKEN

// association definitions, grouped by decreasing precedence in java script

%right EXPR_TO
%left EXPR_GTE EXPR_GT EXPR_LTE EXPR_LT
%left EXPR_IS EXPR_NEQ
%left EXPR_AND
%left EXPR_OR
%left EXPR_ADD EXPR_SUB
%left EXPR_MUL EXPR_DIV EXPR_MOD
%right UMINUS UPLUS EXPR_NOT


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
%type <newline> newline

%type <macro> macro
%type <ifmacro> ifmacro
%type <elseifmacro> elseifmacro
%type <elsemacro> elsemacro
%type <endifmacro> endifmacro
%type <print> print
%type <setmacro> setmacro
%type <expression> assignment
%type <expression> expression
%type <expression> expression_two_operands
%type <variable> variable
%type <intconst> intconst

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
    }
  ;
passage :
    head body
    {
    LOG_DEBUG << "Parser: passage -> head body: "<< "create a passage:type(Passage) out of the head:type(Head) and body::type(Body) objects";
    $$ = new Passage(*$1, *$2);
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
    |newline
    {
    LOG_DEBUG << "Parser: bodypart -> newline: "<< "pass newline:type(Newline) to bodypart:type(BodyPart)";
    $$ = $1;
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
    $$ = new Text($1->to_string());
    }
  ;

text :
    TEXT_TOKEN
    {
    LOG_DEBUG << "Parser: text -> TEXT_TOKEN: "<< "create top:text:type(Text)";
    $$ = new Text(*$1);
    delete $1;
    }
    |FORMATTING_OPEN TEXT_TOKEN FORMATTING_CLOSE
    {
    //TODO:check if F_OPEN and F_CLOSE are the same
    //TODO:check if anything needs to be deleted here
    LOG_DEBUG << "Parser: formatted -> FORMATTING_OPEN TEXT_TOKEN FORMATTING_CLOSE: "<< "create top:formatted:type(--Text--) with 2:token:TEXT_TOKEN";
    $$ = new Text(*$2);
    delete $1;
    delete $2;
    delete $3;
    }
    |FORMATTING_OPEN text FORMATTING_CLOSE
    {
    //TODO:check if F_OPEN and F_CLOSE are the same
    //TODO:check if anything needs to be deleted here
    LOG_DEBUG << "Parser: formatted -> FORMATTING_OPEN formatted FORMATTING_CLOSE: "<< "pass formatted:type(--Text--) up to top:formatted:type(--Text--)";
    delete $1;
    delete $3;
    }
    |FORMATTING TEXT_TOKEN FORMATTING
    {
    //TODO:check if FORMATTING($1) and FORMATTING($3) are the same
    LOG_DEBUG << "Parser: formatted -> FORMATTING TEXT_TOKEN FORMATTING: "<< "pass formatted:type(--Text--) up to top:formatted:type(--Text--)";
    $$ = new Text(*$2);
    delete $1;
    delete $2;
    delete $3;
    }
    |FORMATTING text FORMATTING
    {
    //TODO:check if FORMATTING($1) and FORMATTING($3) are the same
    //TODO:check if anything needs to be deleted here
    LOG_DEBUG << "Parser: formatted -> FORMATTING formatted FORMATTING: "<< "pass formatted:type(--Text--) up to top:formatted:type(--Text--)";
    $$ = $2;
    delete $1;
    delete $3;
    }
  ;

newline:
    NEWLINE {
    LOG_DEBUG << "Parser: newline -> NEWLINE: "<< "create top:newline:type(Newline)";
    $$ = new Newline();
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
    |LINK_OPEN TEXT_TOKEN LINK_SMALL_CLOSE LINK_EXPRESSION_OPEN expression LINK_CLOSE
    {
    LOG_DEBUG << "Parser: link -> LINK_OPEN TEXT_TOKEN LINK_SEPARATOR TEXT_TOKEN LINK_CLOSE: "<< "create top:link:type(Link) with 4:token:TEXT_TOKEN & 2:token:TEXT_TOKEN";
    //TODO: expressions in links
    $$ = new Link(*$2);
    delete $2;
    }
    |LINK_OPEN TEXT_TOKEN LINK_SEPARATOR TEXT_TOKEN LINK_SMALL_CLOSE LINK_EXPRESSION_OPEN expression LINK_CLOSE
    {
    LOG_DEBUG << "Parser: link -> LINK_OPEN TEXT_TOKEN LINK_SEPARATOR TEXT_TOKEN LINK_CLOSE: "<< "create top:link:type(Link) with 4:token:TEXT_TOKEN & 2:token:TEXT_TOKEN";
    //TODO: expressions in links
    $$ = new Link(*$4, *$2);
    delete $4;
    delete $2;
    }
  ;

macro :
    print
    {
    LOG_DEBUG << "macro -> print: pass print:type(Print) to macro:type(Macro)";
    $$ = $1;
    }
    |setmacro
    {
    LOG_DEBUG << "macro -> setmacro: pass setmacro:type(SetMacro) to macro:type(Macro)";
    $$ = $1;
    }
    |ifmacro
    {
    LOG_DEBUG << "macro -> ifmacro: pass ifmacro:type(IfMacro) to macro:type(Macro)";
    $$ = $1;
    }
    |elseifmacro
    {
    LOG_DEBUG << "macro -> elseifmacro: pass elseifmacro:type(ElseIfMacro) to macro:type(Macro)";
    $$ = $1;
    }
    |elsemacro
    {
    LOG_DEBUG << "macro -> else: pass else:type(ElseMacro) to macro:type(Macro)";
    $$ = $1;
    }
    |endifmacro
    {
    LOG_DEBUG << "macro -> endifmacro: pass endifmacro:type(EndIfMacro) to macro:type(Macro)";
    $$ = $1;
    }
  ;

print:
    MACRO_OPEN MACRO_PRINT expression MACRO_CLOSE
    {
    LOG_DEBUG << "print -> MACRO_OPEN MACRO_PRINT expression MACRO_CLOSE create top:macro:type(--Print--) with 2:expression";
    $$ = new Print($3);
    }
    |MACRO_OPEN expression MACRO_CLOSE
    {
    //TODO:check if we need error handling here
    LOG_DEBUG << "print -> MACRO_OPEN expression MACRO_CLOSE create top:macro:type(--Print--) with 2:expression";
    $$ = new Print($2);
    }
  ;

setmacro:
    MACRO_OPEN MACRO_SET assignment MACRO_CLOSE
    {
    LOG_DEBUG << "setmacro -> MACRO_OPEN MACRO_SET expression MACRO_CLOSE :";
    $$ = new SetMacro($3);
    }
    |MACRO_OPEN assignment MACRO_CLOSE
    {
    //TODO:check if we need error handling here
    LOG_DEBUG << "setmacro -> MACRO_OPEN MACRO_PRINT expression MACRO_CLOSE :";
    $$ = new SetMacro($2);
    }
  ;

ifmacro:
    MACRO_OPEN MACRO_IF expression MACRO_CLOSE
    {
    LOG_DEBUG << "Parser: ifmacro -> MACRO_OPEN MACRO_IF expression MACRO_CLOSE: "<< "matched, make a dummy if";
    $$ = new IfMacro(new Variable("VAR"));
    }
  ;

elseifmacro:
    MACRO_OPEN MACRO_ELSE_IF expression MACRO_CLOSE
    {
    LOG_DEBUG << "Parser: elseifmacro -> MACRO_OPEN MACRO_ELSE_IF expression MACRO_CLOSE: "<< "matched, make a dummy elseif";
    $$ = new ElseIfMacro(new Variable("VAR"));
    }
  ;

elsemacro:
    MACRO_OPEN MACRO_ELSE MACRO_CLOSE
    {
    LOG_DEBUG << "Parser: elsemacro -> MACRO_OPEN MACRO_ELSE MACRO_CLOSE: "<< "matched, make a dummy else";
    $$ = new ElseMacro();
    }
  ;

endifmacro:
    MACRO_OPEN MACRO_ENDIF MACRO_CLOSE
    {
    LOG_DEBUG << "Parser: endifmacro -> MACRO_OPEN MACRO_ENDIF MACRO_CLOSE: "<< "matched, make a dummy endifmacro";
    $$ = new EndIfMacro();
    }
  ;

expression :
    EXPR_OPEN expression EXPR_CLOSE
    {
    LOG_DEBUG << "expression -> EXPR_OPEN expression EXPR_CLOSE: passed expression upwards";
    $$ = $2;
    }
    |EXPR_NOT expression
    {
    LOG_DEBUG << "expression -> EXPR_NOT expression: created $$ = new UnaryOperation(UnOps::NOT, $2)";
    $$ = new UnaryOperation(UnOps::NOT, $2);
    delete $2;
    }
    |EXPR_SUB expression %prec UMINUS
    {
    LOG_DEBUG << "expression -> EXPR_SUB expression %prec UMINUS: created $$ = new UnaryOperation(UnOps::MINUS, $1, $3)";
    $$ = new UnaryOperation(UnOps::MINUS, $2);
    delete $2;
    }
    |EXPR_ADD expression %prec UPLUS
    {
    LOG_DEBUG << "expression -> EXPR_ADD expression %prec UPLUS: created $$ = new UnaryOperation(UnOps::PLUS, $1, $3)";
    $$ = new UnaryOperation(UnOps::PLUS, $2);
    delete $2;
    }
    |expression EXPR_DIV expression
    {
    LOG_DEBUG << "expression -> expression EXPR_DIV expression: created $$ = new BinaryOperation(BinOps::DIV, $1, $3)";
    $$ = new BinaryOperation(BinOps::DIV, $1, $3);
    delete $1;
    delete $3;
    }
    |expression EXPR_MOD expression
    {
    LOG_DEBUG << "expression -> expression EXPR_MOD expression: created $$ = new BinaryOperation(BinOps::MOD, $1, $3)";
    $$ = new BinaryOperation(BinOps::MOD, $1, $3);
    delete $1;
    delete $3;
    }
    |expression EXPR_ADD expression
    {
    LOG_DEBUG << "expression -> expression EXPR_ADD expression: created $$ = new BinaryOperation(BinOps::ADD, $1, $3)";
    $$ = new BinaryOperation(BinOps::ADD, $1, $3);
    delete $1;
    delete $3;
    }
    |expression EXPR_SUB expression
    {
    LOG_DEBUG << "expression -> expression EXPR_SUB expression: created $$ = new BinaryOperation(BinOps::SUB, $1, $3)";
    $$ = new BinaryOperation(BinOps::SUB, $1, $3);
    delete $1;
    delete $3;
    }
    |expression EXPR_GTE expression
    {
    LOG_DEBUG << "expression -> expression EXPR_GTE expression: created $$ = new BinaryOperation(BinOps::GTE, $1, $3)";
    $$ = new BinaryOperation(BinOps::GTE, $1, $3);
    delete $1;
    delete $3;
    }
    |expression EXPR_GT expression
    {
    LOG_DEBUG << "expression -> expression EXPR_GT expression: created $$ = new BinaryOperation(BinOps::GT, $1, $3)";
    $$ = new BinaryOperation(BinOps::GT, $1, $3);
    delete $1;
    delete $3;
    }
    |expression EXPR_LTE expression
    {
    LOG_DEBUG << "expression -> expression EXPR_LTE expression: $$ = new BinaryOperation(BinOps::LTE, $1, $3)";
    $$ = new BinaryOperation(BinOps::LTE, $1, $3);
    delete $1;
    delete $3;
    }
    |expression EXPR_LT expression
    {
    LOG_DEBUG << "expression -> expression EXPR_LT expression: created $$ = new BinaryOperation(BinOps::LT, $1, $3)";
    $$ = new BinaryOperation(BinOps::LT, $1, $3);
    delete $1;
    delete $3;
    }
    |expression EXPR_NEQ expression
    {
    LOG_DEBUG << "expression-> expression EXPR_NEQ expression: created $$ = new BinaryOperation(BinOps::NEQ, $1, $3)";
    $$ = new BinaryOperation(BinOps::NEQ, $1, $3);
    delete $1;
    delete $3;
    }
    |expression EXPR_AND expression
    {
    LOG_DEBUG << "expression-> expression EXPR_AND expression: created $$ = new BinaryOperation(BinOps::AND, $1, $3)";
    $$ = new BinaryOperation(BinOps::AND, $1, $3);
    delete $1;
    delete $3;
    }
    |expression EXPR_OR expression
    {
    LOG_DEBUG << "expression-> expression EXPR_OF expression: created $$ = new BinaryOperation(BinOps::OR, $1, $3)";
    $$ = new BinaryOperation(BinOps::OR, $1, $3);
    delete $1;
    delete $3;
    }
    |variable EXPR_IS expression
    {
    LOG_DEBUG << "expression-> variable EXPR_IS expression: created $$ = new BinaryOperation(BinOps::IS, $1, $3)";
    $$ = new BinaryOperation(BinOps::IS, $1, $3);
    delete $1;
    delete $3;
    }
    |expression EXPR_IS expression
    {
    LOG_DEBUG << "expression-> expression EXPR_IS expression: created new BinaryOperation(BinOps::IS, $1, $3)";
    $$ = new BinaryOperation(BinOps::IS, $1, $3);
    delete $1;
    delete $3;
    }
    |intconst
    {
    LOG_DEBUG << "expression-> intconst: )";
    $$ = $1;
    }
    |expression_two_operands
    {
    LOG_DEBUG << "expression-> expression_two_operands: )";
    $$ = $1;
    }
 ;

expression_two_operands:
    expression EXPR_MUL expression
    {
    LOG_DEBUG << "expression -> expression EXPR_MUL expression: created $$ = new BinaryOperation(BinOps::MUL, $1, $3)";
    $$ = new BinaryOperation(BinOps::MUL, *$1, *$3);
    delete $1;
    delete $3;
    }
    |
    variable EXPR_MUL expression
    {
    LOG_DEBUG << "expression -> variable EXPR_MUL expression: created $$ = new BinaryOperation(BinOps::MUL, $1, $3)";
    $$ = new BinaryOperation(BinOps::MUL, *$1, *$3);
    delete $1;
    delete $3;
    }
    |
    expression EXPR_MUL variable
    {
    LOG_DEBUG << "expression -> expression EXPR_MUL variable: created $$ = new BinaryOperation(BinOps::MUL, $1, $3)";
    $$ = new BinaryOperation(BinOps::MUL, *$1, *$3);
    delete $1;
    delete $3;
    }
    |
    variable EXPR_MUL variable
    {
    LOG_DEBUG << "expression -> variable EXPR_MUL variable: created $$ = new BinaryOperation(BinOps::MUL, $1, $3)";
    $$ = new BinaryOperation(BinOps::MUL, *$1, *$3);
    delete $1;
    delete $3;
    }
  ;

assignment:
    expression EXPR_TO expression
    {
    LOG_DEBUG << "assignment-> expression EXPR_TO expression: created $$ = new BinaryOperation(BinOps::TO, $1, $3)";
    $$ = new BinaryOperation(BinOps::TO, $1, $3);
    delete $1;
    delete $3;
    }
    |variable EXPR_TO expression
    {
    LOG_DEBUG << "assignment-> variable EXPR_TO expression: created $$ = new BinaryOperation(BinOps::TO, $1, $3)";
    $$ = new BinaryOperation(BinOps::TO, $1, $3);
    delete $1;
    delete $3;
    }

variable:
    EXPR_VAR
    {
    LOG_DEBUG << "variable-> EXPR_VAR: L";
    $$ = new Variable(*$1);
    delete $1;
    }
   ;

intconst:
    EXPR_INT
    {
    LOG_DEBUG << "intconst-> EXPR_INT: L";
    $$ = new Const<int> ($1);
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