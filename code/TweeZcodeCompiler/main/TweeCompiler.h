//
// Created by Danni on 09.06.15.
//

#ifndef PROJECT_TWEECOMPILER_H
#define PROJECT_TWEECOMPILER_H

#include "ITweeCompiler.h"
#include "ZAssemblyGenerator.h"
#include <map>

class TweeCompiler : public ITweeCompiler {
public:
    void compile(TweeFile& tweeDoc, std::ostream& out);

private:
    std::map<std::string, int> passageName2id;

    std::unique_ptr<ZAssemblyGenerator> _assgen;
};


#endif //PROJECT_TWEECOMPILER_H
