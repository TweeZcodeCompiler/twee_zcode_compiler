#ifndef NEWLINE_H
#define NEWLINE_H

#include "BodyPart.h"
#include "BodyPartsVisitor.h"

#include <string>

class Newline : public BodyPart {

public:
    virtual Newline* clone() const {
        return new Newline(*this);
    }

    Newline();

    std::string to_string() const;

    void accept(BodyPartsVisitor&);
};


#endif //NEWLINE_H
