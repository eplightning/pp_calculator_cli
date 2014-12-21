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
        Parser(const std::vector<Token *> &tokens, const std::map<char, int> &precedence);

        Expression *parse();

    private:
        void cleanupStack(std::stack<Expression*> &output, std::stack<OperatorToken*> &operators) const;
        Expression *subParse(char endingBracket);

        const std::vector<Token*> &m_tokens;
        const std::map<char, int> &m_precedence;
        std::vector<Token*>::const_iterator m_it;
    };

}

#endif // PARSER_H

