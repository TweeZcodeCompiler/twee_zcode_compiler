//
// Created by lars on 31.05.15.
//

#ifndef PASSAGEBODY_H
#define PASSAGEBODY_H


#include "../PassageElement.h"

#include <vector>
#include <string>


class PassageBody : public TweeElement {

private:
    std::vector <PassageElement> passageElements;

public:

    PassageBody();

    void addElement(PassageElement);

    PassageBody operator+=(const PassageElement &);

    std::string to_string();

    std::string to_ZASS();

};


#endif //PASSAGEBODY_H
