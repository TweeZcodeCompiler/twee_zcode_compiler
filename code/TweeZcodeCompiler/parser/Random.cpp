#include "include/Passage/Body/Expressions/Random.h"

Random::Random(const Expression &start, const Expression &end) {
    this->start = std::unique_ptr<Expression>(start.clone());
    this->end = std::unique_ptr<Expression>(end.clone());
}

Random::Random(const Expression *start, const Expression *end) {
    this->start = std::unique_ptr<Expression>(start->clone());
    this->end = std::unique_ptr<Expression>(end->clone());
}

const std::unique_ptr<Expression> &Random::getStart() const {
    return this->start;
}

const std::unique_ptr<Expression> &Random::getEnd() const {
    return this->end;
}

std::string Random::to_string() const {
    return "Random(" + this->getStart()->to_string() + "," + this->getEnd()->to_string()+")";
}