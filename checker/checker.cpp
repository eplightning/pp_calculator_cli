#include "checker.h"
#include "core/exception.h"
#include "tokenizer/token.h"

#include <stack>
#include <vector>

using namespace Calculator;

SyntaxChecker::SyntaxChecker(const std::vector<Token*> &tokens) :
    m_tokens(tokens)
{

}

void SyntaxChecker::check()
{
    if (m_tokens.size() == 0)
        return;

    for (auto it = m_tokens.cbegin(); it != m_tokens.cend(); it++) {
        // sprawdzanie nawiasów
        if ((*it)->type() == TokenType::bracketEnd) {
            if (m_brackets.empty())
                throw Exception("Syntax checker: Bracket end for not existent bracket open");

            if (!m_brackets.back()->matching(*static_cast<BracketToken*>(*it)))
                throw Exception("Syntax checker: Mismatching bracket end");

            m_brackets.pop_back();
        } else if ((*it)->type() == TokenType::bracketOpen) {
            m_brackets.push_back(static_cast<BracketToken*>(*it));
        }

        // operator zasięgu
        if ((*it)->type() == TokenType::op && static_cast<OperatorToken*>(*it)->op() == '.'
            && !insideBracket('[')) {
            throw Exception("Syntax checker: Range operator not inside index operator");
        }

        // sprawdzanie wyr. op wyr.
        if (it != m_tokens.cbegin()) {
            Token *prevToken = *(it - 1);

            if ((*it)->type() == TokenType::num && (prevToken->type() == TokenType::bracketEnd
                                                   || prevToken->type() == TokenType::num)) {
                throw Exception("Syntax checker: Number after bracket end or another number");
            } else if ((*it)->type() == TokenType::op && (prevToken->type() == TokenType::bracketOpen
                                                          || prevToken->type() == TokenType::op)) {
                throw Exception("Syntax checker: Operator after bracket open or another operator");
            }
        }
    }

    if (!m_brackets.empty())
        throw Exception("Syntax checker: One or more open brackets without matching end");
}

bool SyntaxChecker::insideBracket(char bracket)
{
    for (BracketToken *x : m_brackets) {
        if (x->bracket() == bracket)
            return true;
    }

    return false;
}
