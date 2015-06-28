//
// Created by lars on 05.06.15.
//

#include "include/Passage/Body/Newline.h"

#include <string>

Newline::Newline() {
}

std::string Newline::to_string() {
    return "Newline: " + this->getContent();
}