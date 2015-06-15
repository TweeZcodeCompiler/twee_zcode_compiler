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

    return "Formatted Text" + std::endl + "Content: " + this->content + std::endl + "Is Italic: " + this->isItalic
           + std::endl + "Is Bold: " + this->isBold + std::endl + "Is Underlined: " + this->isUnderlined;

}