
#ifndef ENDIF_H
#define ENDIF_H

#include "Macro.h"
#include "../Expressions/Expression.h"

#include <string>
#include <memory>

class EndIf : public Macro {

public:

    EndIf();

    std::string to_string() const;

    virtual EndIf *clone() const {
        return new EndIf(*this);
    }

};

#endif //ENDIF_H
