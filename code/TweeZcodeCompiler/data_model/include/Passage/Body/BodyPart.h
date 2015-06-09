//
// Created by lars on 05.06.15.
//

#ifndef BODYPART_H
#define BODYPART_H

#include <string>

class BodyPart {

public:
    BodyPart();

    ~BodyPart();
    virtual std::string to_string() = 0;

};

#endif //BODYPART_H
