
#ifndef ENDIF_H
#define ENDIF_H

#include "Macro.h"
#include "../IBodyPartsVisitor.h"

#include <string>
#include <memory>

class EndIfMacro : public Macro {

public:

    EndIfMacro();

    std::string to_string() const;

    virtual EndIfMacro *clone() const {
        return new EndIfMacro(*this);
    }

    void accept(IBodyPartsVisitor&) const;
};

#endif //ENDIF_H
