#include "exception.h"

#include <string>

using namespace Calculator;

Exception::Exception() :
    m_message("Unknown exception")
{

}

Exception::Exception(std::string message) :
    m_message(message)
{

}

const char *Exception::what() const throw() {
    return m_message.c_str();
}
