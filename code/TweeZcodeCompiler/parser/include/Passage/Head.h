//
// Created by lars on 05.06.15.
//

#ifndef HEAD_H
#define HEAD_H

#include <vector>
#include <string>

class Head {

private:
    std::string name;
    std::vector <std::string> tags;

public:
    Head();

    Head(std::string);

    Head(std::string, std::vector <std::string>);

    std::string getName();

    std::vector <std::string> getTags();

    std::string to_string();

};

#endif //HEAD_H