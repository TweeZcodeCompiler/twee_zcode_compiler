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
    #include "include/Passage/Body/Macros/Visited.h"
    #include "include/Passage/Body/Macros/Previous.h"
    #include "include/Passage/Body/Macros/Turns.h"
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
    #include "include/Passage/Body/Expressions/Random.h"

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

    std::string getBinOp(int i){
        switch (static_cast<BinOps>(i)) {
            case (BinOps::ADD):
                return "ADD";
            case (BinOps::MUL):
                return "MUL";
            case (BinOps::SUB):
                return "SUB";
            case (BinOps::DIV):
                return "DIV";
            case (BinOps::MOD):
                return "MOD";
            case (BinOps::AND):
                return "AND";
            case (BinOps::OR):
                return "OR";
            case (BinOps::LT):
                return "LT";
            case (BinOps::LTE):
                return "LTE";
            case (BinOps::GT):
                return "GT";
            case (BinOps::GTE):
                return "GTE";
            case (BinOps::IS):
                return "IS";
            case (BinOps::NEQ):
                return "NEQ";
            case (BinOps::TO):
                return "TO";
        }
    }

    std::string getUnOp(int i){
        switch (static_cast<UnOps>(i)) {
            case (UnOps::MINUS):
                return "MINUS";
            case (UnOps::PLUS):
                return "PLUS";
            case (UnOps::NOT):
                return "NOT";
        }
    }

}


%union{
	std::string *string;
	int token;
	int integer;
	bool boolean;

	TweeFile *tweefile;

	Passage *passage;

	Head *head;
	Text *tag;
    Newline *newline;

	Body *body;

	Macro *macro;
	Print *print;
	SetMacro *setmacro;

	Display *display;
    Visited *visited;
    Previous *previous;
    Turns *turns;
    Random *random;


    IfMacro *ifmacro;
    ElseIfMacro *elseifmacro;
    ElseMacro *elsemacro;
    EndIfMacro *endifmacro;


	Expression *expression;
	Variable *variable;
	Const<int> *intconst;
	Const<bool> *boolconst;
	Const<std::string> *strconst;

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
    <string> EXPR_STR

    <token> EXPR_TRUE
    <token> EXPR_FALSE
    <token> EXPR_STR_LIMITER
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
    <token> FUNC_SEPARATOR

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

%type <display> display
%type <visited> visited
%type <previous> previous
%type <turns> turns

%type <expression> expression
%type <expression> expressionAssignment
%type <expression> expressionCompare
%type <expression> expressionLogic
%type <expression> expressionAddSub
%type <expression> expressionMulDivMod
%type <expression> expressionUnary
%type <expression> expressionTop

%type <expression> random

%type <integer> operatorAssignment
%type <integer> operatorCompare
%type <integer> operatorLogic
%type <integer> operatorAddSub
%type <integer> operatorMulDivMod
%type <integer> operatorUnary
%type <variable> variable
%type <intconst> integer
%type <boolconst> boolean


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
    |display
    {
    LOG_DEBUG << "macro -> display: pass display:type(Display) to macro:type(Macro)";
    $$ = $1;
    }
    |visited
    {
    LOG_DEBUG << "macro -> visited: pass visited:type(Visited) to macro:type(Macro)";
    $$ = $1;
    }
    |previous
    {
    LOG_DEBUG << "macro -> previous: pass previous:type(Previous) to macro:type(Macro)";
    $$ = $1;
    }
    |turns
    {
    LOG_DEBUG << "macro -> previous: pass turns:type(Turns) to macro:type(Macro)";
    $$ = $1;
    }
  ;

print:
    MACRO_OPEN MACRO_PRINT expression MACRO_CLOSE
    {
    LOG_DEBUG << "print -> MACRO_OPEN MACRO_PRINT expression MACRO_CLOSE create top:macro:type(--Print--) with 3:expression";
    $$ = new Print($3);
    delete $3;
    }
    |
    MACRO_OPEN MACRO_PRINT expressionAssignment MACRO_CLOSE
    {
    LOG_DEBUG << "print -> MACRO_OPEN MACRO_PRINT expression MACRO_CLOSE create top:macro:type(--Print--) with 3:expression";
    $$ = new Print($3);
    delete $3;
    }
    |MACRO_OPEN expression MACRO_CLOSE
    {
    //TODO:check if we need error handling here
    LOG_DEBUG << "print -> MACRO_OPEN expression MACRO_CLOSE create top:macro:type(--Print--) with 2:expression";
    $$ = new Print($2);
    delete $2;
    }
  ;

display:
    MACRO_OPEN MACRO_DISPLAY EXPR_STR EXPR_CLOSE MACRO_CLOSE
    {
    LOG_DEBUG << "display -> MACRO_OPEN MACRO_DISPLAY EXPR_OPEN strconst MACRO_CLOSE MACRO_CLOSE: create new Display($4);";
    $$ = new Display(*$3);
    delete $3;
    }
  ;

visited:
    MACRO_OPEN EXPR_VISITED EXPR_STR EXPR_CLOSE MACRO_CLOSE
    {
    LOG_DEBUG << "visited -> MACRO_OPEN EXPR_VISITED EXPR_OPEN strconst EXPR_CLOSE MACRO_CLOSE: create new Visited($4)";
    $$ = new Visited(*$3);
    delete $3;
    }
    |MACRO_OPEN EXPR_VISITED EXPR_CLOSE MACRO_CLOSE
    {
    LOG_DEBUG << "visited -> MACRO_OPEN EXPR_VISITED EXPR_OPEN strconst EXPR_CLOSE MACRO_CLOSE: create new Visited($4)";
    $$ = new Visited("");
    }
  ;

previous:
    MACRO_OPEN EXPR_PREVIOUS EXPR_CLOSE MACRO_CLOSE
    {
    LOG_DEBUG << "previous -> MACRO_OPEN EXPR_PREVIOUS EXPR_CLOSE MACRO_CLOSE: create $$ = new Previous()";
    $$ = new Previous();
    }
  ;

turns:
    MACRO_OPEN EXPR_TURNS EXPR_CLOSE MACRO_CLOSE
    {
    LOG_DEBUG << "turns -> MACRO_OPEN EXPR_PREVIOUS EXPR_CLOSE MACRO_CLOSE: create $$ = new Turns()";
    $$ = new Turns();
    }
  ;

setmacro:
    MACRO_OPEN MACRO_SET expressionAssignment MACRO_CLOSE
    {
    LOG_DEBUG << "setmacro -> MACRO_OPEN MACRO_SET expression MACRO_CLOSE :";
    $$ = new SetMacro($3);
    }
    |MACRO_OPEN expressionAssignment MACRO_CLOSE
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
    $$ = new IfMacro($3);
    }
  ;

elseifmacro:
    MACRO_OPEN MACRO_ELSE_IF expression MACRO_CLOSE
    {
    LOG_DEBUG << "Parser: elseifmacro -> MACRO_OPEN MACRO_ELSE_IF expression MACRO_CLOSE: "<< "matched, make a dummy elseif";
    $$ = new ElseIfMacro($3);
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

    /*Operators in precedence*/
operatorAssignment:
    EXPR_TO{$$ = 13;}
  ;

operatorCompare:
     EXPR_LT{$$ = 7;}
    |EXPR_LTE{$$ = 8;}
    |EXPR_GT{$$ = 9;}
    |EXPR_GTE{$$ = 10;}
    |EXPR_IS{$$ = 11;}
    |EXPR_NEQ{$$ = 12;}
  ;

operatorLogic:
     EXPR_AND{$$ = 5;}
    |EXPR_OR{$$ = 6;}
  ;

operatorAddSub:
     EXPR_ADD{$$ = 0;}
    |EXPR_SUB{$$ = 2;}
  ;

operatorMulDivMod:
     EXPR_MUL{$$ = 1;}
    |EXPR_DIV{$$ = 3;}
    |EXPR_MOD{$$ = 4;}
  ;

operatorUnary:
     EXPR_SUB{$$ = 0;}
    |EXPR_ADD{$$ = 1;}
    |EXPR_NOT{$$ = 2;}
  ;

expressionAssignment:
    variable operatorAssignment expression
    {
    LOG_DEBUG << "expression -> variable EXPR_TO expressionCompare: created $$ = new BinaryOperation(BinOps::"+ getBinOp($2) +", $1, $3)";
    $$ = new BinaryOperation(static_cast<BinOps>($2), $1, $3);
    delete $1;
    delete $3;
    }
    |variable operatorAssignment expressionAssignment
    {
    LOG_DEBUG << "expression -> variable EXPR_TO expressionCompare: created $$ = new BinaryOperation(BinOps::"+ getBinOp($2) +", $1, $3)";
    $$ = new BinaryOperation(static_cast<BinOps>($2), $1, $3);
    delete $1;
    delete $3;
    }
  ;

expression:
   expressionCompare
    {
    LOG_DEBUG << "expression-> expressionCompare: pass expressionCompare up)";
    $$ = $1;
    }
  ;

expressionCompare:
    expressionCompare operatorCompare expressionLogic
    {
    LOG_DEBUG << "expressionCompare -> expressionCompare EXPR_"+ getBinOp($2) +" expressionLogic: created $$ = new BinaryOperation(BinOps::"+ getBinOp($2) +", $1, $3)";
    $$ = new BinaryOperation(static_cast<BinOps>($2), $1, $3);
    delete $1;
    delete $3;
    }
   |expressionLogic
    {
    LOG_DEBUG << "expressionCompare-> expressionLogic: pass expressionLogic up)";
    $$ = $1;
    }
  ;

expressionLogic:
    expressionLogic operatorLogic expressionAddSub
    {
    LOG_DEBUG << "expressionLogic -> expressionLogic EXPR_"+ getBinOp($2) +" expressionAddSub: created $$ = new BinaryOperation(BinOps::"+ getBinOp($2) +", $1, $3)";
    $$ = new BinaryOperation(static_cast<BinOps>($2), $1, $3);
    delete $1;
    delete $3;
    }
   |expressionAddSub
    {
    LOG_DEBUG << "expressionLogic-> expressionAddSub: pass expressionAddSub up)";
    $$ = $1;
    }
  ;

expressionAddSub:
    expressionAddSub operatorAddSub expressionMulDivMod
    {
    LOG_DEBUG << "expressionAddSub -> expressionAddSub EXPR_"+ getBinOp($2) +" expressionMulDivMod: created $$ = new BinaryOperation(BinOps::"+ getBinOp($2) +", $1, $3)";
    $$ = new BinaryOperation(static_cast<BinOps>($2), $1, $3);
    delete $1;
    delete $3;
    }
   |expressionMulDivMod
    {
    LOG_DEBUG << "expressionAddSub-> expressionMulDivMod: pass expressionMulDivMod up)";
    $$ = $1;
    }
  ;

expressionMulDivMod:
    expressionMulDivMod operatorMulDivMod expressionUnary
    {
    LOG_DEBUG << "expressionMulDivMod -> expressionMulDivMod EXPR_"+ getBinOp($2) +" expressionUnary: created $$ = new BinaryOperation(BinOps::"+ getBinOp($2) +", $1, $3)";
    $$ = new BinaryOperation(static_cast<BinOps>($2), $1, $3);
    delete $1;
    delete $3;
    }
   |expressionUnary
    {
    LOG_DEBUG << "expressionMulDivMod-> expressionUnary: pass expressionUnary up)";
    $$ = $1;
    }
  ;

expressionUnary:
    operatorUnary expressionTop
    {
    //TODO: check for not int without (), it leads to an error in twee
    LOG_DEBUG << "expressionUnary -> "+getUnOp($1)+" expressionTop: created $$ = new UnaryOperation(UnOps::"+getUnOp($1)+", $2)";
    $$ = new UnaryOperation(static_cast<UnOps>($1), $2);
    delete $2;
    }
   |expressionTop
    {
    LOG_DEBUG << "expressionMulDivMod-> expressionTop: pass expressionTop up)";
    $$ = $1;
    }
  ;

expressionTop:
    EXPR_OPEN expressionCompare EXPR_CLOSE
    {
    LOG_DEBUG << "expressionTop-> EXPR_OPEN expressionCompare EXPR_CLOSE: pass expressionCompare up)";
    $$ = $2;
    }
   |variable
    {
    LOG_DEBUG << "expressionTop-> variable: pass variable up)";
    $$ = $1;
    }
   |integer
    {
    LOG_DEBUG << "expressionTop-> integer: pass integer up)";
    $$ = $1;
    }
   |boolean
    {
    LOG_DEBUG << "expressionTop-> boolean: pass boolean up)";
    $$ = $1;
    }
   |random
    {
    LOG_DEBUG << "expressionTop-> random: pass random function up)";
    $$ = $1;
    }
  ;


random:
    MACRO_OPEN EXPR_RANDOM expression expression EXPR_CLOSE MACRO_CLOSE
    {
    LOG_DEBUG << "random -> MACRO_OPEN EXPR_RANDOM expression expression EXPR_CLOSE MACRO_CLOSE: create new Visited($4)";
    $$ = new Random($3, $4);
    }
  ;

variable:
    EXPR_VAR
    {
    LOG_DEBUG << "variable-> EXPR_VAR: create new Variable(*$1)";
    $$ = new Variable(*$1);
    delete $1;
    }
   ;

integer:
    EXPR_INT
    {
    LOG_DEBUG << "intconst-> EXPR_INT: create $$ = new Const<int> ($1)";
    $$ = new Const<int> ($1);
    }
   ;

boolean:
    EXPR_TRUE
    {
    LOG_DEBUG << "boolean-> EXPR_TRUE: create new Const<bool> (true)";
    $$ = new Const<bool> (true);
    }
    |EXPR_FALSE
    {
    LOG_DEBUG << "boolean-> EXPR_FALSE: create new Const<bool> (false)";
    $$ = new Const<bool> (false);
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
