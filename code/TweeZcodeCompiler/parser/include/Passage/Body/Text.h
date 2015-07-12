//
// Created by lars on 31.05.15.
//

#ifndef TEXT_H
#define TEXT_H

#include "BodyPart.h"
#include "BodyPartsVisitor.h"

#include <string>

class Text : public BodyPart {

private:
    std::string content;

public:
    virtual Text* clone() const {
        return new Text(*this);
    }

    Text(std::string content);

    std::string getContent() const;

    std::string to_string() const;

    void accept(BodyPartsVisitor&);

};


#endif //TEXT_H
