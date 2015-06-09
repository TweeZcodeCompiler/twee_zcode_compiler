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
        MAIN_ROUTINE = "main";

string routineNameForPassage(Passage& passage) {
    stringstream ss;
    ss << "R_" << passage.getPassageName();
    return ss.str();
}

string labelForPassage(Passage& passage) {
    stringstream ss;
    ss << "L_" << passage.getPassageName();
    return ss.str();
}

void TweeCompiler::compile(TweeDoc &tweeDoc, std::ostream &out) {
    ZAssemblyGenerator assgen(out);
    vector<Passage>& passages = tweeDoc.passages;

    int i = 0;
    for(auto passage = passages.begin(); passage != passages.end(); ++passage) {
        passageName2id[passage->getPassageName()] = i;
        i++;
    }

    // main routine
    {
        assgen.addGlobal(PASSAGE_GLOB)
                .addRoutine(MAIN_ROUTINE)
                .call(MAIN_ROUTINE)
                .addLabel(JUMP_TABLE_LABEL);


        for(auto passage = passages.begin(); passage != passages.end(); ++passage) {
            assgen.addLabel(labelForPassage(*passage))
                    .call(passage->getPassageName())
                    .jump(JUMP_TABLE_LABEL);
        }

        for(auto passage = passages.begin(); passage != passages.end(); ++passage) {
            int passageId = passageName2id[passage->getPassageName()];
            assgen.jumpEquals(std::to_string(passageId), labelForPassage(*passage));
        }

        assgen.addLabel(JUMP_TABLE_END_LABEL);

        assgen.quit();
    }

    // passage routines
    {
        for(auto passage = passages.begin(); passage != passages.end(); ++passage) {
            assgen.addRoutine(passage->getPassageName())
                    .read_char(ZAssemblyGenerator::STACK_POINTER);
        }

        // TODO: go through all links and present options here. code should set PASSAGE_GLOB and return

        assgen.ret("0");
    }
}
