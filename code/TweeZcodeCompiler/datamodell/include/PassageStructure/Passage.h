//
// Created by lars on 31.05.15.
//

#ifndef PASSAGE_H
#define PASSAGE_H

#include "../TweeElement.h"
#include "PassageHead.h"
#include "PassageBody.h"

#include <string>

class Passage : public TweeElement {

private:
    PassageHead &head;
    PassageBody &body;

public:
    Passage(PassageHead &, PassageBody &);

    std::string to_string();

    std::string to_ZASS();

};


#endif //PASSAGE_H
