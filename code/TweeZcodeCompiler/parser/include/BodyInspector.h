//
// Created by lars on 11.07.15.
//

#ifndef BODYINSPECTOR_H
#define BODYINSPECTOR_H

#include <Passage/Body/Text.h>
#include <Passage/Body/Link.h>
#include <Passage/Body/Newline.h>

#include "Passage/Body/Macros/Print.h"
#include "Passage/Body/Macros/Display.h"
#include "Passage/Body/Macros/SetMacro.h"

#include "Passage/Body/Macros/IfMacro.h"
#include "Passage/Body/Macros/ElseMacro.h"
#include "Passage/Body/Macros/ElseIfMacro.h"
#include "Passage/Body/Macros/EndIfMacro.h"

class BodyInspector {

public:

    BodyInspector();

    void visit(Text host);

    void visit(Link host);

    void visit(Newline host);

    void visit(Print host);

    void visit(Display host);

    void visit(SetMacro host);

    void visit(IfMacro host);

    void visit(ElseMacro host);

    void visit(ElseIfMacro host);

    void visit(EndIfMacro host);

};


#endif //BODYINSPECTOR_H
