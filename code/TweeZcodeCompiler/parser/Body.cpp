//
// Created by micha on 14.05.15.
//

#include "Body.h"
#include <iostream>

Body::Body(std::string c) {
    content=c;
}

std::string Body::getContent() const {
    return this->content;
}

Body& Body::operator+=(const std::string& right) {
    this->content += right;
    return *this;
}

Body& Body::operator+=(const Body& right) {
    this->operator+=(right.getContent());
    return *this;
}
