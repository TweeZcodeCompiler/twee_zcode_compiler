//
// Created by lars on 26.06.15.
//

#ifndef RELATIONOP_H
#define RELATIONOP_H

#include "Operator.h"

#include <string>
#include <memory>


class RelationOp : public Operator {

public:

    RelationOp(RelationOperation, std::unique_ptr<Expression> &, std::unique_ptr<Expression> &);

    RelationOperation getOperator() const;

    const std::unique_ptr<Expression> &getLeftSide() const;

    const std::unique_ptr<Expression> &getRightSide() const;

    virtual RelationOp *clone() const {
        return new RelationOp(*this);
    }

    std::string to_string() const;

};

#endif //RELATIONOP_H
