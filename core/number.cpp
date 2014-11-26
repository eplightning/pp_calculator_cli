#include "number.h"
#include "exception.h"

#include <ostream>
#include <string>
#include <sstream>
#include <vector>

namespace Calculator {

Number::Number() :
    m_digits(1, 0), m_decimals(0), m_negative(false), m_precision(10), m_accuracy(0)
{

}

Number::Number(const std::string &input) :
    m_digits(), m_accuracy(0)
{
    fromString(input);
}

Number::Number(double input) :
    m_digits(), m_accuracy(0)
{
    std::stringstream str;

    str << input;

    fromString(str.str());
}

Number::Number(int input) :
    m_digits(), m_accuracy(0)
{
    std::stringstream str;

    str << input;

    fromString(str.str());
}

Number &Number::operator+=(const Number &right)
{
    add(right);

    return *this;
}

const Number Number::operator+(const Number &right) const
{
    Number result = *this;

    result += right;

    return result;
}

bool Number::operator==(const Number &other) const
{
    return compareWith(other) == 0;
}

bool Number::operator!=(const Number &other) const
{
    return compareWith(other) != 0;
}

bool Number::operator>(const Number &other) const
{
    return compareWith(other) == -1;
}

bool Number::operator>=(const Number &other) const
{
    return compareWith(other) != 1;
}

bool Number::operator<(const Number &other) const
{
    return compareWith(other) == 1;
}

bool Number::operator<=(const Number &other) const
{
    return compareWith(other) != -1;
}

std::ostream &operator<<(std::ostream &stream, const Number &number)
{
    std::string out = number.toString();

    stream << out;

    return stream;
}

Number &Number::setAccuracy(int accuracy)
{
    m_accuracy = accuracy;
    return *this;
}

Number &Number::setPrecision(int precision)
{
    m_precision = precision;
    return *this;
}

Number &Number::setSign(char sign)
{
    m_negative = (sign == '-');
    return *this;
}

void Number::add(const Number &right, bool ignoreSign)
{
    if (!ignoreSign) {
        // dodatnia + ujemna -> dodatnia - dodatnia
        if (!m_negative && right.m_negative) {
            return subtract(right, true);
        // ujemna + dodatnia -> -5 +6 = -(-(-5) -6)
        } else if (m_negative && !right.m_negative) {
            m_negative = false;
            subtract(right, true);
            m_negative = !m_negative;
            return;
        }
    }

    // dodatnia + dodatnia, ujemna + ujemna

    std::vector<char>::iterator lit = m_digits.begin();
    std::vector<char>::const_iterator rit = right.m_digits.cbegin();

    if (m_decimals > right.m_decimals) {
        lit += m_decimals - right.m_decimals;
    } else if (right.m_decimals > m_decimals) {
        rit += right.m_decimals - m_decimals;
    }

    // bit przeniesienia i bit "czy już doszliśmy końca lewej liczby)
    bool carry = false;
    bool endOfLeft = false;

    while (rit != right.m_digits.cend()) {
        // doszliśmy już do końca lewej (nas) liczby?
        if (!endOfLeft && lit == m_digits.end()) {
            endOfLeft = true;
        }

        // wykonujemy dodawanie na tej cyfrze
        char value = *rit;

        if (!endOfLeft)
            value += *lit;

        if (carry) {
            carry = false;
            value += 1;
        }

        // jeśli >= 10 to ustawiamy bit przeniesienia
        if (value >= 10) {
            carry = true;
            value -= 10;

            if (value >= 10)
                throw Exception("Unexpected error: Digit greater than 9");
        }

        // ustawiamy cyfre lub dodajemy nową jeśli endOfLeft
        if (!endOfLeft) {
            *lit = value;
            lit++;
        } else {
            m_digits.push_back(value); // w tym momencie nasz iterator lewy staje się nieprawidłowy dlatego potrzeba tego endOfLeft
        }

        // następna cyfra z prawej liczby
        rit++;
    }

    // jeśli nadal mamy carry bit to dodajemy 1
    if (carry)
        m_digits.push_back(1);

    // dodajemy brakujące liczby po przecinku
    if (right.m_decimals > m_decimals) {
        m_digits.insert(m_digits.begin(), right.m_digits.cbegin(), right.m_digits.cbegin() + (right.m_decimals - m_decimals));
        m_decimals = right.m_decimals;
    }

    // precyzja
    if (m_decimals > m_precision) {
        m_digits.erase(m_digits.begin(), m_digits.begin() + (m_decimals - m_precision));
        m_decimals = m_precision;
    }

    // zer na końcu nie chcemy (po przecinku)
    removeTrailingZeros();

    // dokładność
    if (m_accuracy > 0) {
        // todo: sprawdzić czy nie ma ładniejszego sposobu (jakiś range iterator?)
        int zeros = m_digits.size() - m_decimals - m_accuracy;

        for (lit = m_digits.begin() + m_decimals; zeros > 0; lit++, zeros--) {
            *lit = 0;
        }
    }
}

void Number::subtract(const Number &right, bool ignoreSign)
{
    if (!ignoreSign) {
        // dodatnia - ujemna -> dodatnia + dodatnia
        if (!m_negative && right.m_negative) {
            return add(right, true);
        // ujemna - dodatnia -> A-B=-(-A+B)
        } else if (m_negative && !right.m_negative) {
            m_negative = false;
            add(right, true);
            m_negative = !m_negative;
            return;
        }
    }

    // ujemna - ujemna -> -5 - -6 = -(5 - 6)
    if (m_negative && right.m_negative)
        m_negative = false;

    // dodatnia - dodatnia
    // TODO

}

int Number::compareWith(const Number &other) const
{
    // najpierw ilość cyfr części całkowitej
    int inta = m_digits.size() - m_decimals;
    int intb = other.m_digits.size() - other.m_decimals;

    if (inta != intb) {
        return inta > intb ? -1 : 1;
    }

    // teraz po kolei cyfry
    auto ita = m_digits.crbegin();
    auto itb = other.m_digits.crbegin();

    while (ita != m_digits.crend()) {
        if (itb == other.m_digits.crend())
            return -1;

        if (*ita == *itb) {
            ita++;
            itb++;
        } else {
            return *ita > *itb ? -1 : 1;
        }
    }

    if (itb != other.m_digits.crend())
        return 1;

    return 0;
}

void Number::fromString(const std::string &input)
{
    m_decimals = 0;
    m_digits.clear();
    m_negative = false;

    int numbersAdded = 0;

    for (auto it = input.crbegin(); it != input.crend(); it++) {
        if (*it == '-' || *it == '+') {
            if (numbersAdded == 0)
                throw Exception("Invalid number: Sign before any digit read");

            m_negative = (*it == '-');
            break;
        }

        if (*it == '.' || *it == ',') {
            if (m_decimals != 0)
                throw Exception("Invalid number: Multiple decimal seperators found");

            m_decimals = numbersAdded;
            continue;
        }

        if (*it < '0' || *it > '9')
            throw Exception("Invalid number: Unknown character found");

        m_digits.push_back(*it - '0');
        ++numbersAdded;
    }

    normalize();

    m_precision = m_decimals > 9 ? m_decimals : 10;
}

void Number::normalize()
{
    // przypadek: ,555 -> 0,555, puste lub , -> 0
    if (m_digits.empty() || m_digits.size() == m_decimals) {
        m_digits.push_back(0);
    } else {
        removeLeadingZeros();
    }

    if (m_decimals > 0) {
        removeTrailingZeros();
    }
}

void Number::removeLeadingZeros()
{
    int integerNumbers = m_digits.size() - m_decimals - 1;

    for (auto di = m_digits.crbegin();
         integerNumbers > 0 && di != m_digits.crend() && *di == 0;
         integerNumbers--, di = m_digits.crbegin()) {
            m_digits.pop_back();
    }
}

void Number::removeTrailingZeros()
{
    int oldDecimals = m_decimals;

    // zliczamy ile zer trzeba usunąć
    for (auto di = m_digits.cbegin(); m_decimals > 0 && di != m_digits.cend() && *di == 0; di++) {
        m_decimals--;
    }

    oldDecimals -= m_decimals;

    if (oldDecimals > 0) {
        m_digits.erase(m_digits.begin(), m_digits.begin() + (oldDecimals));
    }
}

std::string Number::toString() const
{
    std::string stream;

    if (m_negative)
        stream += '-';

    int integers = m_digits.size() - m_decimals;

    for (auto digit = m_digits.crbegin(); digit != m_digits.crend(); digit++) {
        if (--integers == -1)
            stream += '.';

        stream += ('0' + *digit);
    }

    return stream;
}

} // namespace Calculator
