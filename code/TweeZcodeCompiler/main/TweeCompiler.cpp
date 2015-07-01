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

using namespace std;

static const string PASSAGE_GLOB = "PASSAGE_PTR",
        TEXT_FORMAT_ITALIC = "ITALIC",
        TEXT_FORMAT_BOLD = "BOLD",
        TEXT_FORMAT_REVERSED_VIDEO = "REVERSED_VIDEO",
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

    .Func format type, i, value
    LOOP_TYPE:
    je type i ?~CONTINUE
            add TEXT_FORMAT_ITALIC i -> i
    je i 1 ?TOGGLE_OFF
         store i 1
    jump PRINT_MACRO

    TOGGLE_OFF:
    store i 0
    jump PRINT_MACRO

    CONTINUE:
    add i 1 -> i
    jl i 5 ?LOOP_TYPE
         ret 0			//Fail

    PRINT_MACRO:
    store i TEXT_FORMAT_ITALIC
    LOOP_PRINT:
    mul value 10 -> value
    je i 0 ?CONTINUE_PRINT
         add value 1

    CONTINUE_PRINT:
    add i 1
    jl i 5 ?LOOP_PRINT

         set_text_style value
    ret 0


    // print appropriate text formatting args
    {
        assgen.addGlobal(TEXT_FORMAT_ITALIC)
                .addGlobal(TEXT_FORMAT_BOLD)
                .addGlobal(TEXT_FORMAT_FIXED_PITCH)
                .addGlobal(TEXT_FORMAT_REVERSED_VIDEO);

        string varFormatType = "formatType";
        string varCounter = "i";
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
        args.push_back(ZRoutineArgument(varResult));
        assgen.addRoutine(TEXT_FORMAT_ROUTINE, args);

        assgen.addLabel(labelLoopType);
        assgen.jumpEquals(string(varFormatType + " " + varCounter), string("~" + labelContinue));
        assgen.


        assgen.jumpEquals(string(TEXT_FORMAT_ITALIC + " " + to_string(1)), labelItalicOff);
        assgen.setTextStyle();
        assgen.store(TEXT_FORMAT_ITALIC, 1);
        assgen.ret("0");
        assgen.addLabel(labelItalicOff);
        assgen.setTextStyle();
        assgen.store(TEXT_FORMAT_ITALIC, 0);
        assgen.ret("0");

        assgen.addLabel(labelNot1);
        assgen.jumpEquals(string(varFormatType + " " + to_string(2)), labelNot2);
        assgen.jumpEquals(string(TEXT_FORMAT_BOLD + " " + to_string(1)), labelBoldOff);
        assgen.setTextStyle();
        assgen.store(TEXT_FORMAT_BOLD, 1);
        assgen.ret("0");
        assgen.addLabel(labelBoldOff);
        assgen.setTextStyle();
        assgen.store(TEXT_FORMAT_BOLD, 0);
        assgen.ret("0");

        assgen.addLabel(labelNot2);
        assgen.jumpEquals(string(varFormatType + " " + to_string(3)), labelNot3);
        assgen.jumpEquals(string(TEXT_FORMAT_FIXED_PITCH + " " + to_string(1)), labelFixedPitchOff);
        assgen.setTextStyle();
        assgen.store(TEXT_FORMAT_FIXED_PITCH, 1);
        assgen.ret("0");
        assgen.addLabel(labelFixedPitchOff);
        assgen.setTextStyle();
        assgen.store(TEXT_FORMAT_FIXED_PITCH, 0);
        assgen.ret("0");

        assgen.addLabel(labelNot3);
        assgen.jumpEquals(string(TEXT_FORMAT_REVERSED_VIDEO + " " + to_string(1)), labelReversedVideoOff);
        assgen.setTextStyle();
        assgen.store(TEXT_FORMAT_REVERSED_VIDEO, 1);
        assgen.ret("0");
        assgen.addLabel(labelReversedVideoOff);
        assgen.setTextStyle();
        assgen.store(TEXT_FORMAT_REVERSED_VIDEO, 0);
        assgen.ret("0");
    }

    // toggle Italic
    {

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
                    assgen.setTextStyle(format->getFormat());
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
