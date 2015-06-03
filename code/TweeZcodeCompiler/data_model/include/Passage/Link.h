//
// Created by lars on 31.05.15.
//

#ifndef LINK_H
#define LINK_H

#include "../PassageElement.h"
#include "Passage.h"

#include <string>

class Link : public PassageElement {

private:
    Passage &target;
    std::string altName;

public:
    Link(Passage);

    Link(Passage, std::string);

    std::string to_string();

};


#endif //LINK_H
