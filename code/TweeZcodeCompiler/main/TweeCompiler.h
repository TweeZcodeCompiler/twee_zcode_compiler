//
// Created by Danni on 09.06.15.
//

#ifndef PROJECT_TWEECOMPILER_H
#define PROJECT_TWEECOMPILER_H

#include "ITweeCompiler.h"
#include "ZAssemblyGenerator.h"
#include <map>
#include <set>
#include <Passage/Body/Expressions/Expression.h>

class TweeCompiler : public ITweeCompiler {
public:
    void compile(TweeFile& tweeDoc, std::ostream& out);

    void evalExpression(Expression *);

    std::pair<std::string, std::string> labelCreator(std::string);

private:
    std::map<std::string, int> passageName2id;

    std::unique_ptr<ZAssemblyGenerator> _assgen;
    std::set<std::string> globalVariables;
    int labelCount;
};


#endif //PROJECT_TWEECOMPILER_H
