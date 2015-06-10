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

void FormattedText::setIsItalic(bool italic) {

    this->isItalic = italic;

}

void FormattedText::setIsBold(bool bold) {

    this->isBold = bold;

}

void FormattedText::setIsUnderlined(bool underlined) {

    this->isUnderlined = underlined;

}

bool FormattedText::getIsBold() {

    return this->isBold;

}

bool FormattedText::getIsUnderlined() {

    return this->isUnderlined;

}

std::string FormattedText::to_string() {

    return "Formatted Text\n";

}