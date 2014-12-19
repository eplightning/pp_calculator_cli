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
    : m_bracket(bracket)
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

unsigned int OperatorToken::accuracy() const
{
    return 0;
}

unsigned int OperatorToken::precision() const
{
    return 0;
}

TokenType OperatorToken::type() const
{
    return TokenType::op;
}

//
// Operator Token (with precision/accuracy)
//

OperatorPrecisionToken::OperatorPrecisionToken(char op, const std::string &precision)
    : OperatorToken(op)
{
    // niestety klasa od liczb sprowadza zapisy typu 0.1000 na 0.1 więc nie możemy tego użyć do określania precyzji ... :/
    // liczymy ile max. cyfr nas interesuje
    unsigned int max = std::numeric_limits<unsigned int>::max();
    int digits = 0;

    while (max >= 10) {
        max /= 10;
        digits++;
    }

    int base = 1;
    int currentDigits = 0;
    unsigned int current = 0;

    // precyzja (lub dokładność)
    auto it = precision.crbegin();

    for (; it != precision.crend() && *it != '.' && *it != ','; it++) {
        if (*it == '-' || *it == '+')
            continue;

        if (*it < '0' || *it > '9')
            throw Exception("Tokenizer: Unknown character found while reading precision/accuracy");

        if (currentDigits < digits) {
            current += base * ((*it) - '0');
            base *= 10;
            currentDigits++;
        }
    }

    if (it == precision.crend()) {
        m_accuracy = current;
        m_precision = 0;

        return;
    }

    m_precision = current;

    current = 0;
    currentDigits = 0;
    base = 1;

    // dokładność
    for (it++; it != precision.crend(); it++) {
        if (*it == '-' || *it == '+')
            continue;

        if (*it < '0' || *it > '9')
            throw Exception("Tokenizer: Unknown character found while reading accuracy");

        if (currentDigits < digits) {
            current += base * ((*it) - '0');
            base *= 10;
            currentDigits++;
        }
    }

    m_accuracy = current;
}

unsigned int OperatorPrecisionToken::accuracy() const
{
    return m_accuracy;
}

unsigned int OperatorPrecisionToken::precision() const
{
    return m_precision;
}

//
// Number Token
//

NumberToken::NumberToken(Number *num)
    : m_num(num)
{

}

NumberToken::~NumberToken()
{
    delete m_num;
}

Number *NumberToken::num() const
{
    return m_num;
}

TokenType NumberToken::type() const
{
    return TokenType::num;
}
