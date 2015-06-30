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
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

//TweeScanner.h for the Tokens
#include "TweeScanner.h"

#include "include/TweeFile.h"

// TODO: check memory stuff for SAVE_TOKEN, does the parser clear that?
#define SAVE_STRING yylval->string = new std::string(YYText(), YYLeng())
#define SAVE_INT yylval->integer = std::stoi(std::string(YYText(), YYLeng()))

// TODO: decide if this is needed, look at that tutorial again: #define TOKEN(t) (yylval.token = t)

/*logging templates: use these to get logging consistent
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "condition" << " matched Token" << "tokenName";
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "condition" << " matched Token " << "tokenName" << " with value " << YYText();
                                LOG_ERROR << "ERROR in Lexer: line: "<< lineno() <<" Condition: " << "condition" << " when matching Token " << "tokenName";
                                LOG_ERROR << "ERROR in Lexer: line: "<< lineno() <<" Condition: " << "condition" << " when matching Token " << "tokenName" << " with value " << YYText();
*/

%}

 /*misc options*/
%option noyywrap

 /* debug options: http://flex.sourceforge.net/manual/Debugging-Options.html#Debugging-Options */
%option debug warn verbose
%option yylineno

 /* output relevant stuff*/
%option yyclass="Twee::TweeScanner" c++
%option outfile="GeneratedTweeLexer.cpp"

 /*Definitions */
PASSAGE_START           ::
BODY_PASSAGE_START      ::
NEWLINE                 \n
WHITESPACE              [ \t]

 /*TITLE_CHAR              [{ASCII_LOWER_CASE}{ASCII_UPPER_CASE}{ASCII_NUMBER}{ASCII_SYMBOL_NOTOKEN}{ASCII_WHITESPACE}] */
TITLE                   [a-zA-Z0-9_\-="+\\/?.,\t ]

TAGS_OPEN               \[
TAGS_CLOSE              \]
 /*TAG_CHAR                [{ASCII_LOWER_CASE}{ASCII_UPPER_CASE}{ASCII_NUMBER}{ASCII_SYMBOL_NOTOKEN}]*/
TAG                     [a-zA-Z0-9\-_="'!+\\/?.,]+

MATCH_REST                  .*
BODY_SYMBOL                 [\{\[<\|\/_=~\^@\{\}:]
QUOTATION_MARK              \"

FORMATTING_ITALICS          \/{2}
FORMATTING_BOLDFACE         \"{2}
FORMATTING_UNDERLINE        _{2}
FORMATTING_STRIKETHROUGH    ={2}
FORMATTING_SUBSCRIPT        ~{2}
FORMATTING_SUPERSCRIPT      \^{2}
FORMATTING_ERROR_STYLING    @{2}

FORMATTING_MONOSPACE_OPEN   \{{3}
FORMATTING_MONOSPACE_CLOSE  \}{3}
FORMATTING_COMMENT_OPEN     \/%
FORMATTING_COMMENT_CLOSE    %\/

LINK_OPEN               \[{2}
LINK_CLOSE              \]{2}
LINK_SMALL_CLOSE        \]
LINK_EXPRESSION_OPEN    \[
LINK_SEPARATOR          \|

MACRO_OPEN              <{2}
MACRO_CLOSE             >{2}

    /*Macro Tokens*/
MACRO_IF                if
MACRO_ELSE              else


MACRO_ENDIF             endif

MACRO_PRINT             print
MACRO_DISPLAY           display

    /*Expression String and Integer Token*/
EXPR_STR_LIMITER        \"
EXPR_STR                [a-zA-Z]+
EXPR_INT                [0-9]+

    /* initial $ sign -> letter (uppercase or lowercase) or an underscore -> any combination of letters, numbers, or underscores */
EXPR_VAR                $[a-zA-z_][a-zA-Z0-9_]*

EXPR_RANDOM            random

EXPR_VISITED           visited[]*\([]*\)
EXPR_PREVIOUS          previous[]*\([]*\)
EXPR_TURNS             turns[]*\([]*\)

EXPR_OPEN              \(
EXPR_CLOSE             \)

    /*Arithmetic Tokens*/
EXPR_ADD                \+
EXPR_MUL                \*
EXPR_SUB                -
EXPR_DIV                \/
EXPR_MOD                %

    /*Logical Tokens*/
EXPR_GT                 >|gt
EXPR_GTE                >=|gte
EXPR_LT                 <|lt
EXPR_LTE                <=|lte
EXPR_NEQ                neq
EXPR_IS                 is|eq
EXPR_AND                and
EXPR_OR                 or
EXPR_NOT                not

    /*Assignment Token*/
EXPR_ASS                to|=

 /*Parser Conditions */
    /*Naming convention in this file: CamelCase*/
    /*Conditions are exclusive*/
%x HeaderTitle
%x HeaderTags
%x HeaderTagsClose
%x Body
%x FormattingErrorInlineStyling
%x FormattingComment
%x BodyLink
%x LinkExpression
%x BodyMacro

%%

 /*___NEW CONDITION___ Start of a passage*/
    /*From: Start of Lexing, Body*/
    /*To:   HeaderTitle    */

    /* :: */
<INITIAL>^{PASSAGE_START}       {
                                //enter condition HeaderTitle
                                BEGIN(HeaderTitle);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "INITIAL" << " matched Token " << " PASSAGE_START";
                                return BisonParser::token::PASSAGE_START;
                                }

    /* unexpected Token(s) */
<INITIAL>.                      {
                                //TODO: lexer error in INITIAL
                                LOG_ERROR << "lexer error in condition INITIAL";
                                LOG_ERROR << "\t matched:";
                                LOG_ERROR << YYText();
                                }

 /* ___NEW CONDITION___ HeaderTitle*/
    /*From: Start of Lexing, Body*/
    /*To:   HeaderTags, Body    */

    /* Title of the Passage */
<HeaderTitle>{TITLE}+            {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << " HeaderTitle" << " matched Token" << " TITLE";
                                SAVE_STRING;
                                return BisonParser::token::TITLE;
                                }

    /* square bracket opened, indicating Tags segment */
<HeaderTitle>{TAGS_OPEN}          {
                                BEGIN(HeaderTags);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << " HeaderTitle" << " matched Token" << " TAGS_OPEN";
                                return BisonParser::token::TAGS_OPEN;
                                }

    /* Everything except :: */
<HeaderTitle>{NEWLINE}          {
                                BEGIN(Body);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "HeaderTitle" << " matched Token" << "NEWLINE";
                                return BisonParser::token::NEWLINE;
                                }

 /* ___NEW CONDITION___ HeaderTags*/
    /*From: HeaderTitle*/
    /*To:   HeaderTagsClose    */

    /* Title of the Passage */
<HeaderTags>{TAG}                 {
                                //stay in condition HeaderTags, look for next token
                                LOG_DEBUG << "stay in condition HeaderTags, look for next token";
                                //return the TAG Token
                                LOG_DEBUG << "return the TAG Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_STRING;
                                return BisonParser::token::TAG;
                                }

    /* square bracket opened, indicating Tags segment */
<HeaderTags>{TAGS_CLOSE}        { //Problem, can't return 2 Tokens; so enter HeaderTagsClose
                                //enter condition HeaderTagsClose
                                LOG_DEBUG << "enter condition HeaderTagsClose";
                                BEGIN(HeaderTagsClose);
                                //return the TAGS_CLOSE token
                                LOG_DEBUG << "return the TAGS_CLOSE token";
                                return BisonParser::token::TAGS_CLOSE;
                                }

    /* unexpected Token(s) */
<HeaderTags>.                   {
                                //TODO: lexer error in HeaderTags
                                LOG_DEBUG << "lexer error in condition HeaderTags";
                                LOG_ERROR << "\t matched:";
                                LOG_ERROR << YYText();
                                }

 /* ___NEW CONDITION___ HeaderTagsClose*/
    /*From: HeaderTags*/
    /*To:   Body    */

    /* End of HeaderTags */
<HeaderTagsClose>{NEWLINE}      {
                                BEGIN(Body);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "HeaderTagsClose" << " matched Token " << "NEWLINE";
                                return BisonParser::token::NEWLINE;
                                }

    /* unexpected Token(s) */
<HeaderTagsClose>.              {
                                LOG_ERROR << "ERROR in Lexer: line: "<< lineno() <<" Condition: " << "condition" << " when matching Token " << "tokenName" << " with value " << YYText();
                                }

 /* ___NEW CONDITION___ Body*/
    /*From: HeaderTitle, HeaderTagsClose*/
    /*To:   TODO: Body: NEWLINE PASSAGE_START    */



    /* :: */
<Body>^{PASSAGE_START}{MATCH_REST} {
                                yyless(2);
                                BEGIN(HeaderTitle);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "PASSAGE_START";
                                return BisonParser::token::PASSAGE_START;
                                }

    /* Link encountered */
<Body>{LINK_OPEN}{MATCH_REST}               {
                                yyless(2);
                                BEGIN(BodyLink);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "LINK_OPEN";
                                return BisonParser::token::LINK_OPEN;
                                }

    /* Macro encountered */
<Body>{MACRO_OPEN}{MATCH_REST}              {
                                yyless(2);
                                BEGIN(BodyMacro);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "MACRO_OPEN";
                                return BisonParser::token::MACRO_OPEN;
                                }

<Body>{FORMATTING_ITALICS}{MATCH_REST}      {
                                yyless(2); //TODO: enable correct matching, BODY_TEXT
                                SAVE_STRING;
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "FORMATTING" << " with value " << YYText();
                                return BisonParser::token::FORMATTING;
                                }

<Body>{FORMATTING_BOLDFACE}{MATCH_REST}      {
                                yyless(2); //TODO: enable correct matching, BODY_TEXT
                                SAVE_STRING;
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "FORMATTING" << " with value " << YYText();
                                return BisonParser::token::FORMATTING;
                                }

<Body>{FORMATTING_UNDERLINE}{MATCH_REST}      {
                                yyless(2); //TODO: enable correct matching, BODY_TEXT
                                SAVE_STRING;
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "FORMATTING" << " with value " << YYText();
                                return BisonParser::token::FORMATTING;
                                }

<Body>{FORMATTING_STRIKETHROUGH}{MATCH_REST}      {
                                yyless(2); //TODO: enable correct matching, BODY_TEXT
                                SAVE_STRING;
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "FORMATTING" << " with value " << YYText();
                                return BisonParser::token::FORMATTING;
                                }

<Body>{FORMATTING_SUBSCRIPT}{MATCH_REST}      {
                                yyless(2);
                                SAVE_STRING;
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "FORMATTING" << " with value " << YYText();
                                 return BisonParser::token::FORMATTING;
                                 }

<Body>{FORMATTING_SUPERSCRIPT}      {
                                yyless(2);
                                SAVE_STRING;
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "FORMATTING" << " with value " << YYText();
                                return BisonParser::token::FORMATTING;
                                }

<Body>{FORMATTING_MONOSPACE_OPEN}{MATCH_REST}      {
                                yyless(3);
                                //TODO: new condition for monospace open&close lexing
                                SAVE_STRING;
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "FORMATTING" << " with value " << YYText();
                                return BisonParser::token::FORMATTING_COMMENT_OPEN;
                                }

<Body>{FORMATTING_MONOSPACE_CLOSE}{MATCH_REST}      {
                                yyless(3); //TODO: new condition for monospace open&close lexing: delete this
                                SAVE_STRING;
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "FORMATTING" << " with value " << YYText();
                                return BisonParser::token::FORMATTING_COMMENT_CLOSE;
                                }

<Body>{FORMATTING_COMMENT_OPEN}{MATCH_REST} {
                                yyless(2);
                                BEGIN(FormattingComment);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "FORMATTING" << " with value " << YYText();
                                return BisonParser::token::FORMATTING_COMMENT_OPEN;
                                }

<Body>{FORMATTING_ERROR_STYLING}{MATCH_REST} {
                                yyless(2);
                                BEGIN(FormattingErrorInlineStyling);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "FORMATTING_ERROR_STYLING" << " with value " << YYText();
                                return BisonParser::token::FORMATTING_ERROR_STYLING;
                                }

    /* Passage text */
<Body>{NEWLINE}                 {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "NEWLINE";
                                return BisonParser::token::NEWLINE;
                                }

    /* separate Symbols */
<Body>{BODY_SYMBOL}             {
                                SAVE_STRING;
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "TEXT_TOKEN" << " with value " << YYText();
                                return BisonParser::token::TEXT_TOKEN;
                                }
    /* escaped quotation marks*/
 /*<Body>{QUOTATION_MARK}          {
                                SAVE_STRING;
                                LOG_DEBUG << "Lexers: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "TEXT_TOKEN" << " with value " << YYText();
                                return BisonParser::token::TEXT_TOKEN;
                                }*/

    /* Passage text */
<Body>[^\{\[<\|\n\/\"_=~\^@\{\}:\n]+        {
                                SAVE_STRING;
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "Body" << " matched Token " << "TEXT_TOKEN" << " with value " << YYText();
                                return BisonParser::token::TEXT_TOKEN;
                                }



 /* ___NEW CONDITION___ FormattingErrorInlineStyling*/
    /*From: Body */
    /*To:   Body */
    /*TODO: implement error/inline text lexing*/

    /* text */

<FormattingErrorInlineStyling>{FORMATTING_ERROR_STYLING} {  //TODO: enable correct matching, BODY_TEXT
                                BEGIN(Body);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "FormattingErrorInlineStyling" << " matched Token " << "FORMATTING_ERROR_STYLING" << " with value " << YYText();
                                return BisonParser::token::FORMATTING_ERROR_STYLING;
                                }

    /* unexpected Token(s) */
<FormattingErrorInlineStyling>. {
                                LOG_ERROR << "ERROR in Lexer: line: "<< lineno() <<" Condition: " << "FormattingErrorInlineStyling" << " when matching Token " << "." << " with value " << YYText();
                                }

 /* ___NEW CONDITION___ FormattingComment*/
    /*From: Body */
    /*To:   Body */
    /* maybe comments should be lexed out? */


<FormattingComment>{FORMATTING_COMMENT_CLOSE}{MATCH_REST} {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "FormattingComment" << " matched Token " << "FORMATTING_COMMENT_CLOSE" << " with value " << YYText();
                                BEGIN(Body);
                                return BisonParser::token::FORMATTING_COMMENT_CLOSE;
                                }

    /* unexpected Token(s) */
<FormattingComment>.            {
                                LOG_ERROR << "ERROR in Lexer: line: "<< lineno() <<" Condition: " << "FormattingComment" << " when matching Token " << "." << " with value " << YYText();
                                }

 /* ___NEW CONDITION___ BodyLink*/
    /*From: Body */
    /*To:   Body */

    /* link text */
<BodyLink>[^\|\]\[]+              {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "BodyLink" << " matched Token " << "TEXT_TOKEN" << " with value " << YYText();
                                SAVE_STRING;
                                return BisonParser::token::TEXT_TOKEN;
                                }

    /* separator */
<BodyLink>{LINK_SEPARATOR}      {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "BodyLink" << " matched Token " << "LINK_SEPARATOR" << " with value " << YYText();
                                return BisonParser::token::LINK_SEPARATOR;
                                }

    /* leave the link */
<BodyLink>{LINK_SMALL_CLOSE}         {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "BodyLink" << " matched Token " << "LINK_SMALL_CLOSE" << " with value " << YYText();
                                return BisonParser::token::LINK_SMALL_CLOSE;
                                }

    /* leave the link */
<BodyLink>{LINK_EXPRESSION_OPEN}         {
                                BEGIN(LinkExpression);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "BodyLink" << " matched Token " << "LINK_EXPRESSION_OPEN" << " with value " << YYText();
                                return BisonParser::token::LINK_EXPRESSION_OPEN;
                                }

    /* leave the link */
<BodyLink>{LINK_CLOSE}         {
                                BEGIN(Body);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "BodyLink" << " matched Token " << "LINK_CLOSE" << " with value " << YYText();
                                return BisonParser::token::LINK_CLOSE;
                                }


    /* unexpected Token(s) */
<BodyLink>.                     {
                                LOG_ERROR << "ERROR in Lexer: line: "<< lineno() <<" Condition: " << "BodyLink" << " when matching Token " << "." << " with value " << YYText();
                                }

 /* ___NEW CONDITION___ LinkExpression*/
    /*From: BodyLink */
    /*To:   Body */

    /* expression variable */
<LinkExpression>{EXPR_VAR}           {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token EXPR_VAR" << " with value " << YYText();
                                SAVE_STRING;
                                return BisonParser::token::EXPR_VAR;
                                }


    /* expression functions */
<LinkExpression>{EXPR_RANDOM}		{
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_RANDOM" << " with value " << YYText();
                                return BisonParser::token::EXPR_RANDOM;
                                }

<LinkExpression>{EXPR_VISITED}		{
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_VISITED" << " with value " << YYText();
                                return BisonParser::token::EXPR_VISITED;
                                }

<LinkExpression>{EXPR_PREVIOUS}		{
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_PREVIOUS" << " with value " << YYText();
                                return BisonParser::token::EXPR_PREVIOUS;
                                }

<LinkExpression>{EXPR_TURNS}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_TURNS" << " with value " << YYText();
                                return BisonParser::token::EXPR_TURNS;
                                }

<LinkExpression>{EXPR_OPEN}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_OPEN" << " with value " << YYText();
                                return BisonParser::token::EXPR_OPEN;
                                }

<LinkExpression>{EXPR_CLOSE}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_CLOSE" << " with value " << YYText();
                                return BisonParser::token::EXPR_CLOSE;
                                }

    /*Arithmetic Tokens*/
<LinkExpression>{EXPR_ADD}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_ADD" << " with value " << YYText();
                                return BisonParser::token::EXPR_ADD;
                                }
<LinkExpression>{EXPR_MUL}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_MUL" << " with value " << YYText();
                                return BisonParser::token::EXPR_MUL;
                                }
<LinkExpression>{EXPR_SUB}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_SUB" << " with value " << YYText();
                                return BisonParser::token::EXPR_SUB;
                                }
<LinkExpression>{EXPR_DIV}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_DIV" << " with value " << YYText();
                                return BisonParser::token::EXPR_DIV;
                                }
<LinkExpression>{EXPR_MOD}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_MOD" << " with value " << YYText();
                                return BisonParser::token::EXPR_MOD;
                                }

    /*Logical Tokens*/
<LinkExpression>{EXPR_GT}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_GT" << " with value " << YYText();
                                return BisonParser::token::EXPR_GT;
                                }
<LinkExpression>{EXPR_GTE}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_GTE" << " with value " << YYText();
                                return BisonParser::token::EXPR_GTE;
                                }
<LinkExpression>{EXPR_LT}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_LT" << " with value " << YYText();
                                return BisonParser::token::EXPR_LT;
                                }
<LinkExpression>{EXPR_LTE}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_LTE" << " with value " << YYText();
                                return BisonParser::token::EXPR_LTE;
                                }
<LinkExpression>{EXPR_NEQ}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_NEQ" << " with value " << YYText();
                                return BisonParser::token::EXPR_NEQ;
                                }
<LinkExpression>{EXPR_IS}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_IS" << " with value " << YYText();
                                return BisonParser::token::EXPR_IS;
                                }
<LinkExpression>{EXPR_AND}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_AND" << " with value " << YYText();
                                return BisonParser::token::EXPR_AND;
                                }
<LinkExpression>{EXPR_OR}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_OR" << " with value " << YYText();
                                return BisonParser::token::EXPR_OR;
                                }
<LinkExpression>{EXPR_NOT}		{
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_NOT" << " with value " << YYText();
                                return BisonParser::token::EXPR_NOT;
                                }

<LinkExpression>{EXPR_ASS}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: LinkExpression matched Token "<<"EXPR_ASS" << " with value " << YYText();
                                return BisonParser::token::EXPR_ASS;
                                }

    /* parse constant integer */
<LinkExpression>{EXPR_INT}        {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "LinkExpression" << " matched Token " << "EXPR_INT" << " with value " << YYText();
                                SAVE_INT;
                                return BisonParser::token::EXPR_INT;
                                }


    /* leave the link expression */
<LinkExpression>{LINK_CLOSE}    {
                                BEGIN(Body);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "LinkExpression" << " matched Token " << "LINK_CLOSE" << " with value " << YYText();
                                return BisonParser::token::LINK_CLOSE;
                                }

    /* unexpected Token(s) */
<LinkExpression>.                     {
                                LOG_ERROR << "ERROR in Lexer: line: "<< lineno() <<" Condition: " << "BodyLink" << " when matching Token " << "." << " with value " << YYText();
                                }

 /* ___NEW CONDITION___ BodyMacro*/
    /*From: Body */
    /*To:   Body */

    /* macro if */
<BodyMacro>{MACRO_IF}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token MACRO_IF" << " with value " << YYText();
                                return BisonParser::token::MACRO_IF;
                                }

<BodyMacro>{MACRO_ELSE}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"MACRO_ELSE" << " with value " << YYText();
                                return BisonParser::token::MACRO_ELSE;
                                }

<BodyMacro>{MACRO_IF}{WHITESPACE}{MACRO_ELSE} 		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"MACRO_ELSE_IF" << " with value " << YYText();
                                return BisonParser::token::MACRO_ELSE_IF;
                                }

<BodyMacro>{MACRO_ENDIF}		{
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"MACRO_ENDIF" << " with value " << YYText();
                                return BisonParser::token::MACRO_ENDIF;
                                }

    /* macro print */
<BodyMacro>{MACRO_PRINT}        {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"MACRO_PRINT" << " with value " << YYText();
                                return BisonParser::token::MACRO_PRINT;
                                }
    /* macro display */
<BodyMacro>{MACRO_DISPLAY}      {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"MACRO_DISPLAY" << " with value " << YYText();
                                return BisonParser::token::MACRO_DISPLAY;
                                }

    /* expression variable */
<BodyMacro>{EXPR_VAR}           {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token EXPR_VAR" << " with value " << YYText();
                                SAVE_STRING;
                                return BisonParser::token::EXPR_VAR;
                                }


    /* expression functions */
<BodyMacro>{EXPR_RANDOM}		{
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_RANDOM" << " with value " << YYText();
                                return BisonParser::token::EXPR_RANDOM;
                                }

<BodyMacro>{EXPR_VISITED}		{
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_VISITED" << " with value " << YYText();
                                return BisonParser::token::EXPR_VISITED;
                                }

<BodyMacro>{EXPR_PREVIOUS}		{
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_PREVIOUS" << " with value " << YYText();
                                return BisonParser::token::EXPR_PREVIOUS;
                                }

<BodyMacro>{EXPR_TURNS}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_TURNS" << " with value " << YYText();
                                return BisonParser::token::EXPR_TURNS;
                                }

<BodyMacro>{EXPR_OPEN}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_OPEN" << " with value " << YYText();
                                return BisonParser::token::EXPR_OPEN;
                                }

<BodyMacro>{EXPR_CLOSE}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_CLOSE" << " with value " << YYText();
                                return BisonParser::token::EXPR_CLOSE;
                                }

    /*Arithmetic Tokens*/
<BodyMacro>{EXPR_ADD}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_ADD" << " with value " << YYText();
                                return BisonParser::token::EXPR_ADD;
                                }
<BodyMacro>{EXPR_MUL}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_MUL" << " with value " << YYText();
                                return BisonParser::token::EXPR_MUL;
                                }
<BodyMacro>{EXPR_SUB}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_SUB" << " with value " << YYText();
                                return BisonParser::token::EXPR_SUB;
                                }
<BodyMacro>{EXPR_DIV}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_DIV" << " with value " << YYText();
                                return BisonParser::token::EXPR_DIV;
                                }
<BodyMacro>{EXPR_MOD}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_MOD" << " with value " << YYText();
                                return BisonParser::token::EXPR_MOD;
                                }

    /*Logical Tokens*/
<BodyMacro>{EXPR_GT}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_GT" << " with value " << YYText();
                                return BisonParser::token::EXPR_GT;
                                }
<BodyMacro>{EXPR_GTE}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_GTE" << " with value " << YYText();
                                return BisonParser::token::EXPR_GTE;
                                }
<BodyMacro>{EXPR_LT}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_LT" << " with value " << YYText();
                                return BisonParser::token::EXPR_LT;
                                }
<BodyMacro>{EXPR_LTE}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_LTE" << " with value " << YYText();
                                return BisonParser::token::EXPR_LTE;
                                }
<BodyMacro>{EXPR_NEQ}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_NEQ" << " with value " << YYText();
                                return BisonParser::token::EXPR_NEQ;
                                }
<BodyMacro>{EXPR_IS}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_IS" << " with value " << YYText();
                                return BisonParser::token::EXPR_IS;
                                }
<BodyMacro>{EXPR_AND}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_AND" << " with value " << YYText();
                                return BisonParser::token::EXPR_AND;
                                }
<BodyMacro>{EXPR_OR}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_OR" << " with value " << YYText();
                                return BisonParser::token::EXPR_OR;
                                }
<BodyMacro>{EXPR_NOT}		{
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_NOT" << " with value " << YYText();
                                return BisonParser::token::EXPR_NOT;
                                }

<BodyMacro>{EXPR_ASS}		    {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: BodyMacro matched Token "<<"EXPR_ASS" << " with value " << YYText();
                                return BisonParser::token::EXPR_ASS;
                                }

    /* parse constant integer */
<BodyMacro>{EXPR_INT}        {
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "BodyMacro" << " matched Token " << "EXPR_INT" << " with value " << YYText();
                                SAVE_INT;
                                return BisonParser::token::EXPR_INT;
                                }


    /* leave the macro */
<BodyMacro>{MACRO_CLOSE}        {
                                BEGIN(Body);
                                LOG_DEBUG << "Lexer: line: "<< lineno() <<" Condition: " << "BodyMacro" << " matched Token " << "MACRO_CLOSE" << " with value " << YYText();
                                return BisonParser::token::MACRO_CLOSE;
                                }



    /* unexpected Token(s) */
<BodyMacro>.                    {
                                LOG_ERROR << "ERROR in Lexer: line: "<< lineno() <<" Condition: " << "BodyMacro" << " when matching Token " << "." << " with value " << YYText();
                                }

%%