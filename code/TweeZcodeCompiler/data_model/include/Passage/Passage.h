//
// Created by lars on 31.05.15.
//

#ifndef PASSAGE_H
#define PASSAGE_H

#include "../TweeElement.h"
#include "../PassageElement.h"


#include <string>

class Passage : public TweeElement {

private:
    std::string name;
    std::vector <PassageElement> elemnts;

public:
    Passage(std::string);

    void addElement(PassageElement);

    std::string to_string();

};


#endif //PASSAGE_H
