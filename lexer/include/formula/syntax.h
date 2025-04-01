#pragma once

namespace formula
{

enum class Syntax : int
{
    NONE = 0,
    COMMENT = 1,
    KEYWORD = 2,
};

constexpr int operator+(Syntax value)
{
    return static_cast<int>(value);
}

} // namespace formula
