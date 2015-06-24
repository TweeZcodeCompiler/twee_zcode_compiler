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
#define SAVE_TOKEN yylval->string = new std::string(YYText(), YYLeng())
// TODO: decide if this is needed, look at that tutorial again: #define TOKEN(t) (yylval.token = t)

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
BODY_TEXT               [a-zA-Z0-9\-'!+\?.,\t ]+
    /*these chars are used by FORMATTING tokens:*/
    /* / " _ = ~ ^ { % */

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
LINK_SEPARATOR          \|

 /*LINK_TEXT_CHAR          [{ASCII_LOWER_CASE}{ASCII_UPPER_CASE}{ASCII_NUMBER}{ASCII_SYMBOL_NOTOKEN}{ASCII_WHITESPACE}]*/
LINK_TEXT               [a-zA-Z0-9\-_="'!+\\/?.,\t ]+

MACRO_OPEN              <{2}
MACRO_CLOSE             >{2}
 /*MACRO_TEXT_CHAR         [{ASCII_LOWER_CASE}{ASCII_UPPER_CASE}{ASCII_NUMBER}{ASCII_SYMBOL_NOTOKEN}{ASCII_WHITESPACE}]*/
 /*MACRO_TEXT              [a-zA-Z0-9\-_="'!+\\/?.,\t ]+ */

    /*Macro Tokens*/
MACRO_IF                if
MACRO_ELSE              else
MACRO_ENDIF             endif

MACRO_PRINT             print
MACRO_DISPLAY           display

    /*Expression Tokens*/
EXPR_VAR               \$[_a-zA-Z][_a-zA-Z0-9]*
EXPR_RANDOM            random

EXPR_VISITED           visited
EXPR_PREVIOUS          previous
EXPR_TURNS             turns

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
EXPR_IS                 is
EXPR_EQ                 eq
EXPR_AND                and
EXPR_OR                 or

    /*Assignment Token*/
EXPR_TO                 to
EXPR_ASS                =


    /*Expression String and Integer Token*/
EXPR_STR_LIMITER        \"
EXPR_STR                [a-zA-Z]+
EXPR_INT                [0-9]+



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
%x BodyMacro

%%

 /*___NEW CONDITION___ Start of a passage*/
    /*From: Start of Lexing, Body*/
    /*To:   HeaderTitle    */

    /* :: */
<INITIAL>^{PASSAGE_START}       {
                                //enter condition HeaderTitle
                                BEGIN(HeaderTitle);
                                LOG_DEBUG << "enter condition HeaderTitle";
                                //return the PASSAGE_START token
                                LOG_DEBUG << "return the PASSAGE_START token";
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
                                //stay in condition HeaderTitle, look for next token
                                LOG_DEBUG << "stay in condition HeaderTitle, look for next token";
                                //return the TITLE Token
                                LOG_DEBUG << "return the TITLE Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::TITLE;
                                }

    /* square bracket opened, indicating Tags segment */
<HeaderTitle>{TAGS_OPEN}          {
                                //enter condition HeaderTags
                                LOG_DEBUG << "enter condition HeaderTags";
                                BEGIN(HeaderTags);
                                //return the TAGS_OPEN token
                                LOG_DEBUG << "return the TAGS_OPEN token";
                                return BisonParser::token::TAGS_OPEN;
                                }

    /* Everything except :: */
<HeaderTitle>{NEWLINE}          {
                                //enter condition Body
                                LOG_DEBUG << "enter condition Body";
                                BEGIN(Body);
                                //return the NEWLINE token
                                LOG_DEBUG << "return the NEWLINE token";
                                return BisonParser::token::NEWLINE;
                                }

    /* unexpected Token(s) */
<HeaderTitle>.                  {
                                //TODO: lexer error in HeaderTitle
                                LOG_ERROR << "lexer error in condition HeaderTitle";
                                LOG_ERROR << "\t matched:";
                                LOG_ERROR << YYText();
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
                                SAVE_TOKEN;
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
                                //enter condition Body
                                LOG_DEBUG << "enter condition Body";
                                BEGIN(Body);
                                //return the NEWLINE token
                                LOG_DEBUG << "return the NEWLINE token";
                                return BisonParser::token::NEWLINE;
                                }

    /* unexpected Token(s) */
<HeaderTagsClose>.              {
                                //TODO: lexer error in HeaderTagsClose
                                LOG_ERROR << "lexer error in condition HeaderTagsClose";
                                LOG_ERROR << "\t matched:";
                                LOG_ERROR << YYText();
                                }

 /* ___NEW CONDITION___ Body*/
    /*From: HeaderTitle, HeaderTagsClose*/
    /*To:   TODO: Body: NEWLINE PASSAGE_START    */

    /* Passage text */
<Body>{BODY_TEXT}               {
                                //stay in condition Body, look for next token
                                LOG_DEBUG << "stay in condition Body, look for next token";
                                //return the TEXT Token
                                LOG_DEBUG << "return the TEXT Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::TEXT_TOKEN;
                                }

    /* :: */
<Body>^{PASSAGE_START} {
                                //enter condition HeaderTitle
                                LOG_DEBUG << "enter condition HeaderTitle";
                                BEGIN(HeaderTitle);
                                //return the PASSAGE_START token
                                LOG_DEBUG << "return the PASSAGE_START token";
                                return BisonParser::token::PASSAGE_START;
                                }

    /* Link encountered */
<Body>{LINK_OPEN}               {
                                //enter condition BodyLink
                                LOG_DEBUG << "enter condition BodyLink";
                                BEGIN(BodyLink);
                                //return the LINK_OPEN token
                                LOG_DEBUG << "return the LINK_OPEN token";
                                return BisonParser::token::LINK_OPEN;
                                }

    /* Macro encountered */
<Body>{MACRO_OPEN}              {
                                LOG_DEBUG << "enter condition BodyMacro";
                                BEGIN(BodyMacro);
                                LOG_DEBUG << "return the MACRO_OPEN token";
                                return BisonParser::token::MACRO_OPEN;
                                }

<Body>{FORMATTING_ITALICS}      { //TODO: enable correct matching, BODY_TEXT
                                LOG_DEBUG << "return the FORMATTING Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::FORMATTING;
                                }

<Body>{FORMATTING_BOLDFACE}      { //TODO: enable correct matching, BODY_TEXT
                                LOG_DEBUG << "return the FORMATTING Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::FORMATTING;
                                }

<Body>{FORMATTING_UNDERLINE}      { //TODO: enable correct matching, BODY_TEXT
                                LOG_DEBUG << "return the FORMATTING Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::FORMATTING;
                                }

<Body>{FORMATTING_STRIKETHROUGH}      { //TODO: enable correct matching, BODY_TEXT
                                LOG_DEBUG << "return the FORMATTING Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::FORMATTING;
                                }

<Body>{FORMATTING_SUBSCRIPT}      {
                                 LOG_DEBUG << "return the FORMATTING Token";
                                 LOG_DEBUG << "\t matched:";
                                 LOG_DEBUG << YYText();
                                 SAVE_TOKEN;
                                 return BisonParser::token::FORMATTING;
                                 }

<Body>{FORMATTING_SUPERSCRIPT}      {
                                LOG_DEBUG << "return the FORMATTING Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::FORMATTING;
                                }

<Body>{FORMATTING_MONOSPACE_OPEN}      {
                                //TODO: new condition for monospace open&close lexing
                                LOG_DEBUG << "return the FORMATTING_COMMENT_OPEN token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::FORMATTING_COMMENT_OPEN;
                                }

<Body>{FORMATTING_MONOSPACE_CLOSE}      { //TODO: new condition for monospace open&close lexing: delete this
                                LOG_DEBUG << "return the FORMATTING_COMMENT_CLOSE token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::FORMATTING_COMMENT_CLOSE;
                                }

<Body>{FORMATTING_COMMENT_OPEN} {
                                LOG_DEBUG << "enter condition FormattingComment";
                                BEGIN(FormattingComment);
                                LOG_DEBUG << "return the FORMATTING_COMMENT_OPEN token";
                                return BisonParser::token::FORMATTING_COMMENT_OPEN;
                                }

<Body>{FORMATTING_ERROR_STYLING} {
                                LOG_DEBUG << "enter condition FormattingErrorInlineStyling";
                                BEGIN(FormattingErrorInlineStyling);
                                return BisonParser::token::FORMATTING_ERROR_STYLING;
                                }

    /* unexpected Token(s) */
<Body>.                         {
                                //TODO: lexer error in Body
                                LOG_ERROR << "lexer error in condition Body";
                                LOG_ERROR << "\t matched:";
                                LOG_ERROR << YYText();
                                }

 /* ___NEW CONDITION___ FormattingErrorInlineStyling*/
    /*From: Body */
    /*To:   Body */
    /*TODO: implement error/inline text lexing*/

    /* text */
<FormattingErrorInlineStyling>{BODY_TEXT} {
                                LOG_DEBUG << "stay in condition FormattingErrorInlineStyling, look for next token";
                                //return the TEXT Token
                                LOG_DEBUG << "return the TEXT Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::TEXT_TOKEN;
                                }

<FormattingErrorInlineStyling>{FORMATTING_ERROR_STYLING} {  //TODO: enable correct matching, BODY_TEXT
                                LOG_DEBUG << "enter condition Body";
                                BEGIN(Body);
                                return BisonParser::token::FORMATTING_ERROR_STYLING;
                                }

    /* unexpected Token(s) */
<FormattingErrorInlineStyling>. {
                                //TODO: lexer error in FormattingErrorInlineStyling
                                LOG_ERROR << "lexer error in condition FormattingErrorInlineStyling";
                                LOG_ERROR << "\t matched:";
                                LOG_ERROR << YYText();
                                }

 /* ___NEW CONDITION___ FormattingComment*/
    /*From: Body */
    /*To:   Body */
    /* maybe comments should be lexed out? */

    /* text */
<FormattingComment>{BODY_TEXT} {
                                LOG_DEBUG << "stay in condition FormattingComment, look for next token";
                                //return the TEXT Token
                                LOG_DEBUG << "return the TEXT Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::TEXT_TOKEN;
                                }

<FormattingComment>{FORMATTING_COMMENT_CLOSE} {
                                LOG_DEBUG << "enter condition Body";
                                BEGIN(Body);
                                return BisonParser::token::FORMATTING_COMMENT_CLOSE;
                                }

    /* unexpected Token(s) */
<FormattingComment>.            {
                                //TODO: lexer error in FormattingComment
                                LOG_ERROR << "lexer error in condition FormattingComment";
                                LOG_ERROR << "\t matched:";
                                LOG_ERROR << YYText();
                                }

 /* ___NEW CONDITION___ BodyLink*/
    /*From: Body */
    /*To:   Body */

    /* link text */
<BodyLink>{LINK_TEXT}           {
                                //stay in condition BodyLink, look for next token
                                LOG_DEBUG << "stay in condition BodyLink, look for next token";
                                //return the TEXT Token
                                LOG_DEBUG << "return the TEXT Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::TEXT_TOKEN;
                                }

    /* separator */
<BodyLink>{LINK_SEPARATOR}      {
                                //stay in condition BodyLink, look for next token
                                LOG_DEBUG << "stay in condition BodyLink, look for next token";
                                //return the LINK_CLOSE token
                                LOG_DEBUG << "return the LINK_CLOSE token";
                                return BisonParser::token::LINK_SEPARATOR;
                                }

    /* leave the link */
<BodyLink>{LINK_CLOSE}         {
                                //enter condition Body
                                LOG_DEBUG << "enter condition Body";
                                BEGIN(Body);
                                //return the LINK_CLOSE token
                                LOG_DEBUG << "return the LINK_CLOSE token";
                                return BisonParser::token::LINK_CLOSE;
                                }


    /* unexpected Token(s) */
<BodyLink>.                         {
                                //TODO: lexer error in BodyLink
                                LOG_ERROR << "lexer error in condition BodyLink";
                                LOG_ERROR << "\t matched:";
                                LOG_ERROR << YYText();
                                }

 /* ___NEW CONDITION___ BodyMacro*/
    /*From: Body */
    /*To:   Body */

    /* macro if */
<BodyMacro>{MACRO_IF}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the IF Token
                                LOG_DEBUG << "return the IF Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::IF_TOKEN;
                                }
<BodyMacro>{MACRO_ELSE}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the ELSE Token
                                LOG_DEBUG << "return the ELSE Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::ELSE_TOKEN;
                                }
<BodyMacro>{MACRO_ENDIF}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the ENDIF Token
                                LOG_DEBUG << "return the ENDIF Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::ENDIF_TOKEN;
                                }

    /* macro print */
<BodyMacro>{MACRO_PRINT}        {
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the PRINT Token
                                LOG_DEBUG << "return the PRINT Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::PRINT_TOKEN;
                                }
    /* macro display */
<BodyMacro>{MACRO_DISPLAY}         {
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the TEXT Token
                                LOG_DEBUG << "return the DISPLAY Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::DISPLAY_TOKEN;
                                }
    /* expression variable */
<BodyMacro>{EXPR_VAR}         {
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the VAR Token
                                LOG_DEBUG << "return the VAR Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::VAR_TOKEN;
                                }
    /* expression functions */
<BodyMacro>{EXPR_RANDOM}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the RANDOM Token
                                LOG_DEBUG << "return the RANDOM Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::RANDOM_TOKEN;
                                }

<BodyMacro>{EXPR_VISITED}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the VISITED Token
                                LOG_DEBUG << "return the VISITED Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::VISITED_TOKEN;
                                }
<BodyMacro>{EXPR_PREVIOUS}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the PREVIOUS Token
                                LOG_DEBUG << "return the PREVIOUS Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::PREVIOUS_TOKEN;
                                }
<BodyMacro>{EXPR_TURNS}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the TURNS Token
                                LOG_DEBUG << "return the TURNS Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::TURNS_TOKEN;
                                }

<BodyMacro>{EXPR_OPEN}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the OPEN Token
                                LOG_DEBUG << "return the OPEN Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::OPEN_TOKEN;
                                }
<BodyMacro>{EXPR_CLOSE}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the CLOSE Token
                                LOG_DEBUG << "return the CLOSE Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::CLOSE_TOKEN;
                                }

    /*Arithmetic Tokens*/
<BodyMacro>{EXPR_ADD}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the ADD Token
                                LOG_DEBUG << "return the ADD Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::ADD_TOKEN;
                                }
<BodyMacro>{EXPR_MUL}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the MUL Token
                                LOG_DEBUG << "return the MUL Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::MUL_TOKEN;
                                }
<BodyMacro>{EXPR_SUB}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the SUB Token
                                LOG_DEBUG << "return the SUB Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::SUB_TOKEN;
                                }
<BodyMacro>{EXPR_DIV}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the DIV Token
                                LOG_DEBUG << "return the DIV Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::DIV_TOKEN;
                                }
<BodyMacro>{EXPR_MOD}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the MOD Token
                                LOG_DEBUG << "return the MOD Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::MOD_TOKEN;
                                }

    /*Logical Tokens*/
<BodyMacro>{EXPR_GT}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the GT Token
                                LOG_DEBUG << "return the GT Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::GT_TOKEN;
                                }
<BodyMacro>{EXPR_GTE}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the GTE Token
                                LOG_DEBUG << "return the GTE Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::GTE_TOKEN;
                                }
<BodyMacro>{EXPR_LT}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the LT Token
                                LOG_DEBUG << "return the LT Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::LT_TOKEN;
                                }
<BodyMacro>{EXPR_LTE}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the LTE Token
                                LOG_DEBUG << "return the LTE Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::LTE_TOKEN;
                                }
<BodyMacro>{EXPR_NEQ}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the NEQ Token
                                LOG_DEBUG << "return the NEQ Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::NEQ_TOKEN;
                                }
<BodyMacro>{EXPR_IS}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the IS Token
                                LOG_DEBUG << "return the IS Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::IS_TOKEN;
                                }
<BodyMacro>{EXPR_EQ}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the EQ Token
                                LOG_DEBUG << "return the EQ Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::EQ_TOKEN;
                                }
<BodyMacro>{EXPR_AND}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the AND Token
                                LOG_DEBUG << "return the AND Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::AND_TOKEN;
                                }
<BodyMacro>{EXPR_OR}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the OR Token
                                LOG_DEBUG << "return the OR Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::OR_TOKEN;
                                }

    /*Assignment Token*/
<BodyMacro>{EXPR_TO}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the TO Token
                                LOG_DEBUG << "return the TO Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::TO_TOKEN;
                                }
<BodyMacro>{EXPR_ASS}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the ASS Token
                                LOG_DEBUG << "return the ASS Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::ASS_TOKEN;
                                }

    /* strings and ints */
<BodyMacro>{EXPR_STR_LIMITER}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the EXPR_STR_LIMITER Token
                                LOG_DEBUG << "return the EXPR_STR_LIMITER Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::EXPR_STR_LIMITER_TOKEN;
                                }

<BodyMacro>{EXPR_STR}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the EXPR_STR Token
                                LOG_DEBUG << "return the EXPR_STR Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::EXPR_STR_TOKEN;
                                }
<BodyMacro>{EXPR_INT}		{
                                //stay in condition BodyMacro, look for next token
                                LOG_DEBUG << "stay in condition BodyMacro, look for next token";
                                //return the INT Token
                                LOG_DEBUG << "return the INT Token";
                                LOG_DEBUG << "\t matched:";
                                LOG_DEBUG << YYText();
                                SAVE_TOKEN;
                                return BisonParser::token::INT_TOKEN;
                                }
    /* leave the macro */
<BodyMacro>{MACRO_CLOSE}        {
                                //enter condition Body
                                LOG_DEBUG << "enter condition Body";
                                BEGIN(Body);
                                //return the MACRO_CLOSE token
                                LOG_DEBUG << "return the MACRO_CLOSE token";
                                return BisonParser::token::MACRO_CLOSE;
                                }

    /* unexpected Token(s) */
<BodyMacro>.                    {
                                //TODO: lexer error in BodyMacro
                                LOG_ERROR << "lexer error in condition BodyMacro";
                                LOG_ERROR << "\t matched:";
                                LOG_ERROR << YYText();
                                }

%%