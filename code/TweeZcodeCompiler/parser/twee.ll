/*
TweeZCodeCompiler:
Lexer Input file.
install flex on your system (build/make works with 2.5.39 on my system)

if you are here because of an error, have a look at this: http://flex.sourceforge.net/manual/Diagnostics.html#Diagnostics
TODO: YYLMAX should be considered, pushback should be considered
TODO: consider lexer speed & opt: http://flex.sourceforge.net/manual/Options-for-Scanner-Speed-and-Size.html#Options-for-Scanner-Speed-and-Size

some stated 'goals':
 convert UTF-8 to ASCII
    [\x80-\xFF] is a UTF-8 match
    also look at: http://sourceforge.net/p/flex/mailman/message/22611390/
    and: http://stackoverflow.com/questions/9611682/flexlexer-support-for-unicode
*/

%{

#include <iostream>
#include <memory>
#include <vector>
#include <string>

//TweeScanner.h for the Tokens
#include "TweeScanner.h"

#include "include/TweeFile.h"

// TODO: check memory stuff for SAVE_TOKEN, does the parser clear that?
#define SAVE_TOKEN yylval->string = new std::string(YYText(), YYLeng())
// TODO: decide if this is needed, look at that tutorial again: #define TOKEN(t) (yylval.token = t)

void lex_log(std::string message) {
    std::cout << "Lexer: " << message << " . . ." << "\n";
}

%}

 /*misc options*/
%option noyywrap

 /* debug options: http://flex.sourceforge.net/manual/Debugging-Options.html#Debugging-Options */
%option debug warn verbose

 /* output relevant stuff*/
%option yyclass="Twee::TweeScanner" c++
%option outfile="GeneratedTweeLexer.cpp"

 /*Definitions */
 /*TODO: needed Expressions*/

ASCII_LOWER_CASE        abcdefghijklmnopqrstuvwxyz
ASCII_UPPER_CASE        ABCDEFGHIJKLMNOPQRSTUVWXYZ
ASCII_NUMBER            0123456789
 /*TODO: still experimental*/
ASCII_SYMBOL_NOTOKEN    -_=+\\/?.,
ASCII_WHITESPACE        \t

PASSAGE_START           ::
NEWLINE                 \n

 /*TITLE_CHAR              [{ASCII_LOWER_CASE}{ASCII_UPPER_CASE}{ASCII_NUMBER}{ASCII_SYMBOL_NOTOKEN}{ASCII_WHITESPACE}] */
TITLE                   [a-zA-Z0-9_\-="+\\/?.,\t ]

TAGS_OPEN               \[
TAGS_CLOSE              \]
 /*TAG_CHAR                [{ASCII_LOWER_CASE}{ASCII_UPPER_CASE}{ASCII_NUMBER}{ASCII_SYMBOL_NOTOKEN}]*/
TAG                     [a-zA-Z0-9\-_="'!+\\/?.,]+

 /*BODY_TEXT_CHAR          [{ASCII_LOWER_CASE}{ASCII_UPPER_CASE}{ASCII_NUMBER}{ASCII_SYMBOL_NOTOKEN}{ASCII_WHITESPACE}]*/
BODY_TEXT               [a-zA-Z0-9\-_="'!+\\/?.,\t ]+

FORMATTING_OPEN         \^{3}
FORMATTING_CLOSE        \^{3}
BODY_TEXT_FORMATTED     [a-zA-Z0-9\-_="'!+\\/?.,\t ]+

LINK_OPEN               \[{2}
LINK_CLOSE              \]{2}
LINK_SEPARATOR          \|

 /*LINK_TEXT_CHAR          [{ASCII_LOWER_CASE}{ASCII_UPPER_CASE}{ASCII_NUMBER}{ASCII_SYMBOL_NOTOKEN}{ASCII_WHITESPACE}]*/
LINK_TEXT               [a-zA-Z0-9\-_="'!+\\/?.,\t ]+

MACRO_OPEN              <{2}
MACRO_CLOSE             >{2}
 /*MACRO_TEXT_CHAR         [{ASCII_LOWER_CASE}{ASCII_UPPER_CASE}{ASCII_NUMBER}{ASCII_SYMBOL_NOTOKEN}{ASCII_WHITESPACE}]*/
MACRO_TEXT              [a-zA-Z0-9\-_="'!+\\/?.,\t ]+

 /*Parser Conditions */
    /*Naming convention in this file: CamelCase*/
    /*Conditions are exclusive*/
%x HeaderTitle
%x HeaderTags
%x HeaderTagsClose
%x Body
%x BodyFormattedText
%x BodyLink
%x BodyMacro

%%

 /*___NEW CONDITION___ Start of a passage*/
    /*From: Start of Lexing, Body*/
    /*To:   HeaderTitle    */

    /* :: */
<INITIAL>^{PASSAGE_START} {
                                //enter condition HeaderTitle
                                BEGIN(HeaderTitle);
                                lex_log("enter condition HeaderTitle");
                                //return the PASSAGE_START token
                                lex_log("return the PASSAGE_START token");
                                return BisonParser::token::PASSAGE_START;
                                }

    /* unexpected Token(s) */
<INITIAL>.                      {
                                //TODO: lexer error in INITIAL
                                lex_log("lexer error in INITIAL");
                                }

 /* ___NEW CONDITION___ HeaderTitle*/
    /*From: Start of Lexing, Body*/
    /*To:   HeaderTags, Body    */

    /* Title of the Passage */
<HeaderTitle>{TITLE}+            {
                                //stay in condition HeaderTitle, look for next token
                                lex_log("stay in condition HeaderTitle, look for next token");
                                //return the TITLE Token
                                lex_log("return the TITLE Token");
                                lex_log("\t matched:");
                                lex_log(YYText());
                                SAVE_TOKEN;
                                return BisonParser::token::TITLE;
                                }

    /* square bracket opened, indicating Tags segment */
<HeaderTitle>{TAGS_OPEN}          {
                                //enter condition HeaderTags
                                lex_log("enter condition HeaderTags");
                                BEGIN(HeaderTags);
                                //return the TAGS_OPEN token
                                lex_log("return the TAGS_OPEN token");
                                return BisonParser::token::TAGS_OPEN;
                                }

    /* Everything except :: */
<HeaderTitle>{NEWLINE}          {
                                //enter condition Body
                                lex_log("enter condition Body");
                                BEGIN(Body);
                                //return the NEWLINE token
                                lex_log("return the NEWLINE token");
                                return BisonParser::token::NEWLINE;
                                }

    /* unexpected Token(s) */
<HeaderTitle>.                  {
                                //TODO: lexer error in HeaderTitle
                                lex_log("lexer error in HeaderTitle");
                                lex_log("\t matched:");
                                lex_log(YYText());
                                }

 /* ___NEW CONDITION___ HeaderTags*/
    /*From: HeaderTitle*/
    /*To:   HeaderTagsClose    */

    /* Title of the Passage */
<HeaderTags>{TAG}                 {
                                //stay in condition HeaderTags, look for next token
                                lex_log("stay in condition HeaderTags, look for next token");
                                //return the TAG Token
                                lex_log("return the TAG Token");
                                lex_log("\t matched:");
                                lex_log(YYText());
                                SAVE_TOKEN;
                                return BisonParser::token::TAG;
                                }

    /* square bracket opened, indicating Tags segment */
<HeaderTags>{TAGS_CLOSE}        { //Problem, can't return 2 Tokens; so enter HeaderTagsClose
                                //enter condition HeaderTagsClose
                                lex_log("enter condition HeaderTagsClose");
                                BEGIN(HeaderTagsClose);
                                //return the TAGS_CLOSE token
                                lex_log("return the TAGS_CLOSE token");
                                return BisonParser::token::TAGS_CLOSE;
                                }

    /* unexpected Token(s) */
<HeaderTags>.                   {
                                //TODO: lexer error in HeaderTags
                                lex_log("lexer error in HeaderTags");
                                }

 /* ___NEW CONDITION___ HeaderTagsClose*/
    /*From: HeaderTags*/
    /*To:   Body    */

    /* End of HeaderTags */
<HeaderTagsClose>{NEWLINE}      {
                                //enter condition Body
                                lex_log("enter condition Body");
                                BEGIN(Body);
                                //return the NEWLINE token
                                lex_log("return the NEWLINE token");
                                return BisonParser::token::NEWLINE;
                                }

    /* unexpected Token(s) */
<HeaderTagsClose>.              {
                                //TODO: lexer error in HeaderTagsClose
                                lex_log("lexer error in HeaderTagsClose");
                                lex_log("\t matched:");
                                lex_log(YYText());
                                }

 /* ___NEW CONDITION___ Body*/
    /*From: HeaderTitle, HeaderTagsClose*/
    /*To:   TODO: Body: NEWLINE PASSAGE_START    */

    /* Passage text */
<Body>{BODY_TEXT}               {
                                //stay in condition Body, look for next token
                                lex_log("stay in condition Body, look for next token");
                                //return the TEXT Token
                                lex_log("return the TEXT Token");
                                lex_log("\t matched:");
                                lex_log(YYText());
                                SAVE_TOKEN;
                                return BisonParser::token::TEXT_TOKEN;
                                }

    /* :: */
<Body>^{PASSAGE_START} {
                                //enter condition HeaderTitle
                                lex_log("enter condition HeaderTitle");
                                BEGIN(HeaderTitle);
                                //return the PASSAGE_START token
                                lex_log("return the PASSAGE_START token");
                                return BisonParser::token::PASSAGE_START;
                                }

    /* formatting encountered */
<Body>{FORMATTING_OPEN}         {
                                //enter condition BodyFormattedText
                                lex_log("enter condition BodyFormattedText");
                                BEGIN(BodyFormattedText);
                                //return the FORMATTING_OPEN token
                                lex_log("return the FORMATTING_OPEN token");
                                return BisonParser::token::FORMATTING_OPEN;
                                }

    /* Link encountered */
<Body>{LINK_OPEN}               {
                                //enter condition BodyLink
                                lex_log("enter condition BodyLink");
                                BEGIN(BodyLink);
                                //return the LINK_OPEN token
                                lex_log("return the LINK_OPEN token");
                                return BisonParser::token::LINK_OPEN;
                                }

    /* Macro encountered */
<Body>{MACRO_OPEN}              {
                                //enter condition BodyMacro
                                lex_log("enter condition BodyMacro");
                                BEGIN(BodyMacro);
                                //return the MACRO_OPEN token
                                lex_log("return the MACRO_OPEN token");
                                return BisonParser::token::MACRO_OPEN;
                                }

    /* New line means we have to check the next few tokens TODO New Passage after Passage */
<Body>{NEWLINE}                 {
                                //TODO New Passage after Passage
                                lex_log("New Passage after Passage");
                                }

    /* unexpected Token(s) */
<Body>.                         {
                                //TODO: lexer error in Body
                                lex_log("lexer error in Body");
                                lex_log("\t matched:");
                                lex_log(YYText());
                                }

 /* ___NEW CONDITION___ BodyFormattedText*/
    /*From: Body */
    /*To:   Body */

    /* formatted text */
<BodyFormattedText>{BODY_TEXT_FORMATTED}  {
                                //stay in condition BodyFormattedText, look for next token
                                lex_log("stay in condition BodyFormattedText, look for next token");
                                //return the TEXT Token
                                lex_log("return the TEXT Token");
                                lex_log("\t matched:");
                                lex_log(YYText());
                                SAVE_TOKEN;
                                return BisonParser::token::TEXT_TOKEN;
                                }

    /* leave formatting */
<BodyFormattedText>{FORMATTING_CLOSE}         {
                                //enter condition Body
                                lex_log("enter condition Body");
                                BEGIN(Body);
                                //return the FORMATTING_CLOSE token
                                lex_log("return the FORMATTING_CLOSE token");
                                return BisonParser::token::FORMATTING_CLOSE;
                                }

    /* unexpected Token(s) */
<BodyFormattedText>.            {
                                //TODO: lexer error in BodyFormattedText
                                lex_log("lexer error in BodyFormattedText");
                                }

 /* ___NEW CONDITION___ BodyLink*/
    /*From: Body */
    /*To:   Body */

    /* link text */
<BodyLink>{LINK_TEXT}  {
                                //stay in condition BodyLink, look for next token
                                lex_log("stay in condition BodyLink, look for next token");
                                //return the TEXT Token
                                lex_log("return the TEXT Token");
                                lex_log("\t matched:");
                                lex_log(YYText());
                                SAVE_TOKEN;
                                return BisonParser::token::TEXT_TOKEN;
                                }

    /* separator */
<BodyLink>{LINK_SEPARATOR}      {
                                //stay in condition BodyLink, look for next token
                                lex_log("stay in condition BodyLink, look for next token");
                                //return the LINK_CLOSE token
                                lex_log("return the LINK_CLOSE token");
                                return BisonParser::token::LINK_SEPARATOR;
                                }

    /* leave the link */
<BodyLink>{LINK_CLOSE}         {
                                //enter condition Body
                                lex_log("enter condition Body");
                                BEGIN(Body);
                                //return the LINK_CLOSE token
                                lex_log("return the LINK_CLOSE token");
                                return BisonParser::token::LINK_CLOSE;
                                }


    /* unexpected Token(s) */
<BodyLink>.                         {
                                //TODO: lexer error in BodyLink
                                lex_log("lexer error in BodyLink");
                                }

 /* ___NEW CONDITION___ BodyMacro*/
    /*From: Body */
    /*To:   Body */

    /* macro text */
<BodyMacro>{MACRO_TEXT}         {
                                //stay in condition BodyMacro, look for next token
                                lex_log("stay in condition BodyMacro, look for next token");
                                //return the TEXT Token
                                lex_log("return the TEXT Token");
                                lex_log("\t matched:");
                                SAVE_TOKEN;
                                return BisonParser::token::TEXT_TOKEN;
                                }

    /* leave the macro */
<BodyMacro>{MACRO_CLOSE}         {
                                //enter condition Body
                                lex_log("enter condition Body");
                                BEGIN(Body);
                                //return the MACRO_CLOSE token
                                lex_log("return the MACRO_CLOSE token");
                                return BisonParser::token::MACRO_CLOSE;
                                }

    /* unexpected Token(s) */
<BodyMacro>.                         {
                                //TODO: lexer error in BodyMacro
                                lex_log("lexer error in BodyMacro");
                                }

%%