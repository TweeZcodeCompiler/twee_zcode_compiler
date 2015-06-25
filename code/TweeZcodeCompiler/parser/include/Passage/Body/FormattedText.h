//
// Created by lars on 06.06.15.
//

#ifndef FORMATTEDTEXT_H
#define FORMATTEDTEXT_H

#include "Text.h"

#include <string>

class FormattedText : public Text {

private:
    bool italic;
    bool bold;
    bool underlined;

public:

    FormattedText(std::string);

    FormattedText(std::string, bool, bool, bool);

    virtual FormattedText* clone() const{
        return new FormattedText(*this);
    }

    bool isItalic();

    bool isBold();

    bool isUnderlined();

    void setIsItalic(bool italic);

    void setIsBold(bool bold);

    void setIsUnderlined(bool underlined);

    std::string to_string();

};

#endif //FORMATTEDTEXT_H
