//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Macros/Display.h"

Display::Display(const std::string passage) {
    this->passage = passage;
}

const std::string &Display::getPassage() const {
    return this->passage;
}

std::string Display::to_string() const {
    return "Display: " + this->getPassage();
}

void Display::accept(BodyInspector bodyInspector) const {
    bodyInspector.visit(*this);
}