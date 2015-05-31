//
// Created by lars on 31.05.15.
//

#ifndef TEXT_H
#define TEXT_H

#include "../PassageElement.h"
#include <string>

class Text : public PassageElement {

private:
    std::string content;

public:
    Text(std::string);

    std::string to_string();

    std::string to_ZASS();

};


#endif //TEXT_H
