#ifndef RANGE_H
#define RANGE_H

namespace Calculator {

    class Range {
    public:
        Range(int left, int right) :
            m_left(left), m_right(right) { }

        int left() const { return m_left; }
        int right() const { return m_right; }

    private:
        int m_left;
        int m_right;
    };

}

#endif // RANGE_H

