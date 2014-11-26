#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

namespace Calculator {

    class Exception : public std::exception {
    public:
        explicit Exception();
        explicit Exception(std::string message);
        virtual ~Exception() throw() { }

        virtual const char *what() const throw();

    private:
        std::string m_message;
    };
}

#endif // EXCEPTION_H
