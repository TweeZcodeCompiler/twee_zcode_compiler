//
// Created by lars on 31.05.15.
//

#include "include/PassageElements/Text.h"

#include <string>

Text::Text(std::string content) {
    this->content = content;
}

void Text::addText(std::string content) {
    this->content += content;
}

std::string Text::to_string() {

    return "Text" + std::endl + this->content + std::endl;

}