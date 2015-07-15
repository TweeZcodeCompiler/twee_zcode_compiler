//
// Created by tobias on 6/30/15.
//

#include "BodyPart.h"

#ifndef FORMATTING_H
#define FORMATTING_H

enum Format {
    ITALIC, BOLD, UNDERLINED, MONOSPACE
};

class Formatting : public BodyPart {
private:
    Format format;

public:
    Formatting(std::string type);

    Format getFormat() const;

    std::string getFormatString() const;

    std::string to_string() const;

    virtual Formatting* clone() const {
        return new Formatting(*this);
    }

    void accept(IBodyPartsVisitor&) const;
};

#endif //FORMATTING_H
