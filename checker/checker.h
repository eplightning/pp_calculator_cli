#ifndef CHECKER_H
#define CHECKER_H

#include "tokenizer/token.h"

#include <vector>

namespace Calculator {

    class SyntaxChecker {
    public:
        SyntaxChecker(const std::vector<Token*> &tokens);

        void check();

    private:
        bool insideBracket(char bracket);

        const std::vector<Token*> &m_tokens;
        std::vector<BracketToken*> m_brackets;
    };

}

#endif // CHECKER_H

