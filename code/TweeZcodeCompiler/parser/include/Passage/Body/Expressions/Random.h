
#ifndef RANDOM_H
#define RANDOM_H

#include "Expression.h"

#include <string>
#include <memory>

class Random : public Expression {

private:
    std::unique_ptr<Expression> start;
    std::unique_ptr<Expression> end;

public:

    Random(const Expression *, const Expression *);

    Random(const Expression &, const Expression &);

    Random(const Random &random) {
        this->start.reset(random.getStart()->clone());
        this->end.reset(random.getEnd()->clone());
    }

    const std::unique_ptr<Expression> &getStart() const;

    const std::unique_ptr<Expression> &getEnd() const;

    std::string to_string() const;

    virtual Random *clone() const {
        return new Random(*this);
    }
};

#endif //RANDOM_H
