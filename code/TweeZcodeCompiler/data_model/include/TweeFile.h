//
// Created by lars on 05.06.15.
//

#ifndef TWEEFILE_H
#define TWEEFILE_H

#include "Passage/Passage.h"

#include <vector>
#include <string>

class TweeFile {

private:
    vector <Passage> passages;

public:
    TweeFile();

    vector <Passage> getPassages();

    TweeFile &operator+=(const Passage &);

    std::string to_string();

};

#endif //TWEEFILE_H
