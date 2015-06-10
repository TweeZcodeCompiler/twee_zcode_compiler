//
// Created by lars on 06.06.15.
//

#include "include/Passage/Body/FormattedText.h"

#include <string>

FormattedText::FormattedText(std::string content) : Text(content) {
    this->italic = false;
    this->bold = false;
    this->underlined = false;
}

FormattedText::FormattedText(std::string content, bool isItalic, bool isBold, bool isUnderlined) : Text(content) {
    this->italic = isItalic;
    this->bold = isBold;
    this->underlined = isUnderlined;
}


void FormattedText::setIsItalic(bool italic) {
    this->italic = italic;
}

void FormattedText::setIsBold(bool bold) {
    this->bold = bold;
}

void FormattedText::setIsUnderlined(bool underlined) {
    this->underlined = underlined;
}

bool FormattedText::isItalic() {
    return this->italic;
}

bool FormattedText::isBold() {
    return this->bold;
}

bool FormattedText::isUnderlined() {
    return this->underlined;
}

std::string FormattedText::to_string() {
    return std::string("Formatted Text \n")
           + std::string("Is Italic: ") + (this->isItalic() ? std::string("Yes \n") : std::string("No \n"))
           + std::string("Is Bold: ") + (this->isBold() ? std::string("Yes \n") : std::string("No \n"))
           + std::string("Is Underlined: ") + (this->isUnderlined() ? std::string("Yes \n") : std::string("No \n"))
           + std::string("Content: ") + this->getContent();

}