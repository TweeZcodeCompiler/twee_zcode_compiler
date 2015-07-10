
#include "include/Passage/Body/Expressions/Visited.h"


Visited::Visited() { }


const std::vector<std::string>& Visited::getPassages() const {
    return this->passages;
}

const std::string Visited::getPassage(size_t pos) const {
    return passages.at(pos);
}

size_t Visited::getPassageCount() {
    return passages.size();
}

Visited& Visited::operator+=(std::string passageName) {
    this->passages.push_back(passageName);
    return *this;
}

std::string Visited::to_string() const {
    std::string result = "Visited: ";

    for (auto iter = this->passages.begin(); iter != this->passages.end(); ++iter)
        result +=  std::string(*iter) + " ";

    return result;
}
