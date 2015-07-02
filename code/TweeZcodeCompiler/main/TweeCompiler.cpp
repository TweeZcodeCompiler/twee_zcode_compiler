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
#include <Passage/Body/Expressions/Const.h>
#include <Passage/Body/Expressions/Variable.h>

#include <Passage/Body/Macros/Print.h>
#include <Passage/Body/Macros/SetMacro.h>



using namespace std;
using namespace std::experimental;

static const string GLOB_PASSAGE = "PASSAGE_PTR",
        LABEL_MAIN_LOOP = "MAIN_LOOP",
        ROUTINE_MAIN = "main",
        GLOBAL_USER_INPUT = "USER_INPUT",
        TABLE_LINKED_PASSAGES = "LINKED_PASSAGES",
        TABLE_USERINPUT_LOOKUP = "USERINPUT_LOOKUP",
        ROUTINE_PASSAGE_BY_ID = "passage_by_id",
        ROUTINE_NAME_FOR_PASSAGE = "print_name_for_passage",
        ROUTINE_DISPLAY_LINKS = "display_links";

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

    {
        int i = 0;
        for (auto passage = passages.begin(); passage != passages.end(); ++passage) {
            passageName2id[passage->getHead().getName()] = i;
            i++;
        }
    }

    // tables needed for routine linking
    ASSGEN.addByteArray(TABLE_LINKED_PASSAGES, (unsigned)passages.size());
    ASSGEN.addByteArray(TABLE_USERINPUT_LOOKUP, (unsigned)passages.size());


    // globals
    ASSGEN.addGlobal(GLOB_PASSAGE)
            .addGlobal(GLOBAL_USER_INPUT);


    // call passage by id routine
    {
        const string idLocal = "id";
        ASSGEN.addRoutine(ROUTINE_PASSAGE_BY_ID, vector<ZRoutineArgument>({ZRoutineArgument(idLocal)}));
        for(auto it = passages.begin(); it != passages.end(); ++it) {
            string passageName = it->getHead().getName();
            unsigned id = passageName2id.at(passageName);
            ASSGEN.jumpEquals(ASSGEN.makeArgs({idLocal, to_string(id)}), labelForPassage(*it));
        }

        // error case
        ASSGEN.print("invalid id for passage "); // TODO: print offending argument as soon as print_num is available
        ASSGEN.quit();

        for(auto it = passages.begin(); it != passages.end(); ++it) {
            ASSGEN.addLabel(labelForPassage(*it)).call_vs(routineNameForPassage(*it), nullopt, "sp").ret("0");
        }
    }


    // print passage name by id routine
    {
        const string idLocal = "id";
        ASSGEN.addRoutine(ROUTINE_NAME_FOR_PASSAGE, vector<ZRoutineArgument>({ZRoutineArgument(idLocal)}));
        for(auto it = passages.begin(); it != passages.end(); ++it) {
            string passageName = it->getHead().getName();
            unsigned id = passageName2id.at(passageName);
            ASSGEN.jumpEquals(ASSGEN.makeArgs({idLocal, to_string(id)}), labelForPassage(*it));
        }

        // error case
        ASSGEN.print("invalid id for passage "); // TODO: print offending argument as soon as print_num is available
        ASSGEN.quit();

        for(auto it = passages.begin(); it != passages.end(); ++it) {
            ASSGEN.addLabel(labelForPassage(*it)).print(it->getHead().getName()).ret("0");
        }
    }


    // main routine
    {
        // call start routine first
        ASSGEN.addRoutine(ROUTINE_MAIN)
                .markStart()
                .call_vs(ROUTINE_PASSAGE_BY_ID, to_string(passageName2id.at(string("Start"))), "sp");


        ASSGEN.addLabel(LABEL_MAIN_LOOP)
                .call_vs(ROUTINE_DISPLAY_LINKS, string("sp"), "sp")
                .call_vs(ROUTINE_PASSAGE_BY_ID, string("sp"), "sp");

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
            for(auto it = bodyParts.begin(); it != bodyParts.end(); it++) {
                BodyPart* bodyPart = it->get();
                if(Text* text = dynamic_cast<Text*>(bodyPart)) {
                    ASSGEN.print(text->getContent());
                } else if (Variable* variable = dynamic_cast<Variable*>(bodyPart)) {
                    ASSGEN.variable(variable->getName());
                } else if (Link* link = dynamic_cast<Link*>(bodyPart)) {
                    // TODO: catch invalid link
                    ASSGEN.storeb(TABLE_LINKED_PASSAGES, passageName2id.at(link->getTarget()), 1);
                } else if (Newline* text = dynamic_cast<Newline*>(bodyPart)) {
                    ASSGEN.newline();
                } else if (Print *print = dynamic_cast<Print *>(bodyPart)) {
                    evalExpression(print->getExpression().get());
                    ASSGEN.print_num("sp");
                } else if (SetMacro *op = dynamic_cast<SetMacro *>(bodyPart)) {
                    LOG_DEBUG << "generate SetMacro assembly code";

                    if (BinaryOperation *binaryOperation = (BinaryOperation *) (op->getExpression().get())) {
                        if (Variable *variable = (Variable *) (binaryOperation->getLeftSide().get())) {
                            std::string variableName = variable->getName();
                            variableName = variableName.erase(0, 1); //remove the $ symbol

                            // TODO: merge this with evalExpression
                            if (Utils::contains<std::string>(globalVariables, variableName)) {
                                ASSGEN.push(variableName);
                            } else {
                                globalVariables.insert(variableName);
                                ASSGEN.addGlobal(variableName);
                                ASSGEN.push(variableName);
                            }

                            evalExpression(binaryOperation->getRightSide().get());

                            ASSGEN.load("sp", variableName);
                        }
                    }
                }
            }
        }
    }
}

void TweeCompiler::evalExpression(Expression *expression) {

    if (Const<int> *constant = dynamic_cast<Const<int> *>(expression)) {

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