#ifndef PARSER_H
#define PARSER_H

#include "expression.h"
#include "tokenizer/token.h"

#include <map>
#include <stack>
#include <vector>

namespace Calculator {

    class Parser {
    public:
        Parser(const std::vector<Token *> &tokens, std::map<char, int> &precedence, int defaultPrecision);

        Expression *parse();

    private:
        void cleanupStack(std::stack<Expression*> &output, std::stack<OperatorToken*> &operators) const;
        Expression *subParse(int accuracy, int precision, char endingBracket);

        const std::vector<Token*> m_tokens;
        std::map<char, int> &m_precedence;
        std::vector<Token*>::const_iterator m_it;
        int m_defaultPrecision;
    };

}

#endif // PARSER_H

