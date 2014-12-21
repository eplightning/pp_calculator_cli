#include "application.h"
#include "number.h"
#include "range.h"
#include "checker/checker.h"
#include "tokenizer/token.h"
#include "tokenizer/tokenizer.h"

#include <exception>
#include <iostream>
#include <string>

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

    // sprawdzamy sens
    SyntaxChecker checker(tokens);
    checker.check();

    /*for (Token *x : tokens) {
        switch (x->type()) {
            case TokenType::bracketEnd:
                std::cout << "BracketEnd " << ((BracketToken*) x)->bracket();
                break;

            case TokenType::bracketOpen:
                std::cout << "BracketOpen " << ((BracketToken*) x)->bracket();
                break;

            case TokenType::num:
                std::cout << "Num " << (((NumberToken*) x)->num());
                break;

            case TokenType::op:
                OperatorToken *y = (OperatorToken*) x;
                std::cout << "Op " << y->op() << " (" << y->precision() << " " << y->accuracy() << ")";
                break;
        }

        std::cout << std::endl;
    }*/



    // usuwanie tokenów
    for (Token *tok : tokens) {
        delete tok;
    }

    return 0;
}

int Application::execute() const
{
    std::string input;

    if (m_verbose)
        std::cout << "Kalkulator, podaj wyrazenie do obliczenia lub exit, aby wyjsc" << std::endl;

    while (true) {
        if (m_verbose)
            std::cout << "> ";

        std::getline(std::cin, input);

        if (input == "exit")
            break;

        try {
            Number out = this->calculate(input, 10);

            if (m_verbose)
                std::cout << "= ";

            std::cout << out << std::endl;
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        } catch(...) {
            std::cerr << "Unknown exception" << std::endl;
        }
    }

    return 0;
}
