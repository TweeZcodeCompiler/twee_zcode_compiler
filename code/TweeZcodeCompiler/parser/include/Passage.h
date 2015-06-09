//
// Created by micha on 14.05.15.
//

#ifndef YACC_PASSAGE_H
#define YACC_PASSAGE_H

#include "Body.h"
#include <string>

class Passage {

private:
    Body& body;
    std::string passageName;

public:
    Passage(std::string name, Body& body1);

    Body getBody();

    std::string getPassageName();
};


#endif //YACC_PASSAGE_H
