//
// Created by Danni on 09.06.15.
//

#include "TweeCompiler.h"
#include "ZAssemblyGenerator.h"
#include "exceptions.h"
#include "Utils.h"

#include <plog/Log.h>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <set>

#include <Passage/Body/Link.h>
#include <Passage/Body/Text.h>
#include <Passage/Body/Newline.h>

#include <Passage/Body/Expressions/BinaryOperation.h>
#include <Passage/Body/Expressions/UnaryOperation.h>
#include <algorithm>

#include <plog/Log.h>
#include <Passage/Body/Expressions/Const.h>
#include <Passage/Body/Expressions/Variable.h>

#include <Passage/Body/Macros/Print.h>
#include <Passage/Body/Macros/SetMacro.h>

#include <Passage/Body/Expressions/Variable.h>
#include <Passage/Body/Macros/IfMacro.h>
#include <Passage/Body/Macros/ElseIfMacro.h>
#include <Passage/Body/Macros/ElseMacro.h>
#include <Passage/Body/Macros/EndIfMacro.h>



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

#ifndef ASSGEN
#define ASSGEN (*_assgen)
#endif

void maskString(std::string &string) {
    std::replace(string.begin(), string.end(), ' ', '_');
}

string routineNameForPassageName(std::string passageName) {
    stringstream ss;
    maskString(passageName);
    ss << "R_" << passageName;
    return ss.str();
}


string routineNameForPassage(Passage &passage) {
    return routineNameForPassageName(passage.getHead().getName());
}

string labelForPassage(Passage &passage) {
    stringstream ss;
    string passageName = passage.getHead().getName();
    maskString(passageName);
    ss << "L_" << passageName;
    return ss.str();
}

void TweeCompiler::compile(TweeFile &tweeFile, std::ostream &out) {
    _assgen = unique_ptr<ZAssemblyGenerator>(new ZAssemblyGenerator(out));

    vector<Passage> passages = tweeFile.getPassages();

    globalVariables = std::set<std::string>();

    labelCount = 0;
    
    int ifDepth = -1;
    int ifJumpLabelID = 0;
    int ifEndJumpLabelID = 0;
    int possibleIfDepth = 255;
    std::array<std::string,12> nextJumpLabels;
    std::array<std::string,12> endJumpLabels;
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
        ASSGEN.addGlobal(PASSAGE_GLOB)
                .addGlobal(USER_INPUT);

        // call start routine first
        ASSGEN.addRoutine(MAIN_ROUTINE)
                .markStart()
                .call(routineNameForPassageName("start"), PASSAGE_GLOB)
                .addLabel(JUMP_TABLE_LABEL);

        for (auto passage = passages.begin(); passage != passages.end(); ++passage) {
            int passageId = passageName2id.at(passage->getHead().getName());

            ASSGEN.jumpEquals(ZAssemblyGenerator::makeArgs({std::to_string(passageId), PASSAGE_GLOB}),
                              labelForPassage(*passage));
        }

        for (auto passage = passages.begin(); passage != passages.end(); ++passage) {
            ASSGEN.addLabel(labelForPassage(*passage))
                    .call(routineNameForPassage(*passage), PASSAGE_GLOB)
                    .jump(JUMP_TABLE_LABEL);
        }

        ASSGEN.addLabel(JUMP_TABLE_END_LABEL);

        ASSGEN.quit();
    }

    // passage routines
    {
        for (auto passage = passages.begin(); passage != passages.end(); ++passage) {
            const vector<unique_ptr<BodyPart>> &bodyParts = passage->getBody().getBodyParts();

            // declare passage routine
            ASSGEN.addRoutine(routineNameForPassage(*passage));

            ASSGEN.println(string("***** ") + passage->getHead().getName() + string(" *****"));

            //  print passage contents
            for (auto it = bodyParts.begin(); it != bodyParts.end(); it++) {
                BodyPart *bodyPart = it->get();
                if (Text *text = dynamic_cast<Text *>(bodyPart)) {
                    ASSGEN.print(text->getContent());
                } else if(Newline* text = dynamic_cast<Newline*>(bodyPart)) {
                    ASSGEN.newline();
                } else if (Print *print = dynamic_cast<Print *>(bodyPart)) {
                    evalExpression(print->getExpression().get());
                    ASSGEN.print_num("sp");
                } else if (SetMacro *setMacro = dynamic_cast<SetMacro *>(bodyPart)) {
                    evalExpression(setMacro->getExpression().get());
                } else if (IfMacro * ifmacro = dynamic_cast<IfMacro *>(bodyPart)) {
                    //depth++,
                    //check preceding ifmacro
                    //set label for jump to after if block (else, else if, endif)
                    //evaluate expression
                    //make jump to set label if expression is true
                    ifDepth++;
                    /*
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
                        LOG_ERROR << errorMessage;
                        throw TweeDocumentException();
                    }
                    */
                    nextJumpLabels[ifDepth] = IF_JUMP_LABEL + std::to_string(++ifJumpLabelID);
                    endJumpLabels[ifDepth] = IF_JUMP_END_LABEL + std::to_string(++ifEndJumpLabelID);
                    //TODO: evaluate expression IfMacro
                    evalExpression(ifmacro->getExpression().get());
                    ASSGEN.jumpNotEquals(ZAssemblyGenerator::makeArgs({"sp", "0"}) , nextJumpLabels[ifDepth]);
                    //precedingIfMacros[ifDepth] = 1;
                } else if (ElseIfMacro * elseifmacro = dynamic_cast<ElseIfMacro *>(bodyPart)) {
                    //check preceding ifmacro
                    //save label for jump to after if/else if block , in this case else
                    //make jump to set label if expression is true
                    //set label for jump to after if block (else, else if, endif)
                    //evaluate expression
                    //make jump to set label if expression is true
                    //else part
                    /*
                     * if (precedingIfMacros[ifDepth] != 1 || precedingIfMacros[ifDepth] != 2){
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
                        LOG_ERROR << errorMessage;
                        throw TweeDocumentException();
                    }
                     */
                    ASSGEN.jump(endJumpLabels[ifDepth]);
                    ASSGEN.addLabel(nextJumpLabels[ifDepth]);
                    nextJumpLabels[ifDepth] = IF_JUMP_LABEL + std::to_string(++ifJumpLabelID);
                    //TODO: evaluate expression ElseIfMacro
                    std::string ifExprVal = elseifmacro->getExpression()->to_string();
                    LOG_DEBUG << ifExprVal;
                    if(ifExprVal.compare("Const: 1")) {
                        ASSGEN.push("1");
                    } else if (ifExprVal.compare("Const: 0")) {
                        ASSGEN.push("0");
                    }
                    ASSGEN.jumpNotEquals(ZAssemblyGenerator::makeArgs({"sp", "0"}) , nextJumpLabels[ifDepth]);
                    // precedingIfMacros[ifDepth] = 2;
                } else if (ElseMacro * elsemacro = dynamic_cast<ElseMacro *>(bodyPart)) {
                    //check preceding ifmacro
                    //save label for jump to after if/else if block , in this case else
                    //make jump to set label if expression is true
                    /*
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
                        LOG_ERROR << errorMessage;
                        throw TweeDocumentException();
                    }
                    */
                    ASSGEN.jump(endJumpLabels[ifDepth]);
                    ASSGEN.addLabel(nextJumpLabels[ifDepth]);
                    //precedingIfMacros[ifDepth] = 3;
                } else if (EndIfMacro * endifemacro = dynamic_cast<EndIfMacro *>(bodyPart)) {
                    //check preceding ifmacro
                    //make jump to set label if expression is trueJumpLabels[ifDepth];
                    //decrease depth
                    /*
                    if (precedingIfMacros[ifDepth] == 0) {
                        std::string errorMessage = "unexpected EndIfMacro without an if macro, look for an orphaned <<[ ]*endif[ ]*>> ";
                        LOG_ERROR << errorMessage;
                        throw TweeDocumentException();
                    } else {
                        std::string errorMessage = "unexpected EndIfMacro without an if macro, look for an orphaned <<[ ]*endif[ ]*>> ";
                        LOG_ERROR << errorMessage;
                    }
                     */
                    ASSGEN.addLabel(endJumpLabels[ifDepth]);
                    ifDepth--;
                    //precedingIfMacros[ifDepth] = 0;
                }
            }

            ASSGEN.newline();

            vector<Link *> links;
            // get links from passage
            for (auto it = bodyParts.begin(); it != bodyParts.end(); ++it) {
                BodyPart *bodyPart = it->get();
                if (Link *link = dynamic_cast<Link *>(bodyPart)) {
                    links.push_back(link);
                }
            }

            // present choices to user
            ASSGEN.println("Select one of the following options");
            int i = 1;
            for (auto link = links.begin(); link != links.end(); link++) {
                ASSGEN.println(string("    ") + to_string(i) + string(") ") + (*link)->getTarget());
                i++;
            }

            ASSGEN.addLabel(READ_BEGIN);

            // read user input
            ASSGEN.read_char(USER_INPUT);

            // jump to according link selection
            i = 0;
            for (auto link = links.begin(); link != links.end(); link++) {
                string label = string("L") + to_string(i);
                ASSGEN.jumpEquals(ZAssemblyGenerator::makeArgs({to_string(ZSCII_NUM_OFFSET + i), USER_INPUT}), label);

                i++;
            }

            // no proper selection was made
            ASSGEN.jump(READ_BEGIN);

            i = 0;
            for (auto link = links.begin(); link != links.end(); link++) {
                string label = string("L") + to_string(i);
                try {
                    int targetPassageId = passageName2id.at((*link)->getTarget());
                    ASSGEN.addLabel(label);
#ifdef ZAS_DEBUG
                    ASSGEN.print(string("selected ") + to_string(targetPassageId) );
                    #endif
                    ASSGEN.ret(to_string(targetPassageId));
                } catch (const out_of_range &err) {
                    cerr << "could not find passage for link target \"" << (*link)->getTarget() << "\"" << endl;
                    throw TweeDocumentException();
                }
                i++;
            }
        }
    }
}

void TweeCompiler::evalExpression(Expression *expression) {

    if (Const<int> *constant = dynamic_cast<Const<int> *>(expression)) {

        ASSGEN.push(std::to_string(constant->getValue()));

    } else if (Const<bool> *constant = dynamic_cast<Const<bool> *>(expression)) {
        ASSGEN.push(std::to_string(constant->getValue()));
    } else if (Variable *variable = dynamic_cast<Variable *>(expression)) {

        std::string prunedVarName = variable->getName().substr(1);

        if (Utils::contains<std::string>(globalVariables, prunedVarName)) {
            ASSGEN.push(prunedVarName);
        } else {
            globalVariables.insert(prunedVarName);
            ASSGEN.addGlobal(prunedVarName);
            ASSGEN.push(prunedVarName);
        }

    } else if (BinaryOperation *binOp = dynamic_cast<BinaryOperation *>(expression)) {
        std::pair<std::string, std::string> labels;

        TweeCompiler::evalExpression(binOp->getLeftSide().get());
        TweeCompiler::evalExpression(binOp->getRightSide().get());

        switch (binOp->getOperator()) {
            case BinOps::ADD:
                ASSGEN.add("sp", "sp", "sp");
                break;
            case BinOps::SUB:
                ASSGEN.sub("sp", "sp", "sp");
                break;
            case BinOps::MUL:
                ASSGEN.mul("sp", "sp", "sp");
                break;
            case BinOps::DIV:
                ASSGEN.div("sp", "sp", "sp");
                break;
            case BinOps::MOD:
                ASSGEN.mod("sp", "sp", "sp");
                break;
            case BinOps::AND:
                ASSGEN.land("sp", "sp", "sp");
                break;
            case BinOps::OR:
                ASSGEN.lor("sp", "sp", "sp");
                break;
            case BinOps::TO:
                ASSGEN.store("sp", "sp");
                break;
            case BinOps::LT:
                labels = labelCreator("lower");
                ASSGEN.jumpLower(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;
            case BinOps::LTE:
                labels = labelCreator("lowerEquals");
                ASSGEN.jumpLowerEquals(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;
            case BinOps::GT:
                labels = labelCreator("greater");
                ASSGEN.jumpGreater(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;
            case BinOps::GTE:
                labels = labelCreator("greaterEquals");
                ASSGEN.jumpGreaterEquals(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;
            case BinOps::IS:
                ASSGEN.jumpEquals(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;
            case BinOps::NEQ:
                ASSGEN.jumpNotEquals(std::string("sp") + " " + std::string("sp"), labels.first);
                ASSGEN.push("0");
                ASSGEN.jump(labels.second);
                ASSGEN.addLabel(labels.first);
                ASSGEN.push("1");
                ASSGEN.addLabel(labels.second);
                break;

        }
    } else if (UnaryOperation *unOp = dynamic_cast<UnaryOperation *>(expression)) {
        TweeCompiler::evalExpression(unOp->getExpression().get());
        switch (unOp->getOperator()) {
            case UnOps::NOT:
                ASSGEN.lnot("sp", "sp");
                break;
            case UnOps::PLUS:
                ASSGEN.push("0");
                ASSGEN.add("sp", "sp", "sp");
                break;
            case UnOps::MINUS:
                ASSGEN.push("0");
                ASSGEN.sub("sp", "sp", "sp");
                break;
            default:
                break;
        }
    }
}

std::pair<std::string, std::string> TweeCompiler::labelCreator(std::string labelName) {

    std::string resultLabel = labelName.append("_");
    std::string afterLabel = "after_";
    resultLabel.append(std::to_string(labelCount));
    afterLabel.append(std::to_string(labelCount));
    ++labelCount;

    return std::make_pair(resultLabel, afterLabel);

}