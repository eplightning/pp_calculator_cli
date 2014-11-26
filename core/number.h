#ifndef NUMBER_H
#define NUMBER_H

#include <ostream>
#include <string>
#include <vector>

namespace Calculator {

    class Number {
    public:
        // konstruktory
        Number();
        Number(const std::string &input);
        Number(int input);
        Number(double input);

        // ustawianie prec/dokł operacji
        Number &setAccuracy(int accuracy);
        Number &setPrecision(int precision);

        // ustawianie znaku
        Number &setSign(char sign);

        // operatory działań
        Number &operator+=(const Number &right);
        const Number operator+(const Number &right) const;

        // operatory porównań
        bool operator==(const Number &other) const;
        bool operator!=(const Number &other) const;
        bool operator>(const Number &other) const;
        bool operator>=(const Number &other) const;
        bool operator<(const Number &other) const;
        bool operator<=(const Number &other) const;

        // operator dla strumieni
        friend std::ostream &operator<<(std::ostream &stream, const Number &number);

    private:
        void add(const Number &right, bool ignoreSign = false);
        void subtract(const Number &right, bool ignoreSign = false);

        int compareWith(const Number &other, bool ignoreSign = false) const;

        void fromString(const std::string &input);
        std::string toString() const;

        void normalize();
        void removeLeadingZeros();
        void removeTrailingZeros();

        // cyfry, miejsce przecinka, -/+
        std::vector<char> m_digits;
        unsigned int m_decimals;
        bool m_negative;

        // precyzja i dokładność operacji
        unsigned int m_precision;
        unsigned int m_accuracy;
    };

    std::ostream &operator<<(std::ostream &stream, const Number &number);
}

#endif // NUMBER_H
