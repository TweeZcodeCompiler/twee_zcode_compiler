//
// Created by Danni on 09.06.15.
//

#ifndef PROJECT_TWEECOMPILER_H
#define PROJECT_TWEECOMPILER_H

#include "BodyPartsVisitor.h"
#include <Passage/Body/Expressions/Expression.h>
#include <Passage/Body/Expressions/BinaryOperation.h>

#include <map>
#include <set>
#include <stack>

#include "ITweeCompiler.h"
#include "ZAssemblyGenerator.h"

struct IfContext {
    unsigned caseCount = 0;
    unsigned number = 0;
};

class TweeCompiler : public ITweeCompiler, BodyPartsVisitor {
public:
    void compile(TweeFile &tweeDoc, std::ostream &out);

    void evalExpression(Expression *);

    void evalAssignment(BinaryOperation *expression);

    std::pair<std::string, std::string> makeLabels(std::string);

    void visit(Text host);

    void visit(Link host);

    void visit(Newline host);

    void visit(PrintMacro host);

    void visit(DisplayMacro host);

    void visit(SetMacro host);

    void visit(IfMacro host);

    void visit(ElseMacro host);

    void visit(ElseIfMacro host);

    void visit(EndIfMacro host);


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
