//
// Created by iguana on 06.06.15.
//

#ifndef FORMATTEDTEXT_H
#define FORMATTEDTEXT_H

#include "Text.h"

#include <string>

class FormattedText : public Text {

private:
    bool isItalic;
    bool isBold;
    bool isUnderlined;

public:

    FormattedText(std::string, bool, bool, bool);

    bool getIsItalic();

    bool getIsBold();

    bool getIsUnderlined();

    void setIsItalic(bool italic);

    void setIsBold(bool bold);

    void setIsUnderlined(bool underlined);

    std::string to_string();

};

#endif //FORMATTEDTEXT_H
