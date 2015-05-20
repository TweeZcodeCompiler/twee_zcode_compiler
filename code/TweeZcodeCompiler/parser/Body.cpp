//
// Created by micha on 14.05.15.
//

#include "Body.h"
#include <iostream>

Body::Body(std::string c) {
    content=c;
}

std::string Body::getContent(){
    return this->content;
}
