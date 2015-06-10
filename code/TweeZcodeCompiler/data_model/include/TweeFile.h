//
// Created by lars on 05.06.15.
//

#ifndef TWEEFILE_H
#define TWEEFILE_H

#include "Passage/Passage.h"
#include "Passage/Body.h"
#include "Passage/Head.h"

#include <vector>
#include <string>

class TweeFile {

private:
    std::vector <Passage> passages;

public:
    TweeFile();

    std::vector <Passage> getPassages();

    TweeFile &operator+=(const Passage &);

    std::string to_string();

};

#endif //TWEEFILE_H
