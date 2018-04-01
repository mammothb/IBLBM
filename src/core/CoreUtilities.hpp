#ifndef SRC_CORE_COREUTILITIES_HPP_
#define SRC_CORE_COREUTILITIES_HPP_

#include <limits>
#include <numeric>
#include <vector>

#include "IblbmDebug.hpp"

namespace iblbm
{
namespace util
{
/**
 * Does dot product between 2 vectors of equal length
 * \param rVec1 first vector
 * \param rVec2 second vector
 *
 * \return dot product of rVec1 and rVec2
 */
template <typename T>
T InnerProduct(
    const std::vector<T> &rVec1
  , const std::vector<T> &rVec2)
{
  IBLBM_PRECONDITION(rVec1.size() == rVec2.size());
  return std::inner_product(rVec1.begin(), rVec1.end(), rVec2.begin(), T{});
}

/**
 * return true if number is close to zero
 *
 * \param rNumber to be tested
 */
template <class T>
inline bool IsNearZero(const T& rNumber)
{
  auto epsilon {std::numeric_limits<T>::epsilon()};
  return (rNumber > -epsilon && rNumber < epsilon);
}
}  // namespace util
}  // namespace iblbm

#endif  // SRC_CORE_COREUTILITIES_HPP_