//
// Created by lars on 05.06.15.
//

#include "include/TweeFile.h"

#include <vector>
#include <string>


TweeFile::TweeFile() { }

std::vector<Passage> TweeFile::getPassages() {
    return this->passages;
}

TweeFile::~TweeFile() {

}

TweeFile &TweeFile::operator+=(const Passage &passage) {
    this->passages.push_back(passage);
    return *this;
}

std::string TweeFile::to_string() {
    std::string result = "TweeFile \n";

    for (std::vector<Passage>::iterator iter = this->getPassages().begin(); iter != this->getPassages().end(); ++iter) {
        result += iter->to_string() + "\n";
    }

    return result;
}