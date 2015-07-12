//
// Created by lars on 11.07.15.
//

#ifndef IBODYPARTSVISITOR_H
#define IBODYPARTSVISITOR_H

class Text;
class Link;
class Newline;

class PrintMacro;
class DisplayMacro;
class SetMacro;

class IfMacro;
class ElseMacro;
class ElseIfMacro;
class EndIfMacro;

class IBodyPartsVisitor {

public:

    virtual void visit(const Text& host) = 0;

    virtual void visit(const Link& host) = 0;

    virtual void visit(const Newline& host) = 0;

    virtual void visit(const PrintMacro& host) = 0;

    virtual void visit(const DisplayMacro& host) = 0;

    virtual void visit(const SetMacro& host) = 0;

    virtual void visit(const IfMacro& host) = 0;

    virtual void visit(const ElseMacro& host) = 0;

    virtual void visit(const ElseIfMacro& host) = 0;

    virtual void visit(const EndIfMacro& host) = 0;

};


#endif //IBODYPARTSVISITOR_H
