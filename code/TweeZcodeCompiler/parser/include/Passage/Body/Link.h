//
// Created by lars on 31.05.15.
//

#ifndef LINK_H
#define LINK_H

#include "BodyPart.h"
#include "IBodyPartsVisitor.h"

#include <string>

class Link : public BodyPart {

private:
    std::string target;
    std::string altName;

public:
    Link(std::string);

    Link(std::string, std::string);

    std::string getTarget() const;

    std::string getAltName() const;

    std::string to_string() const;

    virtual Link* clone() const {
        return new Link(*this);
    }

    void accept(IBodyPartsVisitor&) const;
};


#endif //LINK_H
