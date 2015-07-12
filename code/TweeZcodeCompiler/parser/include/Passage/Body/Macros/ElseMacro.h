
#ifndef ELSEMACRO_H
#define ELSEMACRO_H

#include "Macro.h"
#include "../BodyPartsVisitor.h"

#include <string>
#include <memory>

class ElseMacro : public Macro {

public:

    ElseMacro();

    std::string to_string() const;

    virtual ElseMacro *clone() const {
        return new ElseMacro(*this);
    }

    void accept(BodyPartsVisitor);

};

#endif //ELSEMACRO_H
