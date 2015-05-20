//
// Created by micha on 14.05.15.
//

#ifndef YACC_BODY_H
#define YACC_BODY_H

#include <string>

class Body {

private:
    std::string content;

public:
    Body(std::string c);

    std::string getContent();

    Body& operator+=(const std::string& right);
};


#endif //YACC_BODY_H
