/*
This is the lexical analyzer for passages in twee code.
TODO: debug parameter for the matching debug statements
*/

%{

#include <iostream>
#include <memory>

#include "include/Passage.h"
#include "include/Body.h"
#include "GeneratedTweeParser.hpp"
#include "TweeScanner.h"

#define SAVE_TOKEN yylval->string = new std::string(YYText(), YYLeng())
//#define TOKEN(t) (yylval.token = t)
#define DEBUG_LEXER std::cout << "LEXER DEBUG: " <<
%}

%option nodefault yyclass="Twee::TweeScanner" noyywrap c++
%option debug

DOUBLE_COLON    ::
PTITLE          [-=`~!@#$%^&*()_+\|/?';<>.,:\[\]\{\}\\"'a-zA-Z0-9 \t]+
PBODYCHAR       [-=`~!@#$%^&*()_+\|/?';<>.,:\[\]\{\}\\"'a-zA-Z0-9 \t]
PBODYWORD       {PBODYCHAR}+

 /*TODO: check Space and Tab behaviour*/
SPACE           [ \t]
LINEBREAK       [\n]

%x PHEAD
%x PTAGS
%x PBODY
%x PMACRO
%x PCLOSE
%x PBODYNL

%%
  /*at Start----------*/

<INITIAL>^{DOUBLE_COLON}  {
                                DEBUG_LEXER "new token DOUBLE_COLON " << '\n';
                                BEGIN(PHEAD);
                                DEBUG_LEXER "enter cnd -> PHEAD" << '\n';
                                return BisonParser::token::DOUBLE_COLON;
                                }

 /*discard the rest, because we are looking for a passage*/
<INITIAL>.               std::cout << "ERROR: TOKEN OUTSIDE OF A PASSAGE " << YYText() << '\n';

  /*PHEAD: Title scanning----------*/
 /*everything except LINEBREAK*/

<PHEAD>{PTITLE}         {
                        DEBUG_LEXER "new token PTITLE with content : " << YYText() << '\n';
                        SAVE_TOKEN;
                        return BisonParser::token::PTITLE;
                        }

<PHEAD>{LINEBREAK}      {
                        DEBUG_LEXER "new token LINEBREAK " << "\n";
                        DEBUG_LEXER "enter cnd -> PBODY" << '\n';
                        BEGIN(PBODY);

                        return BisonParser::token::LINEBREAK;
                        }

<PHEAD>.                {
                        DEBUG_LEXER "discarding char: " << YYText() << '\n';
                        }


  /*PBODY: Passage Body scanning----------*/
 /*scan until next ::-Line, while doing that go into PMACRO when "<<" */

 /* look to at Start---------- for next-paragraph-scanning */
 /*match the next uninterruped paragraph body text */

<PBODY>{PBODYWORD}      {
                        DEBUG_LEXER "new token PBODYWORD with content : " << YYText() << '\n';
                        SAVE_TOKEN;
                        return BisonParser::token::PBODYWORD;
                        }

<PBODY>{LINEBREAK}      {
                        BEGIN(PBODYNL);
                        DEBUG_LEXER "enter cnd -> PBODYNL" << '\n';
                        }

<PBODY>.                {
                        DEBUG_LEXER "discarding char: " << YYText() << '\n';
                        }
  /*PBODYNL: Passage Body New Line scanning----------*/
 /*after getting a new line, starts looking for a ::, else stuff gets put back on the stack*/

<PBODYNL>::             {
                        DEBUG_LEXER "new token DOUBLE_COLON " << "\n";
                        BEGIN(PHEAD);
                        return BisonParser::token::DOUBLE_COLON;
                        }

<PBODYNL>{LINEBREAK}    {
                        DEBUG_LEXER "PBODYNL matched \\n" << YYText() << '\n';
                        BEGIN(PBODY);
                        DEBUG_LEXER "new token LINEBREAK " << "\n";
                        DEBUG_LEXER "enter cnd -> PBODY" << '\n';
                        return BisonParser::token::LINEBREAK;
                        }

<PBODYNL>.              {
                        DEBUG_LEXER "PBODYNL matched : " << YYText() << '\n';
                        DEBUG_LEXER "PBODYNL putting " << YYText() << "back" << '\n';
                        unput(YYText()[0]);
                        BEGIN(PBODY);
                        DEBUG_LEXER "new token LINEBREAK " << "\n";
                        DEBUG_LEXER "enter cnd -> PBODY" << '\n';
                        return BisonParser::token::LINEBREAK;
                        }

%%