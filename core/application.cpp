#include "application.h"
#include "number.h"
#include "range.h"
#include "checker/checker.h"
#include "parser/expression.h"
#include "parser/parser.h"
#include "tokenizer/token.h"
#include "tokenizer/tokenizer.h"

#include <exception>
#include <iostream>
#include <string>
#include <map>

using namespace Calculator;

Application::Application(bool verbose) :
    m_verbose(verbose)
{
}

Number Application::calculate(const std::string &input, int precision) const
{
    // tokenizujemy
    Tokenizer tok(input);
    std::vector<Token*> tokens = tok.tokenize();

    Expression *expr = 0;

    try {
        // sprawdzamy sens
        SyntaxChecker checker(tokens);
        checker.check();

        // parser
        Parser parser(tokens, m_precedence);
        expr = parser.parse();
    } catch (...) {
        // usuwanie tokenów
        for (Token *tok : tokens) {
            delete tok;
        }

        throw;
    }

    // rekurencyjnie liczymy rozwiązanie
    Number out;

    try {
        out = expr->evaluate(0, precision);
    } catch (...) {
        // usuwanie wyrażeń
        delete expr;

        // usuwanie tokenów
        for (Token *tok : tokens) {
            delete tok;
        }

        throw;
    }

    // usuwanie wyrażeń
    delete expr;

    // usuwanie tokenów
    for (Token *tok : tokens) {
        delete tok;
    }

    return out;
}

int Application::execute()
{
    // pierwszeństwo operatorów
    m_precedence['.'] = 5;
    m_precedence['+'] = 4;
    m_precedence['-'] = 4;
    m_precedence[':'] = 3;
    m_precedence['/'] = 2;
    m_precedence['*'] = 2;
    m_precedence['['] = 1;
    m_precedence['~'] = 0;

    std::string input;

    if (m_verbose)
        std::cout << "Kalkulator, podaj wyrazenie do obliczenia lub exit, aby wyjsc" << std::endl;

    while (!std::cin.fail() && !std::cin.eof()) {
        if (m_verbose)
            std::cout << "> ";

        do {
            std::getline(std::cin, input);
        } while(input.empty());

        if (input == "exit")
            break;

        try {
            Number out = this->calculate(input, 10);

            if (m_verbose)
                std::cout << "= ";

            std::cout << out;
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        } catch(...) {
            std::cerr << "Unknown exception" << std::endl;
        }

        std::cout << std::endl;
    }

    return 0;
}
