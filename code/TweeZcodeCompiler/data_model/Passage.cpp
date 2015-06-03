//
// Created by lars on 31.05.15.
//

#include "include/Passage/Passage.h"
#include "include/PassageElement.h"

#include <string>

Passage::Passage(std::string name) {

    this->name = name;

}

void addElement(PassageElement element) {
    this->elements.push_back(element);
}

std::string Passage::to_string() {

    std::string result = "Name " + this->name;

    for (std::vector<PassageElement>::iterator iter = this->elemnts.begin(); iter != this->elemnts.end(); ++iter) {
        result += *iter;
    }

    return result;
}
