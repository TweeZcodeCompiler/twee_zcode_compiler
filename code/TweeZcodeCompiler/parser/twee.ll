/*
This is the lexical analyzer for passages in twee code.
*/

%{

#include <iostream>
#include <memory>

#include "Passage.h"
#include "Body.h"
#include "GeneratedTweeParser.hh"
#include "TweeScanner.h"

#define SAVE_TOKEN yylval->string = new std::string(YYText(), YYLeng())
//#define TOKEN(t) (yylval.token = t)

%}

%option nodefault yyclass="Twee::TweeScanner" noyywrap c++

DOUBLE_COLON    ::
PTITLE          [-=`~!@#$%^&*()_+\|/?';<>.,:\]\{\}\\"'a-zA-Z0-9 \t]+
PTAGS_OPEN      \[
PTAGS_CLOSE     \]

PBODYCHAR       [-=`~!@#$%^&*()_+\|/?';>.,:\[\]\{\}\\"'a-zA-Z0-9 \t\n]
PBODYLT         <
PBODYWORD       {PBODYCHAR}*({PBODYLT}?{PBODYCHAR}+)*

PTAG            [-=`~!@#$%^&*()_+\|/?';<>.,:\[\{\}\\"'a-zA-Z0-9]+

 /*TODO: check Space and Tab behaviour*/
SPACE           [ \t]
 /*TAB             [\t]*/
LINEBREAK       [\n\r]

MACRO_OPEN      <<
MACRO_CLOSE     >>

%x PHEAD
%x PTAGS
%x PBODY
%x PMACRO
%x PCLOSE

%%
  /*at Start----------*/

<INITIAL,PBODY>^{DOUBLE_COLON}  {
                                std::cout << "=> DOUBLE_COLON " << YYText() << '\n';
                                BEGIN(PHEAD);
                                // *yylval = atoi(yytext);
                                //std::cout << DOUBLE_COLON << '\n';
                                //return DOUBLE_COLON;
                                }

 /*discard the rest, because we are looking for a passage*/
<INITIAL>.               std::cout << "ERROR: TOKEN OUTSIDE OF A PASSAGE " << YYText() << '\n';

  /*PHEAD: Title scanning----------*/
 /*everything except ( and LINEBREAK*/

<PHEAD>{PTITLE}         {
                        std::cout << "=> PTITLE " << YYText() << '\n';
                        // *yylval = atoi(yytext);
                        //return Passage_name;
                        }

<PHEAD>{PTAGS_OPEN}     {
                        std::cout << "=> PTAGS_OPEN " << '\n';
                        BEGIN(PTAGS);
                        //std::cout << "BEGIN(PTAGS);" << '\n';
                        }

<PHEAD>{LINEBREAK}      {
                        std::cout << "=> LINEBREAK " << "\n\n";
                        BEGIN(PBODY);
                        //std::cout << "BEGIN(PBODY);" << '\n';
                        // *yylval = atoi(YYText());
                        SAVE_TOKEN;
                        return BisonParser::token::LINEBREAK;
                        }

  /*PTAGS: Title Tags scanning----------*/
 /*Get the Tags, discard the whitespaces, close when )*/

 /*match the next words */
<PTAGS>{PTAG}           {
                        std::cout << "=> PTAG " << YYText() << '\n';
                        }

 /*throw the spaces aside */
<PTAGS>{SPACE}          /*Skip*/

 /*match the end of the tags */
<PTAGS>{PTAGS_CLOSE}    {
                        std::cout << "=> PTAGS_CLOSE " << "\n";
                        BEGIN(PCLOSE);
                        //std::cout << "BEGIN(PCLOSE);" << '\n';
                        }
 /*TODO: Error when Linebreak?*/

  /*PBODYCLOSE: After Tags scanning----------*/
 /*Error when no Linebreak*/

 /*match a linebreak */
<PCLOSE>{LINEBREAK}     {
                            std::cout << "=> LINEBREAK " << YYText();
                            BEGIN(PBODY);
                            //std::cout << "BEGIN(PBODY);" << '\n';
                            
                            //return LINEBREAK;
                            }

 /*throw the spaces aside */
<PCLOSE>{SPACE}         /*Skip*/

 /*match unexpected chars to error */
<PCLOSE>.               /*TODO: Error when no Linebreak*/

  /*PBODY: Passage Body scanning----------*/
 /*scan until next ::-Line, while doing that go into PMACRO when "<<" */

 /* look to at Start---------- for next-paragraph-scanning */
 /*match the next uninterruped paragraph body text */
<PBODY>{PBODYWORD}      {
                        std::cout << "=> PBODYWORD " << YYText() << '\n';
                        //yylval.string = std::string(YYText());
                        SAVE_TOKEN;
                        return BisonParser::token::PBODYWORD;
                        }
 /*match macro start */
<PBODY>{MACRO_OPEN}      {
                        std::cout << "=> MACRO_OPEN " << '\n';
                        BEGIN(PMACRO);
                        //std::cout << "BEGIN(PMACRO);" << '\n';
                        }

  /*PMACRO: Passage Body Macro scanning----------*/
 /*scan macro, esp. macro tokens */
 /*TODO: Macro Keyword scanning*/
 /*match the whole macro text */
<PMACRO>(^[{MACRO_CLOSE}])+  {
                        std::cout << "=> MACRO_TEXT " << YYText() << '\n';
                        }
 /*match the macro close */
<PMACRO>{MACRO_CLOSE}     {
                        std::cout << "=> MACRO_CLOSE " << '\n';
                        BEGIN(PBODY);
                        //std::cout << "BEGIN(PBODY);" << '\n';
                        }

%%