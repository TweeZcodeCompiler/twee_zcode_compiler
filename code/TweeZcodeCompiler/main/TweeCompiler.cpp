//
// Created by Danni on 09.06.15.
//

#include "TweeCompiler.h"
#include "ZAssemblyGenerator.h"
#include "exceptions.h"
#include <sstream>
#include <iostream>
#include <Passage/Body/Link.h>
#include <Passage/Body/Text.h>
#include <Passage/Body/Newline.h>
#include <Passage/Body/Expressions/Variable.h>
#include <Passage/Body/Macros/IfMacro.h>
#include <Passage/Body/Macros/ElseIfMacro.h>
#include <Passage/Body/Macros/ElseMacro.h>
#include <Passage/Body/Macros/EndIfMacro.h>
#include <plog/Log.h>

using namespace std;

static const string PASSAGE_GLOB = "PASSAGE_PTR",
        JUMP_TABLE_LABEL = "JUMP_TABLE_START",
        JUMP_TABLE_END_LABEL = "JUMP_TABLE_END",
        MAIN_ROUTINE = "main",
        USER_INPUT = "USER_INPUT",
        READ_BEGIN = "READ_BEGIN",
        IF_JUMP_LABEL = "IFJUMP",
        IF_JUMP_END_LABEL = "IFJUMPEND";

static const unsigned int ZSCII_NUM_OFFSET = 49;

//#define ZAS_DEBUG

void maskString(std::string& string) {
    std::replace( string.begin(), string.end(), ' ', '_');
}

string routineNameForPassageName(std::string passageName) {
    stringstream ss;
    maskString(passageName);
    ss << "R_" << passageName;
    return ss.str();
}


string routineNameForPassage(Passage& passage) {
    return routineNameForPassageName(passage.getHead().getName());
}

string labelForPassage(Passage& passage) {
    stringstream ss;
    string passageName = passage.getHead().getName();
    maskString(passageName);
    ss << "L_" << passageName;
    return ss.str();
}

void TweeCompiler::compile(TweeFile &tweeFile, std::ostream &out) {
    ZAssemblyGenerator assgen(out);
    vector<Passage> passages = tweeFile.getPassages();

    int ifDepth = -1;
    int ifJumpLabelID = 0;
    int ifEndJumpLabelID = 0;
    int possibleIfDepth = 255;
    std::array<std::string,255> nextJumpLabels;
    std::array<std::string,255> endJumpLabels;
    std::array<int,255> precedingIfMacros;
    for(auto precedingIfMacro = precedingIfMacros.begin(); precedingIfMacro != precedingIfMacros.end(); ++precedingIfMacro) {
        LOG_DEBUG << precedingIfMacro;
    }
    {
        int i = 0;
        for (auto passage = passages.begin(); passage != passages.end(); ++passage) {
            passageName2id[passage->getHead().getName()] = i;
            i++;
        }
    }

    // main routine
    {
        // globals
        assgen.addGlobal(PASSAGE_GLOB)
                .addGlobal(USER_INPUT);

        // call start routine first
        assgen.addRoutine(MAIN_ROUTINE)
                .markStart()
                .call(routineNameForPassageName("start"), PASSAGE_GLOB)
                .addLabel(JUMP_TABLE_LABEL);

        for(auto passage = passages.begin(); passage != passages.end(); ++passage) {
            int passageId = passageName2id.at(passage->getHead().getName());

            assgen.jumpEquals(ZAssemblyGenerator::makeArgs({std::to_string(passageId), PASSAGE_GLOB}), labelForPassage(*passage));
        }

        for(auto passage = passages.begin(); passage != passages.end(); ++passage) {
            assgen.addLabel(labelForPassage(*passage))
                    .call(routineNameForPassage(*passage), PASSAGE_GLOB)
                    .jump(JUMP_TABLE_LABEL);
        }

        assgen.addLabel(JUMP_TABLE_END_LABEL);

        assgen.quit();
    }

    // passage routines
    {
        for(auto passage = passages.begin(); passage != passages.end(); ++passage) {
            const vector<unique_ptr<BodyPart>>& bodyParts = passage->getBody().getBodyParts();

            // declare passage routine
            assgen.addRoutine(routineNameForPassage(*passage));

            assgen.println(string("***** ") + passage->getHead().getName() + string(" *****"));

            //  print passage contents
            for(auto it = bodyParts.begin(); it != bodyParts.end(); it++) {
                BodyPart* bodyPart = it->get();
                if(Text* text = dynamic_cast<Text*>(bodyPart)) {
                    assgen.print(text->getContent());
                } else if(Newline* text = dynamic_cast<Newline*>(bodyPart)) {
                    assgen.newline();
                } else if (Variable * variable = dynamic_cast<Variable *>(bodyPart)) {
                    assgen.variable(variable->getName());
                } else if (Macro * macro = dynamic_cast<Macro *>(bodyPart)) {
                    LOG_DEBUG << "Found a Macro";
                    if (IfMacro * ifmacro = dynamic_cast<IfMacro *>(macro)) {
                        //depth++,
                        //check preceding ifmacro
                        //set label for jump to after if block (else, else if, endif)
                        //evaluate expression
                        //make jump to set label if expression is true
                        ifDepth++;
                        if (precedingIfMacros[ifDepth] != 0) {
                            std::string errorMessage = "unexpected IfMacro after ";

                            switch(precedingIfMacros[ifDepth]) {
                                case 1:
                                    errorMessage += "another if IfMacro: ";
                                    errorMessage += "check your code for an <<[ ]*if.*>> not followed by an <<endif>> before another <<[ ]*if.*>>";
                                    break;
                                case 2:
                                    errorMessage += "an ElseIfMacro: ";
                                    errorMessage += "check your code for an <<[ ]*else[ ]*if.*>> not followed by an <<endif>> before another <<[ ]*if.*>>";
                                    break;
                                case 3:
                                    errorMessage += "an ElseIfMacro: ";
                                    errorMessage += "check your code for an <<[ ]*else[ ]*>> not followed by an <<endif>> before another <<[ ]*if.*>>";
                                    break;
                            }
                            cerr << errorMessage;
                            throw TweeDocumentException();
                        }
                        nextJumpLabels[ifDepth] = IF_JUMP_LABEL + std::to_string(++ifJumpLabelID);
                        endJumpLabels[ifDepth] = IF_JUMP_END_LABEL + std::to_string(++ifEndJumpLabelID);
                        //TODO: evaluate expression IfMacro
                        std::string ifExprVal = ifmacro->getExpression()->to_string();
                        LOG_DEBUG << ifExprVal;
                        if(ifExprVal.compare("Const: 1")) {
                            assgen.push("1");
                        } else if (ifExprVal.compare("Const: 0")) {
                            assgen.push("0");
                        }
                        assgen.jumpNotEquals(ZAssemblyGenerator::makeArgs({"sp", "0"}) , nextJumpLabels[ifDepth]);
                        precedingIfMacros[ifDepth] = 1;
                    } else if (ElseIfMacro * elseifmacro = dynamic_cast<ElseIfMacro *>(bodyPart)) {
                        //check preceding ifmacro
                        //save label for jump to after if/else if block , in this case else
                        //make jump to set label if expression is true
                        //set label for jump to after if block (else, else if, endif)
                        //evaluate expression
                        //make jump to set label if expression is true
                        //else part
                        if (precedingIfMacros[ifDepth] != 1 || precedingIfMacros[ifDepth] != 2) {
                            std::string errorMessage = "unexpected ElseIfMacro ";

                            switch(precedingIfMacros[ifDepth]) {
                                case 0:
                                    errorMessage += "without a starting ifMacro: ";
                                    errorMessage += "check your code for an <<[ ]*else[ ]*if.*>> without an associated <<if.*>>";
                                    break;
                                case 3:
                                    errorMessage += "after an ElseMacro";
                                    errorMessage += "check your code for an <<[ ]*else[ ]*if.*>> after an <<[ ]*else[ ]*>>";
                                    break;
                            }
                            cerr << errorMessage;
                            throw TweeDocumentException();
                        }
                        assgen.jump(endJumpLabels[ifDepth]);
                        assgen.addLabel(nextJumpLabels[ifDepth]);
                        nextJumpLabels[ifDepth] = IF_JUMP_LABEL + std::to_string(++ifJumpLabelID);
                        //TODO: evaluate expression ElseIfMacro
                        std::string ifExprVal = elseifmacro->getExpression()->to_string();
                        LOG_DEBUG << ifExprVal;
                        if(ifExprVal.compare("Const: 1")) {
                            assgen.push("1");
                        } else if (ifExprVal.compare("Const: 0")) {
                            assgen.push("0");
                        }
                        assgen.jumpNotEquals(ZAssemblyGenerator::makeArgs({"sp", "0"}) , nextJumpLabels[ifDepth]);
                        precedingIfMacros[ifDepth] = 2;
                    } else if (ElseMacro * elsemacro = dynamic_cast<ElseMacro *>(bodyPart)) {
                        //check preceding ifmacro
                        //save label for jump to after if/else if block , in this case else
                        //make jump to set label if expression is true
                        if (precedingIfMacros[ifDepth] != 1 || precedingIfMacros[ifDepth] != 2) {
                            std::string errorMessage = "unexpected ElseMacro ";

                            switch(precedingIfMacros[ifDepth]) {
                                case 0:
                                    errorMessage += "without a starting ifMacro: ";
                                    errorMessage += "check your code for an <<[ ]*else[ ]*>> without an associated <<if.*>>";
                                    break;
                                case 3:
                                    errorMessage += "after an ElseMacro";
                                    errorMessage += "check your code for an <<[ ]*else[ ]*>> after an <<[ ]*else[ ]*>>";
                                    break;
                            }
                            cerr << errorMessage;
                            throw TweeDocumentException();
                        }
                        assgen.jump(endJumpLabels[ifDepth]);
                        assgen.addLabel(nextJumpLabels[ifDepth]);
                        precedingIfMacros[ifDepth] = 3;
                    } else if (EndIfMacro * endifemacro = dynamic_cast<EndIfMacro *>(bodyPart)) {
                        //check preceding ifmacro
                        //make jump to set label if expression is trueJumpLabels[ifDepth];
                        //decrease depth
                        if (precedingIfMacros[ifDepth] == 0) {
                            std::string errorMessage = "unexpected EndIfMacro without an if macro, look for an orphaned <<[ ]*endif[ ]*>> ";
                            cerr << errorMessage;
                            throw TweeDocumentException();
                        }
                        assgen.addLabel(endJumpLabels[ifDepth]);
                        ifDepth--;
                        precedingIfMacros[ifDepth] = 0;
                    }
                }
            }

            assgen.newline();

            vector<Link*> links;
            // get links from passage
            for(auto it = bodyParts.begin(); it != bodyParts.end(); ++it)
            {
                BodyPart* bodyPart = it->get();
                if(Link* link = dynamic_cast<Link*>(bodyPart)) {
                    links.push_back(link);
                }
            }

            // present choices to user
            assgen.println("Select one of the following options");
            int i = 1;
            for (auto link = links.begin(); link != links.end(); link++) {
                assgen.println(string("    ") + to_string(i) + string(") ") + (*link)->getTarget() );
                i++;
            }

            assgen.addLabel(READ_BEGIN);

            // read user input
            assgen.read_char(USER_INPUT);

            // jump to according link selection
            i = 0;
            for (auto link = links.begin(); link != links.end(); link++) {
                string label = string("L") + to_string(i);
                assgen.jumpEquals(ZAssemblyGenerator::makeArgs({to_string(ZSCII_NUM_OFFSET + i), USER_INPUT}), label);

                i++;
            }

            // no proper selection was made
            assgen.jump(READ_BEGIN);

            i = 0;
            for (auto link = links.begin(); link != links.end(); link++) {
                string label = string("L") + to_string(i);
                try {
                    int targetPassageId = passageName2id.at((*link)->getTarget());
                    assgen.addLabel(label);
                    #ifdef ZAS_DEBUG
                    assgen.print(string("selected ") + to_string(targetPassageId) );
                    #endif
                    assgen.ret(to_string(targetPassageId));
                } catch (const out_of_range &err) {
                    cerr << "could not find passage for link target \"" << (*link)->getTarget() << "\"" << endl;
                    throw TweeDocumentException();
                }
                i++;
            }
        }
    }
}
