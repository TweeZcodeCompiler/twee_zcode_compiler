//
// Created by lars on 31.05.15.
//

#ifndef PASSAGEHEAD_H
#define PASSAGEHEAD_H

#include "../PassageElement.h"

#include <string>

class PassageHead : public TweeElement {

private:
    std::string name;

public:
    PassageHead(std::string);

    std::string getName();

    std::string to_string();

    std::string to_ZASS();

};


#endif //PASSAGEHEAD_H
