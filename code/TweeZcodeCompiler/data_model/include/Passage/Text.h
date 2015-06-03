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

    void addText(std::string);

    std::string to_string();

};


#endif //TEXT_H
