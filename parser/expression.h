#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "core/number.h"

namespace Calculator {

    class Expression {
    public:
        virtual ~Expression();

        virtual Number evaluate() = 0;
    };

    class NumberExpression {
    public:
        NumberExpression(const Number &num);

        Number evaluate();

    private:
        const Number &m_num;
    };

    class OperatorExpression {
    public:
        OperatorExpression(Expression &left, Expression &right);

    protected:
        Expression &m_left;
        Expression &m_right;
    };

}

#endif // EXPRESSION_H

