#include "token.h"
#include "tokenizer.h"
#include "core/exception.h"

#include <string>
#include <vector>

using namespace Calculator;

Tokenizer::Tokenizer(const std::string &input) :
    m_input(input)
{

}

std::vector<Token*> Tokenizer::tokenize()
{
    // pełny reset stanu
    resetState();
    m_it = m_input.cbegin();
    m_it2 = m_it;
    m_last = 0;

    std::vector<Token*> list;

    try {
        while (m_it != m_input.cend()) {
            // jeśli znak, i to jest odpowiednie miejsce dla znaku (za niczym, za operatorem albo za otwarciem nawiasa)
            if (isSign() && (!m_last || (m_last->type() == TokenType::op || m_state_operator) || m_last->type() == TokenType::bracketOpen)) {
                // koniec liczby odkryty, kończymy token i pętla od tego samego miejsca
                if (m_state_numDigitRead) {
                    terminalRead(list);
                    resetState();
                    continue;
                }

                // podwójnie razy znak czytany
                if (m_state_numSign)
                    throw Exception("Tokenizer: Multiple number signs are not allowed");

                // przesuwamy wskaźnik na liczbe jeśli czytamy precyzje
                if (m_state_optOperator) {
                    m_it2 = m_it;
                // jeśli natomiast mamy nie dodany jeszcze do tokenów operator, to robimy to
                } else if (m_state_operator) {
                    terminalRead(list);
                    m_state_operator = 0;
                }

                m_state_numSign = true;

                // znak nie ma prawa być po przecinkach i kropkach
                if (m_state_dots || m_state_numDecimals)
                    throw Exception("Tokenizer: Decimal seperators before sign was read");
            // biały znak lub nawias
            } else if (isWhitespace()) {
                terminalRead(list);
                resetState();
                m_it2 = m_it + 1;
            } else if (isBracket()) {
                terminalRead(list);
                resetState();
                appendBracket(*m_it, list);
                m_it2 = m_it + 1;
            } else if (isOptionalOperator()) {
                if (m_state_optOperator)
                    throw Exception("Tokenizer: Multiple precision operators found for single normal operator");

                // koniec liczby odkryty, kończymy token i pętla od tego samego miejsca
                if (m_state_numDigitRead) {
                    terminalRead(list);
                    resetState();
                    continue;
                }

                if (!m_state_operator) {
                    // operator precyzji można również użyć jako zwykły operator
                    // kończymy konstrukcje operatora już tutaj jako ,że nie chcemy czegoś takiego 5 ::10 5 bo to sensu nie ma
                    m_state_operator = *m_it;
                    terminalRead(list);
                    resetState();
                    m_it2 = m_it + 1;
                } else {
                    m_state_optOperator = *m_it;
                }
            } else if (isOperator()) {
                // koniec liczby odkryty, kończymy token i pętla od tego samego miejsca
                if (m_state_numDigitRead) {
                    terminalRead(list);
                    resetState();
                    continue;
                }

                m_state_operator = *m_it;
            } else if (isDigit()) {
                if (m_state_operator) {
                    if (!m_state_optOperator) {
                        // koniec zwykłego operatora
                        terminalRead(list);
                        resetState();
                        m_it2 = m_it;
                    } else if (!m_state_numDigitRead && !m_state_numSign) {
                        // tworzymy operator z precyzją więc przesuwamy pointer na liczbe żeby nie odczytać znaków operatorów
                        m_it2 = m_it;
                    }
                }

                m_state_numDigitRead = true;
                m_state_numSign = true;

                // resetujemy kropki ,żeby gdzieś nie wkradł się operator range'a
                m_state_dots = 0;
            } else if (isDot() && m_state_dots == 1) {
                ++m_state_dots; // żeby nie wykryło jako osierocone kropki
                terminalRead(list);
                resetState();
                appendRangeOperator(list);
                m_it2 = m_it + 1;
            } else if (isDecimalSeperator()) {
                // koniec liczby odkryty, kończymy token i pętla od tego samego miejsca
                // żeby 5.12.. rozpoznało jako liczbe 5.12 pomimo znaków seperatora na końcu
                if (m_state_numDecimals >= 1 && m_state_numDigitRead) {
                    terminalRead(list);
                    resetState();
                    continue;
                }

                if (isDot())
                    ++m_state_dots;

                m_state_numDecimals++;
            } else {
                throw Exception("Tokenizer: Unknown character");
            }

            m_it++;
        }
    } catch (...) {
        for (auto *x: list) {
            delete x;
        }

        throw;
    }

    // end of file tak jakby
    terminalRead(list);

    return list;
}

void Tokenizer::resetState()
{
    m_state_dots = 0;
    m_state_numDecimals = 0;
    m_state_numDigitRead = false;
    m_state_numSign = false;
    m_state_operator = 0;
    m_state_optOperator = 0;
}

void Tokenizer::terminalRead(std::vector<Token*> &list)
{
    // whitespace
    if (!m_state_operator && !m_state_optOperator && !m_state_numDigitRead &&
        !m_state_numSign  && !m_state_numDecimals) {
        return;
    }

    // dziwne stany
    if (m_state_numSign && !m_state_numDigitRead) {
        throw Exception("Tokenizer: Sign without following numbers");
    } else if (m_state_numDecimals > 1) {
        throw Exception("Tokenizer: More than two decimal seperators detected");
    }

    Token *tok = 0;

    // operatory
    if (m_state_operator) {
        if (m_state_optOperator) {
            if (!m_state_numDigitRead) {
                throw Exception("Tokenizer: Operator with precision without number");
            }

            tok = new OperatorPrecisionToken(m_state_operator, std::string(m_it2, m_it));
            list.push_back(tok);
        } else {
            tok = new OperatorToken(m_state_operator);
            list.push_back(tok);
        }
    } else if (m_state_numDigitRead) {
        tok = new NumberToken(std::string(m_it2, m_it));
        list.push_back(tok);
    }

    if (tok) {
        m_last = tok;
    } else if (m_state_dots == 1 || (m_state_dots == 0 && m_state_numDecimals > 0)) {
        throw Exception("Tokenizer: Orphaned dot or comma (probably at the end)");
    }
}

void Tokenizer::appendBracket(char bracket, std::vector<Token *> &list)
{
    m_last = new BracketToken(bracket);
    list.push_back(m_last);
}

void Tokenizer::appendRangeOperator(std::vector<Token *> &list)
{
    m_last = new OperatorToken('.');
    list.push_back(m_last);
}

bool Tokenizer::isBracket() const
{
    return *m_it == '[' || *m_it == ']' || *m_it == '(' || *m_it == ')';
}

bool Tokenizer::isDot() const
{
    return *m_it == '.';
}

bool Tokenizer::isDecimalSeperator() const
{
    return *m_it == '.' || *m_it == ',';
}

bool Tokenizer::isDigit() const
{
    return *m_it >= '0' && *m_it <= '9';
}

bool Tokenizer::isOperator() const
{
    return *m_it == '+' || *m_it == '-' || *m_it == '*' || *m_it == '/' || *m_it == '~';
}

bool Tokenizer::isOptionalOperator() const
{
    return *m_it == ':';
}

bool Tokenizer::isSign() const
{
    return *m_it == '+' || *m_it == '-';
}

bool Tokenizer::isWhitespace() const
{
    return *m_it == ' ' || *m_it == '\n' || *m_it == '\r';
}
