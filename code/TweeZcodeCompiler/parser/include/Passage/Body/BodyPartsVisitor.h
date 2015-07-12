//
// Created by lars on 11.07.15.
//

#ifndef BODYPARTSVISITOR_H
#define BODYPARTSVISITOR_H

#include "Text.h"
#include "Link.h"
#include "Newline.h"

#include "Macros/PrintMacro.h"
#include "Macros/DisplayMacro.h"
#include "Macros/SetMacro.h"

#include "Macros/IfMacro.h"
#include "Macros/ElseMacro.h"
#include "Macros/ElseIfMacro.h"
#include "Macros/EndIfMacro.h"

class BodyPartsVisitor {

public:

    virtual void visit(Text host) = 0;

    virtual void visit(Link host) = 0;

    virtual void visit(Newline host) = 0;

    virtual void visit(PrintMacro host) = 0;

    virtual void visit(DisplayMacro host) = 0;

    virtual void visit(SetMacro host) = 0;

    virtual void visit(IfMacro host) = 0;

    virtual void visit(ElseMacro host) = 0;

    virtual void visit(ElseIfMacro host) = 0;

    virtual void visit(EndIfMacro host) = 0;

};


#endif //BODYPARTSVISITOR_H
