#ifndef SRC_UTILITY_IBLBMUTILITIES_HPP_
#define SRC_UTILITY_IBLBMUTILITIES_HPP_

#include <stdexcept>

namespace iblbm
{
namespace util
{
/**
 * Does dot product between 2 vectors of equal length
 * \param r_vec_1 first vector
 * \param r_vec_2 second vector
 *
 * \return dot product of r_vec_1 and r_vec_2
 */
template <typename T>
T InnerProduct(
    const std::vector<T> &r_vec_1
  , const std::vector<T> &r_vec_2)
{
  if (r_vec_1.size() != r_vec_2.size()) {
    throw std::runtime_error("Size mismatch");
  }
  T result = 0.0;
  auto it_elem_2 = begin(r_vec_2);
  for (auto it_elem_1 : r_vec_1) result += it_elem_1 * (*it_elem_2++);
  return result;
}
}  // namespace util
}  // namespace iblbm
#endif  // SRC_UTILITY_IBLBMUTILITIES_HPP_
