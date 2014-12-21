#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "core/number.h"

namespace Calculator {

    enum ExpressionType { exprNumber, exprOperator, exprRange };

    class Expression {
    public:
        virtual ~Expression();

        virtual Number evaluate(int accuracy = 0, int precision = 10) = 0;
        virtual ExpressionType type() const = 0;
    };

    class NumberExpression : public Expression {
    public:
        NumberExpression(const Number &num);

        Number evaluate(int accuracy = 0, int precision = 10);
        ExpressionType type() const;

    private:
        const Number &m_num;
    };

    class RangeExpression : public Expression {
    public:
        RangeExpression(Expression *left, Expression *right);
        ~RangeExpression();

        Number evaluate(int accuracy = 0, int precision = 10);
        ExpressionType type() const;

        Number left();
        Number right();

    private:
        Expression *m_left;
        Expression *m_right;
    };

    class OperatorExpression : public Expression {
    public:
        OperatorExpression(Expression *left, Expression *right, int accuracy = -1, int precision = -1);
        ~OperatorExpression();

        ExpressionType type() const;

    protected:
        Expression *m_left;
        Expression *m_right;
        int m_accuracy;
        int m_precision;
    };

    class AdditionExpression : public OperatorExpression {
    public:
        AdditionExpression(Expression *left, Expression *right, int accuracy = -1, int precision = -1);

        Number evaluate(int accuracy = 0, int precision = 10);
    };

    class SubtractionExpression : public OperatorExpression {
    public:
        SubtractionExpression(Expression *left, Expression *right, int accuracy = -1, int precision = -1);

        Number evaluate(int accuracy = 0, int precision = 10);
    };

    class MultiplicationExpression : public OperatorExpression {
    public:
        MultiplicationExpression(Expression *left, Expression *right, int accuracy = -1, int precision = -1);

        Number evaluate(int accuracy = 0, int precision = 10);
    };

    class DivisionExpression : public OperatorExpression {
    public:
        DivisionExpression(Expression *left, Expression *right, int accuracy = -1, int precision = -1);

        Number evaluate(int accuracy = 0, int precision = 10);
    };

    class PrecisionExpression : public OperatorExpression {
    public:
        PrecisionExpression(Expression *left, Expression *right, int accuracy = -1, int precision = -1);

        Number evaluate(int accuracy = 0, int precision = 10);
    };

    class IndexExpression : public OperatorExpression {
    public:
        IndexExpression(Expression *left, Expression *right, int accuracy = -1, int precision = -1);

        Number evaluate(int accuracy = 0, int precision = 10);
    };

}

#endif // EXPRESSION_H

