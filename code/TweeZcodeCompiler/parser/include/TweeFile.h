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
    std::vector<Passage> passages;

public:
    TweeFile();
    ~TweeFile();

    const std::vector<Passage> &getPassages() const;

    TweeFile &operator+=(const Passage &);

    std::string to_string() const;
};

#endif //TWEEFILE_H
