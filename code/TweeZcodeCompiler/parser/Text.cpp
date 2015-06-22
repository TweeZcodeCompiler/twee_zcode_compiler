//
// Created by lars on 05.06.15.
//

#include "include/Passage/Body/Text.h"

#include <string>

Text::Text(std::string content) {
    this->content = content;
}

std::string Text::getContent() {
    return this->content;
}

std::string Text::to_string() {
    return "Text: " + this->getContent();
}