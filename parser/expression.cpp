#include "expression.h"
#include "core/exception.h"
#include "core/number.h"
#include "core/range.h"

using namespace Calculator;

//
// Expression
//

Expression::~Expression()
{

}

//
// Expression Number
//

NumberExpression::NumberExpression(const Number &num) :
    m_num(num)
{

}

Number NumberExpression::evaluate(int accuracy, int precision)
{
    Number out = m_num;

    out.setAccuracy(accuracy);
    out.setPrecision(precision);

    return out;
}

ExpressionType NumberExpression::type() const
{
    return ExpressionType::exprNumber;
}

//
// Expression Range
//

RangeExpression::RangeExpression(Expression *left, Expression *right) :
    m_left(left), m_right(right)
{

}

RangeExpression::~RangeExpression()
{
    delete m_left;
    delete m_right;
}

Number RangeExpression::evaluate(int, int)
{
    throw Exception("Parser: Range cannot be evaluated (can only be used as index)");

    return 0;
}

ExpressionType RangeExpression::type() const
{
    return ExpressionType::exprRange;
}

Number RangeExpression::left()
{
    return m_left->evaluate();
}

Number RangeExpression::right()
{
    return m_right->evaluate();
}

//
// Expression Operator
//

OperatorExpression::OperatorExpression(Expression *left, Expression *right, int accuracy, int precision) :
    m_left(left), m_right(right), m_accuracy(accuracy), m_precision(precision)
{

}

OperatorExpression::~OperatorExpression()
{
    delete m_left;
    delete m_right;
}

ExpressionType OperatorExpression::type() const
{
    return ExpressionType::exprOperator;
}

//
// Expression Add
//

AdditionExpression::AdditionExpression(Expression *left, Expression *right, int accuracy, int precision) :
    OperatorExpression(left, right, accuracy, precision)
{

}

Number AdditionExpression::evaluate(int accuracy, int precision)
{
    if (m_accuracy > -1)
        accuracy = m_accuracy;

    if (m_precision > 0)
        precision = m_precision;

    Number out = m_left->evaluate(accuracy, precision);

    out += m_right->evaluate(accuracy, precision);

    return out;
}

//
// Expression Minus
//

SubtractionExpression::SubtractionExpression(Expression *left, Expression *right, int accuracy, int precision) :
    OperatorExpression(left, right, accuracy, precision)
{

}

Number SubtractionExpression::evaluate(int accuracy, int precision)
{
    if (m_accuracy > -1)
        accuracy = m_accuracy;

    if (m_precision > 0)
        precision = m_precision;

    Number out = m_left->evaluate(accuracy, precision);

    out -= m_right->evaluate(accuracy, precision);

    return out;
}

//
// Expression *
//

MultiplicationExpression::MultiplicationExpression(Expression *left, Expression *right, int accuracy, int precision) :
    OperatorExpression(left, right, accuracy, precision)
{

}

Number MultiplicationExpression::evaluate(int accuracy, int precision)
{
    if (m_accuracy > -1)
        accuracy = m_accuracy;

    if (m_precision > 0)
        precision = m_precision;

    Number out = m_left->evaluate(accuracy, precision);

    out *= m_right->evaluate(accuracy, precision);

    return out;
}

//
// Expression /
//

DivisionExpression::DivisionExpression(Expression *left, Expression *right, int accuracy, int precision) :
    OperatorExpression(left, right, accuracy, precision)
{

}

Number DivisionExpression::evaluate(int accuracy, int precision)
{
    if (m_accuracy > -1)
        accuracy = m_accuracy;

    if (m_precision > 0)
        precision = m_precision;

    Number out = m_left->evaluate(accuracy, precision);

    out /= m_right->evaluate(accuracy, precision);

    return out;
}

//
// Expression ~
//

SquareExpression::SquareExpression(Expression *left, Expression *right, int accuracy, int precision) :
    OperatorExpression(left, right, accuracy, precision)
{

}

Number SquareExpression::evaluate(int accuracy, int precision)
{
    if (m_accuracy > -1)
        accuracy = m_accuracy;

    if (m_precision > 0)
        precision = m_precision;

    Number out = m_left->evaluate(accuracy, precision);

    return out.squareEstimation(m_right->evaluate(accuracy, precision));
}

//
// Expression :
//

PrecisionExpression::PrecisionExpression(Expression *left, Expression *right, int accuracy, int precision) :
    OperatorExpression(left, right, accuracy, precision)
{

}

Number PrecisionExpression::evaluate(int, int precision)
{
    Number right = m_right->evaluate(0, 10);

    int precision2 = right.asInteger(false);

    if (precision2 <= 0)
        precision2 = precision;

    return m_left->evaluate(right.asInteger(true), precision2);
}

//
// Expression []
//

IndexExpression::IndexExpression(Expression *left, Expression *right, int accuracy, int precision) :
    OperatorExpression(left, right, accuracy, precision)
{

}

Number IndexExpression::evaluate(int accuracy, int precision)
{
    Number left = m_left->evaluate(accuracy, precision);

    if (m_right->type() == ExpressionType::exprRange) {
        RangeExpression *rangeExpr = static_cast<RangeExpression*>(m_right);

        Number leftRange = rangeExpr->left();
        Number rightRange = rangeExpr->right();

        int leftRangeInt = leftRange.asInteger(true);
        int rightRangeInt = rightRange.asInteger(true);

        if (leftRange.isNegative())
            leftRangeInt *= -1;

        if (rightRange.isNegative())
            rightRangeInt *= -1;

        return left[Range(leftRangeInt, rightRangeInt)];
    }

    Number right = m_right->evaluate(accuracy, precision);

    int rightInt = right.asInteger(true);

    if (right.isNegative())
        rightInt *= -1;

    return left[rightInt];
}
