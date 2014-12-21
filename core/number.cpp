#include "number.h"
#include "exception.h"

#include <deque>
#include <limits>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>

using namespace Calculator;

std::ostream &Calculator::operator<<(std::ostream &stream, const Number &number)
{
    std::string out = number.toString();

    stream << out;

    return stream;
}

Number::Number() :
    m_digits(1, 0), m_decimals(0), m_negative(false), m_precision(10), m_accuracy(0), m_trailingZeros(0)
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

Number Number::operator+(const Number &right) const
{
    Number result = *this;

    result += right;

    return result;
}

Number &Number::operator-=(const Number &right)
{
    subtract(right);

    return *this;
}

Number Number::operator-(const Number &right) const
{
    Number result = *this;

    result -= right;

    return result;
}

Number &Number::operator*=(const Number &right)
{
    multiply(right);

    return *this;
}

Number Number::operator*(const Number &right) const
{
    Number result = *this;

    result *= right;

    return result;
}

Number &Number::operator/=(const Number &right)
{
    // 0 / x = 0
    if (isNull())
        return *this;

    // a / a = a
    if (*this == right) {
        m_digits.clear();
        m_digits.push_back(1);
        m_decimals = 0;
        m_negative = (m_negative != right.m_negative);
        return *this;
    }

    // a / b = a * (1/b)
    multiply(right.inverse());

    return *this;
}

Number Number::operator/(const Number &right) const
{
    Number result = *this;

    result /= right;

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

Number &Number::setAccuracy(int accuracy)
{
    m_accuracy = accuracy;
    return *this;
}

Number &Number::setPrecision(int precision)
{
    m_precision = precision >= 0 ? precision : 10;
    return *this;
}

bool Number::isNegative() const
{
    return m_negative;
}

Number &Number::setSign(char sign)
{
    m_negative = (sign == '-');
    return *this;
}

Number Number::operator[](int index) const
{
    if (index > 0) {
        index = index + m_decimals - 1;
    } else if (index < 0) {
        index = m_decimals + index;
    } else {
        throw Exception("Number[0] operation is undefined (numbers are indexed 1..n)");
    }

    Number out;

    out.m_accuracy = m_accuracy;
    out.m_precision = m_precision;

    if (index >= 0 && m_digits.size() > static_cast<size_t>(index)) {
        out.m_digits.clear();
        out.m_digits.push_back(m_digits[index]);
    }

    return out;
}

Number Number::operator[](const Range &range) const
{
    int left = range.left();
    int right = range.right();

    if (left == 0 || right == 0)
        throw Exception("Number[0] operation is undefined (numbers are indexed 1..n)");

    if (left < right)
        throw Exception("Number[x..y] operation where x < y is undefined");

    int lefti;
    int righti;

    if (left > 0) {
        lefti = left + m_decimals - 1;
    } else {
        lefti = m_decimals + left;
    }

    if (right > 0) {
        righti = right + m_decimals - 1;
    } else {
        righti = m_decimals + right;
    }

    if ((lefti < 0 || righti < 0) ||
        (m_digits.size() <= static_cast<size_t>(lefti) || m_digits.size() <= static_cast<size_t>(righti)))
        throw Exception("Number[x..y] out of bounds");

    Number out;

    out.m_accuracy = m_accuracy;
    out.m_precision = m_precision;
    out.m_digits.clear();

    out.m_digits.insert(out.m_digits.begin(), m_digits.cbegin() + righti, m_digits.cbegin() + lefti + 1);

    if (left > 0 && right < 0) {
        out.m_decimals = right * (-1);
    }

    out.m_trailingZeros = out.normalize();

    return out;
}

unsigned long long Number::asInteger(bool integers) const
{
    if (!integers && m_decimals == 0)
        return 0;

    auto start = integers ? m_digits.cbegin() + m_decimals : m_digits.cbegin();
    auto end   = integers ? m_digits.cend() : m_digits.cbegin() + m_decimals;

    // liczymy ile cyfr max
    unsigned long long max = std::numeric_limits<unsigned long long>::max();
    int digits = 0;

    while (max >= 10) {
        max /= 10;
        digits++;
    }

    unsigned long long out = 0;

    int base = 1;

    if (!integers) {
        for (int i = 0; digits > 0 && i < m_trailingZeros; i++) {
            base *= 10;
            digits--;
        }

        if (digits == 0) {
            return base / 10;
        }
    }

    for (; digits > 0 && start != end; --digits, base *= 10, ++start) {
        out += base * (*start);
    }

    return out;
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

    // dokładność
    if (m_accuracy > 0) {
        // todo: sprawdzić czy nie ma ładniejszego sposobu (jakiś range iterator?)
        int zeros = m_digits.size() - m_decimals - m_accuracy;

        for (lit = m_digits.begin() + m_decimals; zeros > 0; lit++, zeros--) {
            *lit = 0;
        }
    }

    // zer na końcu nie chcemy (po przecinku)
    removeTrailingZeros();
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

    int cmp = compareWith(right, true);

    Number *bignum;
    const Number *smallnum;

    if (cmp == 0) {
        // po prostu zero
        m_digits.clear();
        m_digits.push_back(0);
        m_negative = false;
        m_decimals = 0;
        return;
    } else if (cmp < 0) {
        bignum = this;
        smallnum = &right;
    } else {
        bignum = new Number(right);
        smallnum = this;

        bignum->m_negative = !m_negative; // 4 - 6  = -(6 - 4)
    }

    auto li = bignum->m_digits.begin();
    auto ri = smallnum->m_digits.cbegin();

    // paddingi jeśli liczba mniejsza ma więcej liczb po przecinku
    int ninePadding = 0;
    int tenPadding = 0;

    // carry
    bool carry = false;

    if (bignum->m_decimals > smallnum->m_decimals) {
        li += (bignum->m_decimals - smallnum->m_decimals);
    } else if (bignum->m_decimals < smallnum->m_decimals) {
        while (*li == 0) {
            *li = 9;
            li++;

            if (li == bignum->m_digits.end())
                throw Exception("Can't find digit greater than 0 ?!");
        }

        --(*li);

        li = bignum->m_digits.begin();

        bignum->m_decimals += smallnum->m_decimals - bignum->m_decimals;
        ninePadding = smallnum->m_decimals - bignum->m_decimals - 1;
        tenPadding = 1;
    }

    for (; ri != smallnum->m_digits.cend(); ri++, li++) {
        if (li == bignum->m_digits.end())
            throw Exception("Bigger number finished earlier than smaller");

        if (tenPadding > 0) {
            li = bignum->m_digits.insert(li, 10 - *ri);
            tenPadding--;
            continue;
        }

        if (ninePadding > 0) {
            li = bignum->m_digits.insert(li, 9 - *ri);
            // przesunąć iterator
            ninePadding--;
            continue;
        }

        int result = *li;

        if (carry) {
            --result;
            carry = false;
        }

        result -= *ri;

        if (result < 0) {
            carry = true;
            result += 10;

            if (result < 0)
                throw Exception("Digit smaller than 0");
        }

        *li = result;
    }

    if (carry) {
        if (li == bignum->m_digits.end() || *li <= 0)
            throw Exception("Cannot subtract last carry");

        --(*li);
    }

    // jeśli robiliśmy na kopii to musimy teraz skopiować wynik i posprzątać
    if (cmp > 0) {
        bignum->m_accuracy = m_accuracy;
        bignum->m_precision = m_precision;

        *this = *bignum;
        delete bignum;
    }

    // precyzja
    if (m_decimals > m_precision) {
        m_digits.erase(m_digits.begin(), m_digits.begin() + (m_decimals - m_precision));
        m_decimals = m_precision;
    }

    // dokładność
    if (m_accuracy > 0) {
        // todo: sprawdzić czy nie ma ładniejszego sposobu (jakiś range iterator?)
        int zeros = m_digits.size() - m_decimals - m_accuracy;

        for (li = m_digits.begin() + m_decimals; zeros > 0; li++, zeros--) {
            *li = 0;
        }
    }

    // czyścimy
    normalize();
}

void Number::multiply(const Number &right)
{
    Number out;
    int shift = 0;

    // każda cyfra mnożnika
    for (auto rit = right.m_digits.cbegin(); rit != right.m_digits.cend(); rit++, shift++) {
        if (*rit == 0)
            continue;

        int carry = 0;
        Number partial;
        partial.m_digits.clear();

        // każda cyfra liczby mnożonej
        for (auto lit = m_digits.begin(); lit != m_digits.end(); lit++) {
            int result = ((*lit) * (*rit)) + carry;

            if (result > 90)
                throw Exception("Digit greater than 9");

            partial.m_digits.push_back(result % 10);
            carry = result / 10;
        }

        if (carry > 0)
            partial.m_digits.push_back(carry);

        partial.shift(shift);

        out += partial;
    }

    // znak liczby
    out.m_negative = (m_negative != right.m_negative);

    // przecinek
    out.m_decimals = m_decimals + right.m_decimals;

    if (out.m_digits.size() < static_cast<size_t>(out.m_decimals)) {
        for (int i = out.m_decimals - out.m_digits.size() + 1; i > 0; i--)
            out.m_digits.push_back(0);
    }

    // kopiujemy wynik
    out.m_accuracy = m_accuracy;
    out.m_precision = m_precision;
    *this = out;

    // precyzja
    if (m_decimals > m_precision) {
        m_digits.erase(m_digits.begin(), m_digits.begin() + (m_decimals - m_precision));
        m_decimals = m_precision;
    }

    // dokładność
    if (m_accuracy > 0) {
        // todo: sprawdzić czy nie ma ładniejszego sposobu (jakiś range iterator?)
        int zeros = m_digits.size() - m_decimals - m_accuracy;

        for (auto li = m_digits.begin() + m_decimals; zeros > 0; li++, zeros--) {
            *li = 0;
        }
    }

    // sprzątanie
    normalize();
}

Number Number::inverse() const
{
    // dzielenie przez 0
    if (isNull())
        throw Exception("Divison by 0 detected");

    // wynikowa liczba
    Number result;
    result.m_digits.clear();
    result.m_precision = m_precision;
    result.m_accuracy = m_accuracy;
    result.m_negative = m_negative;

    // dzieląca liczba
    Number integer = *this;

    if (m_decimals > 0) {
        integer.shift(m_decimals);
    }

    // szukanie odwrotności 0.(dowolna ilość zer)1, lub jedynki
    if (integer.m_digits.size() == 1 && integer.m_digits.front() == 1) {
        result.m_digits.push_back(1);
        result.shift(m_decimals);
        return result;
    }

    // tymczasowa
    Number tmp(10);

    // jako że wrzucamy odwrotnie ...
    std::deque<char> numbers;

    int digit = 0;

    do {
        int cmp = tmp.compareWith(integer, true);

        if (cmp <= 0) {
            ++digit;
            tmp -= integer;
        } else {
            numbers.push_front(digit);
            result.m_decimals++;
            digit = 0;
            tmp.shift(1);
        }
    } while(result.m_decimals <= m_precision && !tmp.isNull());

    if (digit > 0) {
        numbers.push_front(digit);
        result.m_decimals++;
    }

    // teraz dodajemy do liczby
    result.m_digits.insert(result.m_digits.begin(), numbers.begin(), numbers.end());
    result.m_digits.push_back(0);

    // 1 / 0,321 = 1000 * (1 / 321)
    result.shift(m_decimals);

    // dokładność
    if (m_accuracy > 0) {
        // todo: sprawdzić czy nie ma ładniejszego sposobu (jakiś range iterator?)
        int zeros = result.m_digits.size() - result.m_decimals - m_accuracy;

        for (auto li = result.m_digits.begin() + result.m_decimals; zeros > 0; li++, zeros--) {
            *li = 0;
        }
    }

    // sprzątanie
    result.normalize();

    return result;
}

int Number::compareWith(const Number &other, bool ignoreSign) const
{
    // znaki
    if (!ignoreSign && m_negative != other.m_negative)
        return m_negative ? 1 : -1;

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

    m_trailingZeros = normalize();

    m_precision = m_decimals > 9 ? m_decimals : 10;
}

bool Number::isNull() const
{
    return m_digits.size() == 1 && m_digits.front() == 0;
}

int Number::normalize()
{
    // przypadek: ,555 -> 0,555, puste lub , -> 0
    if (m_digits.empty() || m_digits.size() == static_cast<size_t>(m_decimals)) {
        m_digits.push_back(0);
    } else {
        removeLeadingZeros();
    }

    int trailing = 0;

    if (m_decimals > 0) {
        trailing = removeTrailingZeros();
    }

    // ujemne zero
    if (m_negative && isNull())
        m_negative = false;

    return trailing;
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

int Number::removeTrailingZeros()
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

    return oldDecimals;
}

void Number::shift(int distance)
{
    // zera nie można
    if (isNull() || !distance)
        return;

    // w prawo
    if (distance > 0) {
        int decimals = m_decimals - distance;

        for(; decimals < 0; decimals++) {
            m_digits.insert(m_digits.begin(), 0);
        }

        m_decimals = decimals;

        removeLeadingZeros();
    } else {
        m_decimals -= distance;

        while (m_digits.size() <= static_cast<size_t>(m_decimals)) {
            m_digits.push_back(0);
        }

        removeTrailingZeros();
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
