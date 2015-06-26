//
// Created by lars on 26.06.15.
//

#ifndef ASSIGNMENTOP_H
#define ASSIGNMENTOP_H

#include "Operator.h"

#include <string>
#include <memory>


class AssignmentOp : public Operator {

public:

    AssignmentOp(AssignmentOperation, std::unique_ptr<Expression> &, std::unique_ptr<Expression> &);

    AssignmentOperation getOperator() const;

    const std::unique_ptr<Expression> &getLeftSide() const;

    const std::unique_ptr<Expression> &getRightSide() const;

    virtual AssignmentOp *clone() const {
        return new AssignmentOp(*this);
    }

    std::string to_string() const;

};

#endif //ASSIGNMENTOP_H
