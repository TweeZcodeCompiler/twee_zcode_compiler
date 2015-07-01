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
#include <algorithm>
#include <Passage/Body/FormattedText.h>
#include <Passage/Body/Expressions/Variable.h>
#include <plog/Log.h>

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

    // print appropriate text formatting args
    {
        assgen.addGlobal(TEXT_FORMAT_REVERSE_VIDEO)
                .addGlobal(TEXT_FORMAT_BOLD)
                .addGlobal(TEXT_FORMAT_ITALIC)
                .addGlobal(TEXT_FORMAT_FIXED_PITCH);

        string varFormatType = "formatType";
        string varCounter = "i";
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
        assgen.addRoutine(TEXT_FORMAT_ROUTINE, args);

        assgen.addLabel(labelLoopType)
                .jumpEquals(string(varFormatType + " " + varCounter), string("~" + labelContinue))
                .add(TEXT_FORMAT_ITALIC, varCounter, varCounter)
                .jumpEquals(varCounter + " " + to_string(1), labelToggleOff)
                .store(varCounter, to_string(1))
                .jump(labelPrintMacro);

        assgen.addLabel(labelToggleOff)
                .store(varCounter, to_string(0))
                .jump(labelPrintMacro);

        assgen.addLabel(labelContinue)
                .add(varCounter, to_string(1), varCounter)
                .jumpLess(varCounter + " " + to_string(5), labelLoopType)
                .ret("0");

        assgen.addLabel(labelPrintMacro)
                .store(varCounter, TEXT_FORMAT_ITALIC);

        assgen.addLabel(labelLoopPrint)
                .jumpEquals(varCounter + " " + to_string(0), labelContinuePrint)
                .add(varResult, varTypeValue, varResult);

        assgen.addLabel(labelContinuePrint)
                .add(varCounter, to_string(1), varCounter)
                .mul(varTypeValue, to_string(2), varTypeValue)
                .jumpLess(varCounter + " " + to_string(5), labelLoopPrint);

        assgen.setTextStyle(varResult)
                .ret("0");
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
                } else if (FormattedText* format = dynamic_cast<FormattedText*>(bodyPart)) {
                    switch (format->getFormat()) {
                        case Format::BOLD:
                           // assgen.cal
                            break;
                        case Format::ITALIC:

                            break;
                        case Format::UNDERLINED:

                            break;
                        case Format::MONOSPACE:

                            break;
                        default:
                            LOG_DEBUG << "Unknown text formatting";
                            throw;
                    }
                } else if (Variable * variable = dynamic_cast<Variable *>(bodyPart)) {
                    assgen.variable(variable->getName());
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
