#include "application.h"
#include "number.h"

#include <exception>
#include <iostream>
#include <string>

using namespace Calculator;

Application::Application(bool verbose) :
    m_verbose(verbose)
{
}

Number Application::calculate(const std::string &input, int precision) const {
    Number num = input;
    Number num2 = 3;

    Number result = num / num2;

    return result;
    // tokenizer
    // syntax check
    // parser
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
