#pragma once
#include <cstdlib>

namespace util
{
class CircularInt
{
  public:
    CircularInt(size_t modula, size_t value) : modula(modula), value(value)
    {
    }

    CircularInt(size_t modula) : modula(modula), value(0)
    {
    }

    ~CircularInt() = default;

    CircularInt operator+(int v)
    {
        return CircularInt(modula, (value + v) % modula);
    }

    CircularInt operator-(int v)
    {
        return CircularInt(modula, (value - v) % modula);
    }

    CircularInt operator*(int v)
    {
        return CircularInt(modula, (value * v) % modula);
    }

    CircularInt operator/(int v)
    {
        return CircularInt(modula, (value / v) % modula);
    }

    CircularInt operator++(int v)
    {
        return CircularInt(modula, (value++) % modula);
    }

    CircularInt operator--(int v)
    {
        return CircularInt(modula, (value--) % modula);
    }

  private:
    size_t modula;
    size_t value;
};
} // namespace util