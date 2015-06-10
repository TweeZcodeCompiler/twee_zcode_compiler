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

std::string Head::getName() {

    return this->name;

}

std::vector <std::string> Head::getTags() {

    return this->tags;

}

std::string Head::to_string() {

    return std::to_string('\n');

}