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
#include <stack>
#include <Passage/Body/Expressions/BinaryOperation.h>

struct IfContext {
    unsigned caseCount = 0;
    unsigned number = 0;
};

class TweeCompiler : public ITweeCompiler {
public:
    void compile(TweeFile& tweeDoc, std::ostream& out);

    void evalExpression(Expression *);
    void evalAssignment(BinaryOperation * expression);

    std::pair<std::string, std::string> makeLabels(std::string);

private:
    std::map<std::string, int> passageName2id;

    std::unique_ptr<ZAssemblyGenerator> _assgen;

    std::set<std::string> globalVariables;
    int labelCount;
    std::stack<IfContext> ifContexts;
    unsigned ifCount = 0;

    IfContext makeNextIfContext();

    void makePassageRoutine(const Passage &passage);
};


#endif //PROJECT_TWEECOMPILER_H
