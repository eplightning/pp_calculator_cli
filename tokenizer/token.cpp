#include "token.h"
#include "core/exception.h"
#include "core/number.h"

#include <limits>
#include <string>

using namespace Calculator;

//
// Token
//

Token::~Token()
{

}

//
// Bracket Token
//

BracketToken::BracketToken(char bracket)
    : OperatorToken(bracket), m_bracket(bracket)
{

}

char BracketToken::bracket() const
{
    return m_bracket;
}

bool BracketToken::matching(const BracketToken &other) const
{
    if (m_bracket == '(')
        return other.m_bracket == ')';

    if (m_bracket == ')')
        return other.m_bracket == '(';

    if (m_bracket == '[')
        return other.m_bracket == ']';

    if (m_bracket == ']')
        return other.m_bracket == '[';

    if (m_bracket == '{')
        return other.m_bracket == '}';

    if (m_bracket == '}')
        return other.m_bracket == '{';

    return false;
}

TokenType BracketToken::type() const
{
    return (m_bracket == '[' || m_bracket == '{' || m_bracket == '(') ? TokenType::bracketOpen : TokenType::bracketEnd;
}

//
// Operator Token
//

OperatorToken::OperatorToken(char op)
    : m_op(op)
{

}

char OperatorToken::op() const
{
    return m_op;
}

int OperatorToken::accuracy() const
{
    return -1;
}

int OperatorToken::precision() const
{
    return -1;
}

TokenType OperatorToken::type() const
{
    return TokenType::op;
}

//
// Operator Token (with precision/accuracy)
//

OperatorPrecisionToken::OperatorPrecisionToken(char op, const Number &precision)
    : OperatorToken(op)
{
    m_precision = precision.asInteger(false);
    m_accuracy  = precision.asInteger(true);

    if (m_precision == 0)
        m_precision = -1;
}

int OperatorPrecisionToken::accuracy() const
{
    return m_accuracy;
}

int OperatorPrecisionToken::precision() const
{
    return m_precision;
}

//
// Number Token
//

NumberToken::NumberToken(const std::string &num)
    : m_num(num)
{

}

const Number &NumberToken::num() const
{
    return m_num;
}

TokenType NumberToken::type() const
{
    return TokenType::num;
}
