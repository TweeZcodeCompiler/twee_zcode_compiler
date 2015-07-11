//
// Created by lars on 11.07.15.
//

#ifndef ASSEMBLYGENERATORVISITOR_H
#define ASSEMBLYGENERATORVISITOR_H

#include "Passage/Body/Text.h"
#include "Passage/Body/Link.h"
#include "Passage/Body/Newline.h"

#include "Passage/Body/Macros/Print.h"
#include "Passage/Body/Macros/Display.h"
#include "Passage/Body/Macros/SetMacro.h"

#include "Passage/Body/Macros/IfMacro.h"
#include "Passage/Body/Macros/ElseMacro.h"
#include "Passage/Body/Macros/ElseIfMacro.h"
#include "Passage/Body/Macros/EndIfMacro.h"

class AssemblyGeneratorVisitor {

public:

    virtual void visit(Text host) const = 0;

    virtual void visit(Link host) const = 0;

    virtual void visit(Newline host) const = 0;

    virtual void visit(Print host) const = 0;

    virtual void visit(Display host) const = 0;

    virtual void visit(SetMacro host) const = 0;

    virtual void visit(IfMacro host) const = 0;

    virtual void visit(ElseMacro host) const = 0;

    virtual void visit(ElseIfMacro host) const = 0;

    virtual void visit(EndIfMacro host) const = 0;

};


#endif //ASSEMBLYGENERATORVISITOR_H
