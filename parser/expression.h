#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "core/number.h"

namespace Calculator {

    enum ExpressionType { num, op, range };

    class Expression {
    public:
        virtual ~Expression();

        virtual Number evaluate() = 0;
        virtual ExpressionType type() const = 0;
    };

    class NumberExpression : public Expression {
    public:
        NumberExpression(const Number &num);

        Number evaluate();
        ExpressionType type() const;

    private:
        const Number &m_num;
    };

    class RangeExpression : public Expression {
    public:
        RangeExpression(Expression &left, Expression &right);

        Number evaluate();
        ExpressionType type() const;

        Number left();
        Number right();

    private:
        Expression &m_left;
        Expression &m_right;
    };

    class OperatorExpression : public Expression {
    public:
        OperatorExpression(Expression &left, Expression &right, int accuracy = -1, int precision = -1);

        ExpressionType type() const;

    protected:
        Expression &m_left;
        Expression &m_right;
        int m_accuracy;
        int m_precision;
    };

    class AdditionExpression : public OperatorExpression {
    public:
        AdditionExpression(Expression &left, Expression &right, int accuracy = -1, int precision = -1);

        Number evaluate();
    };

    class SubtractionExpression : public OperatorExpression {
    public:
        SubtractionExpression(Expression &left, Expression &right, int accuracy = -1, int precision = -1);

        Number evaluate();
    };

    class MultiplicationExpression : public OperatorExpression {
    public:
        MultiplicationExpression(Expression &left, Expression &right, int accuracy = -1, int precision = -1);

        Number evaluate();
    };

    class DivisionExpression : public OperatorExpression {
    public:
        DivisionExpression(Expression &left, Expression &right, int accuracy = -1, int precision = -1);

        Number evaluate();
    };

    class PrecisionExpression : public OperatorExpression {
    public:
        PrecisionExpression(Expression &left, Expression &right, int accuracy = -1, int precision = -1);

        Number evaluate();
    };

    class IndexExpression : public OperatorExpression {
    public:
        IndexExpression(Expression &left, Expression &right, int accuracy = -1, int precision = -1);

        Number evaluate();
    };

}

#endif // EXPRESSION_H

