//
// Created by lars on 06.06.15.
//

#include "include/Passage/Body/FormattedText.h"

#include <string>

FormattedText::FormattedText(std::string content, bool isItalic, bool isBold, bool isUnderlined) : Text(content) {

    this->isItalic = isItalic;
    this->isBold = isBold;
    this->isUnderlined = isUnderlined;

}

bool FormattedText::getIsItalic() {

    return this->isItalic;

}

bool FormattedText::getIsBold() {

    return this->isBold;

}

bool FormattedText::getIsUnderlined() {

    return this->isUnderlined;

}

std::string FormattedText::to_string() {

    return std::string("Formatted Text \n")
           + std::string("Is Italic: ") + (this->getIsItalic() ? std::string("Yes \n") : std::string("No \n"))
           + std::string("Is Bold: ") + (this->getIsBold() ? std::string("Yes \n") : std::string("No \n"))
           + std::string("Is Underlined: ") + (this->getIsUnderlined() ? std::string("Yes \n") : std::string("No \n"))
           + std::string("Content: ") + this->getContent();

}