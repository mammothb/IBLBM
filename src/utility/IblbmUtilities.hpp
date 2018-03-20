#ifndef SRC_UTILITY_IBLBMUTILITIES_HPP_
#define SRC_UTILITY_IBLBMUTILITIES_HPP_

#include <iomanip>  // std::setprecision
#include <iostream>
#include <stdexcept>
#include <vector>

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

/**
 * Flips the lattice into the proper orientation for printing
 *
 * \param lattice lattice stored row-wise
 * \param nx number of columns of the lattice
 *
 * \return flipped lattice
 */
template <typename T, typename U>
T Flip(
    const T& r_lattice
  , U nx)
{
  auto flipped_lattice(r_lattice);
  auto ny = r_lattice.size() / nx;
  for (auto n = 0u; n < nx * ny; ++n) {
    auto n_flipped = (ny - 1 - n / nx) * nx + n % nx;
    flipped_lattice[n_flipped] = r_lattice[n];
  }  // n
  return flipped_lattice;
}

/**
 * Reorganize lattice using new index format for easier printing
 * 0  1  2
 *  \ | /
 * 3--4--5
 *  / | \
 * 6  7  8
 *
 * \param lattice lattice stored row-wise
 *
 * \return reorganized lattice
 */
template <typename T>
T Organize(const T& r_lattice)
{
  auto organized_lattice(r_lattice);
  auto index = 0;
  for (auto& node : organized_lattice) {
    // 3 and 8 are in organized position
    node[0] = r_lattice[index][6];
    node[1] = r_lattice[index][2];
    node[2] = r_lattice[index][5];
    node[4] = r_lattice[index][0];
    node[5] = r_lattice[index][1];
    node[6] = r_lattice[index][7];
    node[7] = r_lattice[index][4];
    ++index;
  }  // node
  return organized_lattice;
}

/**
 * Print a multi-depth lattice i.e. a n-dimension vector
 *
 * \param lattice stored row-wise
 * \param nx number of columns of lattice
 * \param ny number of rows of lattice
 */
template <typename T, typename U>
void Print(
    const T& r_lattice
  , U nx
  , U ny)
{
  auto depth = r_lattice[0].size();
  // Distribution function and velocity lattices will have size nx * ny, so
  // modulo will return 0. Boundary nodes will modify depth value.
  auto code = depth + r_lattice.size() % (nx * ny);
  // 2 for depth 2, 9 for depth 9, default for boundary
  switch (code) {
    case 2: {
      auto counter = 0;
      auto flipped_lattice = Flip(r_lattice, nx);
      for (auto node : flipped_lattice) {
        std::cout << std::fixed << std::setprecision(2)
                  << node[0] << " " << node[1] << "  ";
        if (++counter % nx == 0) {
          std::cout << std::endl;
          counter = 0;
        }
      }  // lat
      std::cout << std::endl;
      break;
    }
    case 9: {
      auto lat = Flip(Organize(r_lattice), nx);
      // row of lattice
      for (auto y = 0u; y < ny; ++y) {
        // rows in the Q9 square
        for (auto i = 0u; i < depth / 3; ++i) {
          // column of lattice
          for (auto x = 0u; x < nx; ++x) {
            auto n = y * nx + x;
            auto index = i * 3;
            std::cout << std::fixed << std::setprecision(2)
                      << lat[n][index] << " "
                      << lat[n][index + 1] << " "
                      << lat[n][index + 2] << "  ";
          }  // x
          std::cout << std::endl;
        }  // i
        std::cout << std::endl;
      }  // y
      std::cout << std::endl;
      break;
    }
    default: {
      std::vector<U> length = {ny, ny, nx, nx, 4};
      auto lat = Organize(r_lattice);
      // row of boundary, print 4 lines, left, right, top, bottom and corners
      for (auto y = 0u; y < 5; ++y) {
        // rows in the Q9 square
        for (auto i = 0u; i < depth / 3; ++i) {
          // column of lattice
          for (auto x = 0u; x < length[y]; ++x) {
            auto n = 2 * ny + 2 * nx + x;
            auto index = i * 3;
            if (y == 0 || y == 1) n = y * ny + x;
            if (y == 2 || y == 3) n = 2 * ny + (y - 2) * nx + x;
            std::cout << std::fixed << std::setprecision(2)
                      << lat[n][index] << " "
                      << lat[n][index + 1] << " "
                      << lat[n][index + 2] << "  ";
          }  // x
          std::cout << std::endl;
        }  // i
        std::cout << std::endl;
      }  // y
      std::cout << std::endl;
      break;
    }
  }
}

/**
 * Compares two vectors of doubles and ensures that their elementwise values
 * are within delta-% apart
 *
 * @param r_actual_vector First vector
 * @param rExpectedVector Second vector
 * @param delta Maximum percentage by which elements in the vectors can be
 *        different
 *
 * @return TRUE if all elements in the vectors are within delta=% from each
 *         other
 *         FALSE if vectors are of different length or if the elements are
 *         not within delta-% apart
 */
template <typename VECTOR>
bool CheckCloseVector(
    const VECTOR& r_actual_vector
  , const VECTOR& r_expected_vector
  , double delta)
{
  // Check that the two array are of equal length
  auto ret = r_actual_vector.size() == r_expected_vector.size();
  if (ret) {
    for (auto i = 0u; i < r_expected_vector.size(); ++i) {
      ret = ret && std::abs(r_actual_vector[i] - r_expected_vector[i]) <=
          std::abs(r_expected_vector[i]) * delta;
      if (!ret) break;
    }
  }
  else {
    std::cout << "Vectors have different size!" << std::endl;
  }
  return ret;
}
}  // namespace util
}  // namespace iblbm
#endif  // SRC_UTILITY_IBLBMUTILITIES_HPP_
