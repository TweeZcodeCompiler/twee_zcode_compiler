//
// Created by tobias on 6/27/15.
//

#ifndef PROJECT_VARIABLE_H
#define PROJECT_VARIABLE_H

#include "BodyPart.h"
#include <string>

class Variable : public BodyPart {

private:
    std::string name;

public:
    virtual Variable* clone() const {
        return new Variable(*this);
    }

    Variable(std::string name);

    std::string getName();

    std::string to_string();

};


#endif //PROJECT_VARIABLE_H
