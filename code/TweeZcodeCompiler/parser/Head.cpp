//
// Created by lars on 05.06.15.
//

#include "include/Passage/Head.h"

#include <vector>
#include <string>

Head::Head(std::string name) {
    this->name = name;
}

Head::Head(std::string name, std::vector <std::string> tags) {
    this->name = name;
    this->tags = tags;
}

std::string Head::getName() const {
    return this->name;
}

const std::vector <std::string>& Head::getTags() const {
    return this->tags;
}

std::string Head::to_string() const {
    std::string result = std::string("  Head\n    Name: ") + this->getName() + std::string("\n    Tags: ");

    auto tags = this->getTags();
    for (std::vector<std::string>::iterator iter = tags.begin(); iter != tags.end(); ++iter)
        result += (*iter + std::string(" "));

    return result;
}