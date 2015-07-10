
#ifndef VISITED_H
#define VISITED_H

#include "Expression.h"

#include <string>
#include <vector>
#include <memory>

class Visited : public Expression {

private:
    std::vector<std::string> passages;

public:

    Visited();

    size_t getPassageCount();

    const std::vector<std::string>& getPassages() const;

    const std::string getPassage(size_t) const;

    Visited& operator+=(std::string);

    std::string to_string() const;

    virtual Visited *clone() const {
        return new Visited(*this);
    }

};

#endif //VISITED_H
