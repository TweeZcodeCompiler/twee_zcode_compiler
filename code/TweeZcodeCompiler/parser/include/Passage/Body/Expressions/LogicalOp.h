//
// Created by lars on 26.06.15.
//

#ifndef LOGICALOP_H
#define LOGICALOP_H

#include "Operator.h"

#include <string>
#include <memory>


class LogicalOp : public Operator {

public:

    LogicalOp(LogicalOperation, std::unique_ptr<Expression> &, std::unique_ptr<Expression> &);

    LogicalOperation getOperator() const;

    const std::unique_ptr<Expression> &getLeftSide() const;

    const std::unique_ptr<Expression> &getRightSide() const;

    virtual LogicalOp *clone() const {
        return new LogicalOp(*this);
    }

    std::string to_string() const;

};

#endif //LOGICALOP_H
