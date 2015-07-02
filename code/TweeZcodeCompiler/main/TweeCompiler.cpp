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
#include <algorithm>
#include <Passage/Body/FormattedText.h>
#include <Passage/Body/Macros/SetMacro.h>

#include <Passage/Body/Expressions/BinaryOperation.h>
#include <plog/Log.h>
#include <algorithm>
#include <Passage/Body/Expressions/Variable.h>
#include <plog/Log.h>
#include <Passage/Body/Expressions/UnaryOperation.h>
#include <Passage/Body/Expressions/Const.h>
#include <Passage/Body/Expressions/Variable.h>

#include <Passage/Body/Macros/Print.h>
#include <Passage/Body/Macros/SetMacro.h>



using namespace std;

static const string PASSAGE_GLOB = "PASSAGE_PTR",
        TEXT_FORMAT_ITALIC = "ITALIC",
        TEXT_FORMAT_BOLD = "BOLD",
        TEXT_FORMAT_REVERSE_VIDEO = "REVERSED_VIDEO",
        TEXT_FORMAT_FIXED_PITCH = "FIXED_PITCH",
        JUMP_TABLE_LABEL = "JUMP_TABLE_START",
        JUMP_TABLE_END_LABEL = "JUMP_TABLE_END",
        MAIN_ROUTINE = "main",
        TEXT_FORMAT_ROUTINE = "toggleTextFormat",
        USER_INPUT = "USER_INPUT",
        READ_BEGIN = "READ_BEGIN";

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

    // print appropriate text formatting args
    {
        ASSGEN.addGlobal(TEXT_FORMAT_REVERSE_VIDEO)
                .addGlobal(TEXT_FORMAT_BOLD)
                .addGlobal(TEXT_FORMAT_ITALIC)
                .addGlobal(TEXT_FORMAT_FIXED_PITCH);

        string varFormatType = "formatType";
        string varCounter = "counter";
        string varTypeValue = "typeValue";
        string varResult = "result";
        string labelLoopType = "LOOP_TYPE";
        string labelLoopPrint = "LOOP_PRINT";
        string labelContinue = "CONTINUE";
        string labelContinuePrint = "CONTINUE_PRINT";
        string labelToggleOff = "TOGGLE_OFF";
        string labelPrintMacro = "PRINT_MACRO";

        vector<ZRoutineArgument> args;
        args.push_back(ZRoutineArgument(varFormatType));
        args.push_back(ZRoutineArgument(varCounter));
        args.push_back(ZRoutineArgument(varTypeValue, to_string(1)));
        args.push_back(ZRoutineArgument(varResult));
        ASSGEN.addRoutine(TEXT_FORMAT_ROUTINE, args);

        ASSGEN.addLabel(labelLoopType)
                .jumpEquals(string(varFormatType + " " + varCounter), string("~" + labelContinue))
                .add(TEXT_FORMAT_ITALIC, varCounter, varCounter)
                .jumpEquals(varCounter + " " + to_string(1), labelToggleOff)
                .store(varCounter, to_string(1))
                .jump(labelPrintMacro);

        ASSGEN.addLabel(labelToggleOff)
                .store(varCounter, to_string(0))
                .jump(labelPrintMacro);

        ASSGEN.addLabel(labelContinue)
                .add(varCounter, to_string(1), varCounter)
                .jumpLess(varCounter + " " + to_string(5), labelLoopType)
                .ret("0");

        ASSGEN.addLabel(labelPrintMacro)
                .store(varCounter, TEXT_FORMAT_ITALIC);

        ASSGEN.addLabel(labelLoopPrint)
                .jumpEquals(varCounter + " " + to_string(0), labelContinuePrint)
                .add(varResult, varTypeValue, varResult);

        ASSGEN.addLabel(labelContinuePrint)
                .add(varCounter, to_string(1), varCounter)
                .mul(varTypeValue, to_string(2), varTypeValue)
                .jumpLess(varCounter + " " + to_string(5), labelLoopPrint);

        ASSGEN.setTextStyle(varResult)
                .ret("0");
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
                } else if (Newline *text = dynamic_cast<Newline *>(bodyPart)) {
                    ASSGEN.newline();
                } else if (FormattedText *format = dynamic_cast<FormattedText *>(bodyPart)) {
                    switch (format->getFormat()) {
                        case Format::UNDERLINED:
                            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, string("0"), "sp");
                            break;
                        case Format::BOLD:
                            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, string("1"), "sp");
                            break;
                        case Format::ITALIC:
                            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, string("2"), "sp");
                            break;
                        case Format::MONOSPACE:
                            ASSGEN.call_vs(TEXT_FORMAT_ROUTINE, string("3"), "sp");
                            break;
                        default:
                            LOG_DEBUG << "Unknown text formatting";
                            throw;
                    }
                } else if (Print *print = dynamic_cast<Print *>(bodyPart)) {
                    evalExpression(print->getExpression().get());
                    ASSGEN.print_num("sp");
                } else if (SetMacro *setMacro = dynamic_cast<SetMacro *>(bodyPart)) {
                    evalExpression(setMacro->getExpression().get());
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