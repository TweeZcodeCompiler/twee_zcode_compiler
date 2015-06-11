//
// Created by Danni on 09.06.15.
//

#include "TweeCompiler.h"
#include "ZAssemblyGenerator.h"
#include <sstream>
#include <iostream>
#include <Passage/Body/Link.h>
#include <Passage/Body/Text.h>

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
            auto bodyParts = passage->getBody().getBodyParts();

            // declare passage routine
            assgen.addRoutine(routineNameForPassage(*passage));

            assgen.println(string("***** ") + passage->getHead().getName() + string(" *****"));

            //  print passage contents
            for(auto bodyPart = bodyParts.begin(); bodyPart != bodyParts.end(); bodyPart++) {
                if(Text* text = dynamic_cast<Text*>(*bodyPart)) {
                    assgen.print(text->getContent());
                }
            }

            assgen.newline();

            vector<Link*> links;
            // get links from passage
            for(auto bodyPart = bodyParts.begin(); bodyPart != bodyParts.end();
                ++bodyPart)
            {
                if(Link* link = dynamic_cast<Link*>(*bodyPart)) {
                    links.push_back(link);
                }
            }

            // present choices to user
            assgen.println("Select one of the following options:");
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
                assgen.jumpEquals(ZAssemblyGenerator::makeArgs({USER_INPUT, to_string(ZSCII_NUM_OFFSET + i)}), label);

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
                } catch (out_of_range &err) {
                    cerr << "could not find passage for link target \"" << (*link)->getTarget() << "\"" << endl;
                    throw;
                }
                i++;
            }
        }
    }
}
