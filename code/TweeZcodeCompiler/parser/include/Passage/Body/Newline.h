#ifndef NEWLINE_H
#define NEWLINE_H

#include "BodyPart.h"
#include "IBodyPartsVisitor.h"

#include <string>

class Newline : public BodyPart {

public:

    Newline();

    std::string to_string() const;

    virtual Newline* clone() const {
        return new Newline(*this);
    }

    void accept(IBodyPartsVisitor&) const;
};


#endif //NEWLINE_H
