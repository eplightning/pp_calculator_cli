#ifndef TOKEN_H
#define TOKEN_H

#include "core/number.h"

#include <string>

namespace Calculator {

    enum TokenType { num, op, bracketOpen, bracketEnd };

    class Token {
    public:
        virtual ~Token();

        virtual TokenType type() const = 0;
    };

    class OperatorToken : public Token {
    public:
        OperatorToken(char op);

        char op() const;
        virtual int accuracy() const;
        virtual int precision() const;

        TokenType type() const;

    private:
        char m_op;
    };

    class BracketToken : public OperatorToken {
    public:
        BracketToken(char bracket);

        char bracket() const;

        bool matching(const BracketToken &other) const;

        TokenType type() const;

    private:
        char m_bracket;
    };

    class OperatorPrecisionToken : public OperatorToken {
    public:
        OperatorPrecisionToken(char op, const Number &precision);

        int accuracy() const;
        int precision() const;

    private:
        int m_accuracy;
        int m_precision;
    };

    class NumberToken : public Token {
    public:
        NumberToken(const std::string &num);

        const Number &num() const;

        TokenType type() const;

    private:
        Number m_num;
    };

}

#endif // TOKEN_H

