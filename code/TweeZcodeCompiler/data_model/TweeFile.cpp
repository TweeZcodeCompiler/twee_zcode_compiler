//
// Created by lars on 05.06.15.
//

#include "include/TweeFile.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

TweeFile::TweeFile() { }

vector <Passage> TweeFile::getPassages() {

    return this->passages;

}

TweeFile &TweeFile::operator+=(const Passage &passage) {

    this->passages.push_back(passage);
    return *this;
}

std::string TweeFile::to_string() {

    std::string result = "TweeFile \n";

    /*for (int i = 0; i < passages.size(); ++i) {
        result += this->passages[i].to_string();
    }*/

    return result;

}