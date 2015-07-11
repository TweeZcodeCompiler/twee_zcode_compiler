//
// Created by lars on 31.05.15.
//

#ifndef LINK_H
#define LINK_H

#include "BodyPart.h"
#include "../../AssemblyGeneratorVisitor.h"

#include <string>

class Link : public BodyPart {

private:
    std::string target;
    std::string altName;

public:
    Link(std::string);

    Link(std::string, std::string);

    virtual Link* clone() const {
        return new Link(*this);
    }

    std::string getTarget() const;

    std::string getAltName() const;

    std::string to_string() const;

    void accept(AssemblyGeneratorVisitor) const;

};


#endif //LINK_H
