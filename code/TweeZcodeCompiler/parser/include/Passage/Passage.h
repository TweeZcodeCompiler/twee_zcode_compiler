//
// Created by lars on 31.05.15.
//

#ifndef PASSAGE_H
#define PASSAGE_H

#include "Head.h"
#include "Body.h"


class Passage {
private:
    Head head;
    Body body;

public:
    Passage(Head&, Body&);
    const Head& getHead() const;
    const Body& getBody() const;
    std::string to_string() const;
};


#endif //PASSAGE_H
