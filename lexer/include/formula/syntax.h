#pragma once

namespace formula
{

enum class Syntax : int
{
    DEFAULT = 0,
    COMMENT = 1,
};

inline int operator+(Syntax value)
{
    return static_cast<int>(value);
}

} // namespace formula
