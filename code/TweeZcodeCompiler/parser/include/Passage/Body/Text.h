//
// Created by lars on 31.05.15.
//

#ifndef TEXT_H
#define TEXT_H

#include "BodyPart.h"
#include "IBodyPartsVisitor.h"

#include <string>

class Text : public BodyPart {

private:
    std::string content;

public:

    Text(std::string content);

    std::string getContent() const;

    std::string to_string() const;

    virtual Text* clone() const {
        return new Text(*this);
    }

    void accept(IBodyPartsVisitor&) const;
};


#endif //TEXT_H
