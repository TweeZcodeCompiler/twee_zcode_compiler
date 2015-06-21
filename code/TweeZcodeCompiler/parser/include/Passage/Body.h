//
// Created by lars on 05.06.15.
//

#ifndef BODY_H
#define BODY_H

#include "Body/BodyPart.h"

#include <vector>
#include <memory>
#include <string>
#include <algorithm>

class Body {
private:
    std::vector<std::unique_ptr<BodyPart>> bodyparts;


public:

    Body() {}

    ~Body() {}

    Body(const Body& that) {
        copyParts(that);
    };

    Body(const Body&& that) {
        copyParts(that);
    }

    Body& operator=(const Body& that) {
        copyParts(that);
        return *this;
    };

    Body& operator=(const Body&& that) {
        copyParts(that);
        return *this;
    };

    void copyParts(const Body& that) {
        for(auto it = that.bodyparts.begin(); it != that.bodyparts.end(); it++) {
            bodyparts.push_back(std::unique_ptr<BodyPart>(it->get()->clone()));
        }
    }

    const std::vector<std::unique_ptr<BodyPart>> &getBodyParts() const;

    Body& operator+=(BodyPart&);

    std::string to_string();
};

#endif //BODY_H
