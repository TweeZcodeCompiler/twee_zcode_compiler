//
// Created by lars on 31.05.15.
//

#ifndef PASSAGEELEMENT_H
#define PASSAGEELEMENT_H

#include <string>

class PassageElement : public TweeElement {

public:
    std::string to_string();

    std::string to_ZASS();

};

#endif //PASSAGEELEMENT_H