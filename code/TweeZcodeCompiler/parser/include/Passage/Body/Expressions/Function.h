//
// Created by lars on 24.06.15.
//

#ifndef FUNCTION_H
#define FUNCTION_H

#include "Expression.h"
#include "Variable.h"

#include <string>
#include <memory>
#include <vector>

class Function : public Expression {

private:
    FunctionName functionName;
    std::vector<std::unique_ptr<Variable>> arguments;

public:

    Function(FunctionName, std::vector<std::unique_ptr<Variable>>);

    FunctionName getFunctionName() const;

    const std::vector<std::unique_ptr<Variable>> &getArguments() const;

    std::string to_string();

    virtual Function *clone() const {
        return new Function(*this);
    }

};

#endif //FUNCTION_H
