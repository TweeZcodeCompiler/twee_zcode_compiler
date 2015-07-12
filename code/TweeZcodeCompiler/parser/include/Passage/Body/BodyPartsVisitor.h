//
// Created by lars on 11.07.15.
//

#ifndef BODYPARTSVISITOR_H
#define BODYPARTSVISITOR_H

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

class BodyPartsVisitor {

public:

    virtual void visit(Text& host) = 0;

    virtual void visit(Link& host) = 0;

    virtual void visit(Newline& host) = 0;

    virtual void visit(PrintMacro& host) = 0;

    virtual void visit(DisplayMacro& host) = 0;

    virtual void visit(SetMacro& host) = 0;

    virtual void visit(IfMacro& host) = 0;

    virtual void visit(ElseMacro& host) = 0;

    virtual void visit(ElseIfMacro& host) = 0;

    virtual void visit(EndIfMacro& host) = 0;

};


#endif //BODYPARTSVISITOR_H
