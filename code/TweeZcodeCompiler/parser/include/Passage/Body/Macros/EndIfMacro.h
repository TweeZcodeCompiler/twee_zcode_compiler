
#ifndef ENDIF_H
#define ENDIF_H

#include "Macro.h"
#include "../Expressions/Expression.h"
#include "../../../AssemblyGeneratorVisitor.h"

#include <string>
#include <memory>

class EndIfMacro : public Macro {

public:

    EndIfMacro();

    std::string to_string() const;

    virtual EndIfMacro *clone() const {
        return new EndIfMacro(*this);
    }

    void accept(AssemblyGeneratorVisitor) const;

};

#endif //ENDIF_H
