//
// Created by tobias on 6/30/15.
//

#include "BodyPart.h"

#ifndef PROJECT_FORMATTEDTEXT_H
#define PROJECT_FORMATTEDTEXT_H

class FormattedText : public BodyPart {
private:
    Format format;

public:
    FormattedText(std::string type);

    virtual FormattedText* clone() const {
        return new FormattedText(*this);
    }

    Format getFormat();

    std::string to_string() const;

    enum Format {
        ITALIC, BOLD, MONOSPACE, UNDERLINED
    };
};

#endif //PROJECT_FORMATTEDTEXT_H
