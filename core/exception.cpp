#include "exception.h"

#include <string>
#include <sstream>

using namespace Calculator;

Exception::Exception() :
    m_message("Unknown exception")
{

}

Exception::Exception(std::string message) :
    m_message(message)
{

}

Exception::Exception(const std::string &message, int chr) :
    m_message()
{
    std::stringstream str;

    str << message;
    str << " [char #";
    str << chr + 1;
    str << ']';

    m_message = str.str();
}

const char *Exception::what() const throw() {
    return m_message.c_str();
}
