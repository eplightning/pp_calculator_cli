#include "parser.h"
#include "expression.h"
#include "core/exception.h"
#include "tokenizer/token.h"

#include <stack>
#include <vector>

using namespace Calculator;

Parser::Parser(const std::vector<Token*> &tokens, const std::map<char, int> &precedence) :
    m_tokens(tokens), m_precedence(precedence)
{

}

Expression *Parser::parse()
{
    m_it = m_tokens.cbegin();

    return subParse(0);
}

Expression *Parser::subParse(char endingBracket)
{
    std::stack<Expression*> output;
    std::stack<OperatorToken*> operators;

    try {
        while (m_it != m_tokens.cend()) {
            TokenType type = (*m_it)->type();

            if (type == TokenType::bracketOpen) {
                BracketToken *br = static_cast<BracketToken*>(*m_it);

                if (br->bracket() == '[') {
                    if (!operators.empty() && (m_precedence.find('['))->second > (m_precedence.find(operators.top()->op()))->second) {
                        cleanupStack(output, operators);
                    }

                    operators.push(br);
                    m_it++;
                    output.push(subParse(']'));
                } else if (br->bracket() == '(') {
                    m_it++;
                    output.push(subParse(')'));
                } else {
                    throw Exception("Parser: Invalid bracket");
                }

                continue;
            } else if (type == TokenType::bracketEnd) {
                BracketToken *bracket = static_cast<BracketToken*>(*m_it);

                if (!endingBracket) {
                    throw Exception("Parser: Unexpected bracket end");
                } else if (bracket->bracket() == endingBracket) {
                    m_it++;
                    break;
                } else {
                    throw Exception("Parser: Invalid ending of a bracket found");
                }
            } else if (type == TokenType::num) {
                NumberToken *number = static_cast<NumberToken*>(*m_it);

                output.push(new NumberExpression(number->num()));
            } else {
                OperatorToken *opt = static_cast<OperatorToken*>(*m_it);

                if (!operators.empty() && (m_precedence.find(opt->op()))->second > (m_precedence.find(operators.top()->op()))->second) {
                    cleanupStack(output, operators);
                }

                operators.push(opt);
            }

            m_it++;
        }


        if (!operators.empty()) {
            cleanupStack(output, operators);
        }

        if (output.size() == 0)
            throw Exception("Parser: No expressions could be read");

        if (output.size() > 1)
            throw Exception("Parser: More than single expression after processing");
    } catch (...) {
        while (!output.empty()) {
            delete output.top();
            output.pop();
        }

        throw;
    }

    return output.top();
}

void Parser::cleanupStack(std::stack<Expression*> &output, std::stack<OperatorToken*> &operators) const
{
    while (!operators.empty()) {
        OperatorToken *op = operators.top();
        operators.pop();

        if (output.size() < 2)
            throw Exception("Parser: Didn't find 2 expressions in stack for operator");

        Expression *right = output.top();
        output.pop();

        Expression *left = output.top();
        output.pop();

        switch (op->op()) {
            case '+':
                output.push(new AdditionExpression(left, right, op->accuracy(), op->precision()));
                break;

            case '-':
                output.push(new SubtractionExpression(left, right, op->accuracy(), op->precision()));
                break;

            case '*':
                output.push(new MultiplicationExpression(left, right, op->accuracy(), op->precision()));
                break;

            case '/':
                output.push(new DivisionExpression(left, right, op->accuracy(), op->precision()));
                break;

            case ':':
                output.push(new PrecisionExpression(left, right, op->accuracy(), op->precision()));
                break;

            case '[':
                output.push(new IndexExpression(left, right, op->accuracy(), op->precision()));
                break;

            default:
                output.push(new RangeExpression(left, right));
                break;
        }
    }
}
