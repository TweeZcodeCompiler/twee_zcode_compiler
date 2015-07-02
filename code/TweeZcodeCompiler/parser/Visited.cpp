
#include "include/Passage/Body/Expressions/Visited.h"

Visited::Visited(const std::string passage) {
    this->passage = passage;
}

const std::string &Visited::getPassage() const {
    return this->passage;
}

std::string Visited::to_string() const {
    return "Visited: " + this->getPassage();
}