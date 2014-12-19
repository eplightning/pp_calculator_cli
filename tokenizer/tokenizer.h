#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "core/number.h"
#include "token.h"

#include <string>
#include <vector>

namespace Calculator {

    class Tokenizer {
    public:
        Tokenizer(const std::string &input);

        std::vector<Token*> tokenize();
    protected:
        // resetuje stan
        void resetState();

        // dodajemy token na podstawie stanu obecnego
        void terminalRead(std::vector<Token*> &list);

        // dodaje pare prostych tokenów
        void appendBracket(char bracket, std::vector<Token*> &list);
        void appendRangeOperator(std::vector<Token*> &list);

        // typ znaku
        bool isBracket() const;
        bool isDecimalSeperator() const;
        bool isDigit() const;
        bool isDot() const;
        bool isOperator() const;
        bool isOptionalOperator() const;
        bool isSign() const;
        bool isWhitespace() const;

        // wejscie, iteratorki, ostatni token
        const std::string &m_input;
        std::string::const_iterator m_it;
        std::string::const_iterator m_it2;

        // stany
        char  m_state_operator;
        char  m_state_optOperator;
        bool  m_state_numSign;
        bool  m_state_numDigitRead;
        int   m_state_numDecimals;
        int   m_state_dots;
        Token *m_last;
    };

}

#endif // TOKENIZER_H

