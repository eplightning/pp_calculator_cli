#ifndef APPLICATION_H
#define APPLICATION_H

#include "number.h"

#include <string>

namespace Calculator {

    class Application {
    public:
        Application(bool verbose = true);

        int execute() const;

    private:
        Number calculate(const std::string &input, int precision = 10) const;

        bool m_verbose;
    };



}

#endif // APPLICATION_H
