//
// Created by lars on 05.06.15.
//

#include "include/TweeFile.h"

#include <vector>
#include <string>


TweeFile::TweeFile() { }

const std::vector<Passage>& TweeFile::getPassages() const {
    return this->passages;
}

TweeFile::~TweeFile() {

}

TweeFile &TweeFile::operator+=(const Passage &passage) {
    this->passages.push_back(Passage(passage));
    return *this;
}

std::string TweeFile::to_string() const {
    std::string result = "TweeFile \n";

    auto passages = this->getPassages();

    for (auto iter = passages.begin(); iter != passages.end(); ++iter) {
        result += iter->to_string() + "\n";
    }

    return result;
}