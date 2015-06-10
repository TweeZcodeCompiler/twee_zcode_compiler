//
// Created by Danni on 09.06.15.
//

#include "TweeCompiler.h"
#include "ZAssemblyGenerator.h"
#include <sstream>
#include <iostream>

using namespace std;

static const string PASSAGE_GLOB = "PASSAGE_PTR",
        JUMP_TABLE_LABEL = "JUMP_TABLE_START",
        JUMP_TABLE_END_LABEL = "JUMP_TABLE_END",
        MAIN_ROUTINE = "main",
        USER_INPUT = "USER_INPUT";

static const unsigned int ZSCII_NUM_OFFSET = 49;

static const bool DEBUG = true;

string routineNameForPassageName(std::string passageName) {
    stringstream ss;
    ss << "R_" << passageName;
    return ss.str();
}


string routineNameForPassage(Passage& passage) {
    return routineNameForPassageName(passage.getPassageName());
}

string labelForPassage(Passage& passage) {
    stringstream ss;
    ss << "L_" << passage.getPassageName();
    return ss.str();
}

void TweeCompiler::compile(TweeDoc &tweeDoc, std::ostream &out) {
    ZAssemblyGenerator assgen(out);
    vector<Passage>& passages = tweeDoc.passages;

    {
        int i = 0;
        for (auto passage = passages.begin(); passage != passages.end(); ++passage) {
            passageName2id[passage->getPassageName()] = i;
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
            int passageId = passageName2id.at(passage->getPassageName());

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
            // declare passage routine, print passage contents and read user input
            assgen.addRoutine(routineNameForPassage(*passage))
                    .print(passage->getPassageName())
                    .newline()
                    .read_char(USER_INPUT);

            // jump to according link selection
            int i = 0;
            for (auto link = passage->links.begin(); link != passage->links.end(); link++) {
                string label = string("L") + to_string(i);
                assgen.jumpEquals(ZAssemblyGenerator::makeArgs({USER_INPUT, to_string(ZSCII_NUM_OFFSET + i)}), label);

                i++;
            }

            // no proper selection was made
            assgen.ret("-1");

            i = 0;
            for (auto link = passage->links.begin(); link != passage->links.end(); link++) {
                string label = string("L") + to_string(i);
                try {
                    int targetPassageId = passageName2id.at(*link);
                    assgen.addLabel(label);
                    if (DEBUG) assgen.print(string("selected ") + to_string(targetPassageId) );
                    assgen.ret(to_string(targetPassageId));
                } catch (out_of_range &err) {
                    cerr << "could not find passage for link \"" << *link << "\"" << endl;
                    throw;
                }
                i++;
            }
        }
    }
}
