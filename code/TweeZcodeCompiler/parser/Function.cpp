//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Expressions/Function.h"

Function::Function(FunctionName functionName, std::vector<std::unique_ptr<Variable>> arguments) {

    this->functionName = functionName;
    this->arguments = arguments;
}

FunctionName Function::getFunctionName() const {
    return this->functionName;
}

const std::vector<std::unique_ptr<Variable>> &Function::getArguments() const {
    return this->arguments;
}

std::string Function::to_string() {

    std::string result = "Function: " + this->getFunctionName() + "\n  Arguments: ";

    for (auto iter = arguments.begin(); iter != arguments.end(); ++iter)
        result += "    " + (*iter)->to_string();

    return result;


}