//
// Created by Danni on 09.06.15.
//

#ifndef PROJECT_TWEECOMPILER_H
#define PROJECT_TWEECOMPILER_H

#include <AssemblyGeneratorVisitor.h>
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

class TweeCompiler : public ITweeCompiler, AssemblyGeneratorVisitor {
public:
    void compile(TweeFile &tweeDoc, std::ostream &out);

    void evalExpression(Expression *);

    void evalAssignment(BinaryOperation *expression);

    std::pair<std::string, std::string> makeLabels(std::string);

    void visit(Text host) const;

    void visit(Link host) const;

    void visit(Newline host) const;

    void visit(Print host) const;

    void visit(Display host) const;

    void visit(SetMacro host) const;

    void visit(IfMacro host) const;

    void visit(ElseMacro host) const;

    void visit(ElseIfMacro host) const;

    void visit(EndIfMacro host) const;


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
