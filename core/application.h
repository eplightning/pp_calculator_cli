#ifndef APPLICATION_H
#define APPLICATION_H

#include "number.h"

#include <map>
#include <string>

namespace Calculator {

    class Application {
    public:
        Application(bool verbose = true);

        int execute();

    private:
        Number calculate(const std::string &input, int precision = 10);

        std::map<char, int> m_precedence;
        bool m_verbose;
    };



}

#endif // APPLICATION_H
