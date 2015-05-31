//
// Created by lars on 31.05.15.
//

#include "PassageBody.h"

#include <string>
#include <vector>

Passage::PassageBody() { }

void addElement(PassageElement element) {
    this->passageElements.push_back(element);
}

PassageBody operator+=(const PassageElement &element) {
    this->passageElements.push_back(element);
}

std::string to_string() {

    std::string result = "";

    for (std::vector<int>::iterator it = this->passageElements.begin(); it != this->passageElements.end(); ++it)
        result += *(it).to_string();

    return result;
}

std::string to_ZASS() {

    std::string result = "";

    for (std::vector<int>::iterator it = this->passageElements.begin(); it != this->passageElements.end(); ++it)
        result += *(it).to_ZASS();

    return result;

}