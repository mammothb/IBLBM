#ifndef CORE_SRC_LATTICESTATISTICS_HPP_
#define CORE_SRC_LATTICESTATISTICS_HPP_

#include <cstddef>
#include <vector>

namespace iblbm
{
template<typename T>
class LatticeStatistics {
  friend class TestLatticeStatistics;

 public:
  enum AvgValue {
      AVG_RHO,
      AVG_ENERGY,
      NUM_AVG_VALUE
  };

  enum MaxValue {
      MAX_U,
      NUM_MAX_VALUE
  };

  /**
   * Default constructor
   */
  LatticeStatistics();

  /**
   * Default destructor
   */
  ~LatticeStatistics();

  /**
   * Increment values for internal computation
   *
   * \param rho particle density
   * \param uSqr Inner product of fluid velocity
   */
  void IncrementStats(
      T rho
    , T uSqr);

 private:
  /** Variables for internal computation */
  std::vector<T> mInternalAverageValues;
  std::vector<T> mInternalMaxValues;
  std::size_t mInternalNumCells;
};
}  // namespace iblbm

#endif  // CORE_SRC_LATTICESTATISTICS_HPP_
