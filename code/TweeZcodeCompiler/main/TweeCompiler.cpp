//
// Created by Danni on 09.06.15.
//

#include "TweeCompiler.h"
#include "ZAssemblyGenerator.h"
#include "exceptions.h"
#include "Utils.h"

#include <sstream>
#include <iostream>
#include <algorithm>

#include <Passage/Body/Link.h>
#include <Passage/Body/Text.h>
#include <Passage/Body/Newline.h>
#include <Passage/Body/Macros/Print.h>
#include <Passage/Body/Expressions/Expression.h>
#include <Passage/Body/Expressions/Const.h>
#include <Passage/Body/Expressions/Variable.h>
#include <Passage/Body/Expressions/BinaryOperation.h>


using namespace std;

static const string PASSAGE_GLOB = "PASSAGE_PTR",
        JUMP_TABLE_LABEL = "JUMP_TABLE_START",
        JUMP_TABLE_END_LABEL = "JUMP_TABLE_END",
        MAIN_ROUTINE = "main",
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
    vector<std::string> globalVariables;

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
                }
                if(Newline* text = dynamic_cast<Newline*>(bodyPart)) {
                    assgen.newline();
                }
                if (Print *print = dynamic_cast<Print *>(bodyPart)) {
                    Expression *expression = print->getExpression().get();
                    if (Const<int> *constant = dynamic_cast<Const<int> *>(expression)) {
                        assgen.print(std::to_string(constant->getValue()));
                    }
                    if (Variable *variable = dynamic_cast<Variable *>(expression)) {
                        if (Utils::includes(globalVariables, variable->getName()) == 0) {
                            assgen.
                        } else {
                            assgen.print("0");
                        }
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