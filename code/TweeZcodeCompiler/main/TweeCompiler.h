//
// Created by Danni on 09.06.15.
//

#ifndef PROJECT_TWEECOMPILER_H
#define PROJECT_TWEECOMPILER_H

#include <Passage/Body/IBodyPartsVisitor.h>
#include <Passage/Body/Expressions/Expression.h>
#include <Passage/Body/Expressions/BinaryOperation.h>
#include <Passage/Body/Link.h>

#include <map>
#include <set>
#include <stack>

#include "ITweeCompiler.h"
#include "ZAssemblyGenerator.h"

struct IfContext {
    unsigned caseCount = 0;
    unsigned number = 0;
};

class TweeCompiler : public ITweeCompiler, IBodyPartsVisitor {
public:
    void compile(TweeFile &tweeDoc, std::ostream &out);

    Expression * optimizeExpression(Expression *expression);

    void evalExpression(Expression *expression);

    void evalAssignment(BinaryOperation* expression);

    std::pair<std::string, std::string> makeLabels(std::string);

    void visit(const Text& host);

    void visit(const Formatting& host);

    void visit(const Link& host);

    void visit(const Newline& host);

    void visit(const PrintMacro& host);

    void visit(const DisplayMacro& host);

    void visit(const SetMacro& host);

    void visit(const IfMacro& host);

    void visit(const ElseMacro& host);

    void visit(const ElseIfMacro& host);

    void visit(const EndIfMacro& host);


private:
    std::map<std::string, int> passageName2id;

    std::unique_ptr<ZAssemblyGenerator> _assgen;

    std::set<std::string> globalVariables;
    int labelCount;
    unsigned linkCount = 0;
    std::vector<Link> foundLinks;
    std::stack<IfContext> ifContexts;
    unsigned ifCount = 0;

    IfContext makeNextIfContext();

    void makePassageRoutine(const Passage &passage);
};


#endif //PROJECT_TWEECOMPILER_H
