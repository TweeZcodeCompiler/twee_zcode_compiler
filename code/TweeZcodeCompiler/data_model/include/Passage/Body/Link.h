//
// Created by lars on 31.05.15.
//

#ifndef LINK_H
#define LINK_H

#include "BodyPart.h"

#include <string>

class Link : public BodyPart {

private:
    std::string target;
    std::string altName;

public:
    Link(std::string);

    Link(std::string, std::string);

    std::string getTarget();

    std::string getAltName();

    std::string to_string();

};


#endif //LINK_H
