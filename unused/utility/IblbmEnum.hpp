#ifndef SRC_UTILITY_IBLBMENUM_HPP_
#define SRC_UTILITY_IBLBMENUM_HPP_

#include <iostream>

namespace iblbm
{
/**
 * Enum for discrete velocity directions in the D2Q9 model
 * 6  2  5 ^
 *  \ | /  |
 * 3--0--1 |
 *  / | \  |
 * 7  4  8 +--->
 */
enum Directions {
    E = 1,
    N,
    W,
    S,
    NE,
    NW,
    SW,
    SE
};

enum class Edge : int {
    NOT_EDGE = -1,
    RIGHT,
    UPPER,
    LEFT,
    LOWER,
    UPPER_RIGHT,
    UPPER_LEFT,
    LOWER_LEFT,
    LOWER_RIGHT
};

std::ostream& operator<<(std::ostream& rOs, const Edge edge);
}  // namespace iblbm
#endif  // SRC_UTILITY_IBLBMENUM_HPP_
