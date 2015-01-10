#ifndef NUMBER_H
#define NUMBER_H

#include "range.h"

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

        // ustawianie/pobieranie znaku
        bool isNegative() const;
        Number &setSign(char sign);

        // liczba odwrotna
        Number inverse() const;

        // pierwiastek (przybliżenie)
        Number squareEstimation(Number iterations) const;

        // zwraca (a przynajmniej stara się zwrócić) część całkowitą lub po przecinku jako int
        unsigned long long asInteger(bool integers = true) const;

        // operatory indexów
        Number operator[](int index) const;
        Number operator[](const Range &range) const;

        // operatory działań
        Number &operator+=(const Number &right);
        Number operator+(const Number &right) const;
        Number &operator-=(const Number &right);
        Number operator-(const Number &right) const;
        Number &operator*=(const Number &right);
        Number operator*(const Number &right) const;
        Number &operator/=(const Number &right);
        Number operator/(const Number &right) const;

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
        // implementacja działań matem.
        void add(const Number &right, bool ignoreSign = false);
        void subtract(const Number &right, bool ignoreSign = false);
        void multiply(const Number &right);

        // pierwsze przybliżenie pierwiastka
        Number squareSeed() const;

        // porównywarka liczb
        int compareWith(const Number &other, bool ignoreSign = false) const;

        // pomocnicze do wczytywania/zapisywania z/do string
        void fromString(const std::string &input);
        std::string toString() const;

        // sprawdzanie czy zero
        bool isNull() const;

        // pomocnicze do utrzymywania liczb w sensownej postaci
        int normalize();
        void removeLeadingZeros();
        int removeTrailingZeros();

        // przesuwanie przecinka w lewo i prawo
        void shift(int distance);

        std::vector<char> m_digits;
        int m_decimals;
        bool m_negative;
        int m_precision;
        int m_accuracy;
        int m_trailingZeros;
    };

    std::ostream &operator<<(std::ostream &stream, const Calculator::Number &number);
}

#endif // NUMBER_H
