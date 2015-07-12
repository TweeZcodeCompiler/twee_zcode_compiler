//
// Created by lars on 05.06.15.
//

#include "include/Passage/Body/Text.h"
#include "include/Passage/Body/BodyPartsVisitor.h"

#include <string>

Text::Text(std::string content) {
    this->content = content;
}

std::string Text::getContent() const {
    return this->content;
}

std::string Text::to_string() const {
    return "Text: " + this->getContent();
}

void Text::accept(BodyPartsVisitor visitor) {
    visitor.visit(*this);
}