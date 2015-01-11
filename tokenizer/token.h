#ifndef TOKEN_H
#define TOKEN_H

#include "core/number.h"

#include <string>

namespace Calculator {

    enum TokenType { num, op, bracketOpen, bracketEnd };

    class Token {
    public:
        virtual ~Token();

        virtual int position() const = 0;
        virtual TokenType type() const = 0;
    };

    class OperatorToken : public Token {
    public:
        OperatorToken(char op, int position);

        char op() const;
        virtual int accuracy() const;
        virtual int precision() const;

        int position() const;
        TokenType type() const;

    private:
        char m_op;
        int m_position;
    };

    class BracketToken : public OperatorToken {
    public:
        BracketToken(char bracket, int position);

        char bracket() const;

        bool matching(const BracketToken &other) const;

        TokenType type() const;

    private:
        char m_bracket;
    };

    class OperatorPrecisionToken : public OperatorToken {
    public:
        OperatorPrecisionToken(char op, const Number &precision, int position);

        int accuracy() const;
        int precision() const;

    private:
        int m_accuracy;
        int m_precision;
    };

    class NumberToken : public Token {
    public:
        NumberToken(const std::string &num, int position);

        const Number &num() const;

        int position() const;
        TokenType type() const;

    private:
        Number m_num;
        int m_position;
    };

}

#endif // TOKEN_H

