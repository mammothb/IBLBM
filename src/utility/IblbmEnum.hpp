#ifndef SRC_UTILITY_IBLBMENUM_HPP_
#define SRC_UTILITY_IBLBMENUM_HPP_

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
}  // namespace iblbm
#endif  // SRC_UTILITY_IBLBMENUM_HPP_
