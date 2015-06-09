//
// Created by lars on 05.06.15.
//

#include "include/TweeFile.h"

#include <vector>
#include <string>


TweeFile::TweeFile() { }

vector <Passage> TweeFile::getPassages() {

    return this->passages;

}

TweeFile &TweeFile::operator+=(const Passage &passage) {

    this->passages.push_back(passage);
    return *this;
}

std::string TweeFile::to_string() {

    std::string result = "TweeFile" + std::endl;

    for (int i = 0; i < passages.size(); ++i) {
        result += this->passages[i].to_string();
    }

    return result;

}