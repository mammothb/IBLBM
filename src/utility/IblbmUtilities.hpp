#ifndef SRC_UTILITY_IBLBMUTILITIES_HPP_
#define SRC_UTILITY_IBLBMUTILITIES_HPP_

#include <iomanip>  // std::setprecision
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <vector>

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
  if (rVec1.size() != rVec2.size())
      throw std::runtime_error("Size mismatch");
  return std::inner_product(rVec1.begin(), rVec1.end(), rVec2.begin(),
      static_cast<T>(0));
}

/**
 * The hydrodynamic (aka macroscopic) variables on each node are defined as
 * moments of the particle populations. The particle density (rho) stands
 * for the moment of order zero.
 *
 * Formula: rho = Summation of dist.func
 *
 * \param node Distribution function node containing nine discrete velocity
 *        vectors
 * \return zeroth moment of node i.e. particle density
 */
template <typename T>
T GetZerothMoment(const std::vector<T>& rNode)
{
  return std::accumulate(rNode.begin(), rNode.end(), static_cast<T>(0));
}

/**
 * The hydrodynamic (aka macroscopic) variables on each node are defined as
 * moments of the particle populations. The fluid momentum (rho * velocity)
 * stands for the moment of order one.
 *
 * Formula: fluid momentum = Summation of (discrete vel * dist.func)
 *
 * \param node Distribution function node containing nine discrete velocity
 *        vectors
 * \return first moment of node i.e. the fluid momentum
 */
template <typename T, typename VALUE = typename T::value_type>
T GetFirstMoment(
    const T& rNode
  , const std::vector<T>& rDiscreteVelocity)
{
  auto nd = rDiscreteVelocity.front().size();
  T result(nd, 0);
  for (auto d = 0u; d < nd; ++d) {
    result[d] = std::inner_product(rNode.begin(), rNode.end(),
        rDiscreteVelocity.begin(), static_cast<VALUE>(0), std::plus<>(),
        [&d] (VALUE lhs, const T& rRhs) {
          return lhs * rRhs[d];
        });
  }  // d
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
    const T& rLattice
  , U nx)
{
  auto flipped_lattice(rLattice);
  auto ny = rLattice.size() / nx;
  for (auto n = 0u; n < nx * ny; ++n) {
    auto n_flipped = (ny - 1 - n / nx) * nx + n % nx;
    flipped_lattice[n_flipped] = rLattice[n];
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
T Organize(const T& rLattice)
{
  auto organized_lattice(rLattice);
  auto index = 0;
  for (auto& node : organized_lattice) {
    // 3 and 8 are in organized position
    node[0] = rLattice[index][6];
    node[1] = rLattice[index][2];
    node[2] = rLattice[index][5];
    node[4] = rLattice[index][0];
    node[5] = rLattice[index][1];
    node[6] = rLattice[index][7];
    node[7] = rLattice[index][4];
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
    const T& rLattice
  , U nx
  , U ny)
{
  auto depth = rLattice[0].size();
  // Distribution function and velocity lattices will have size nx * ny, so
  // modulo will return 0. Boundary nodes will modify depth value.
  auto code = depth + rLattice.size() % (nx * ny);
  // 2 for depth 2, 9 for depth 9, default for boundary
  switch (code) {
    case 2: {
      auto counter = 0;
      auto flipped_lattice = Flip(rLattice, nx);
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
      auto lat = Flip(Organize(rLattice), nx);
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
      auto lat = Organize(rLattice);
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
 * @param rActualVector First vector
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
    const VECTOR& rActualVector
  , const VECTOR& rExpectedVector
  , double delta)
{
  // Check that the two array are of equal length
  auto ret = rActualVector.size() == rExpectedVector.size();
  if (ret) {
    for (auto i = 0u; i < rExpectedVector.size(); ++i) {
      ret = ret && std::abs(rActualVector[i] - rExpectedVector[i]) <=
          std::abs(rExpectedVector[i]) * delta;
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
