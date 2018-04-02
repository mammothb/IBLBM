#ifndef SRC_GEOMETRY_CUBOIDGEOMETRY2D_HPP_
#define SRC_GEOMETRY_CUBOIDGEOMETRY2D_HPP_

#include <vector>

#include "Cuboid2D.hpp"
#include "OstreamManager.hpp"

namespace iblbm
{
template<typename T> class AbstractIndicatorFunctor2D;
/**
 * Cuboid geometry contains one or multiple cuboid. To represent a connected
 * domain, the distance between two neighboring cuboid must be less than the
 * smallest delta R between them.
 */
template<typename T>
class CuboidGeometry2D
{
 public:
  /**
   * Constructor, creates an empty cuboid
   */
  CuboidGeometry2D();

 private:
  /** Vector of cuboids */
  std::vector<Cuboid2D<T>> mCuboids;
  /** Cuboid which contains all other cuboids */
  Cuboid2D<T> mMotherCuboid;
  /** Flag to indicate periodicity */
  std::vector<bool> mIsPeriodic;
  mutable OstreamManager mOstream;
};
}  // namespace iblbm

#endif  // SRC_GEOMETRY_CUBOIDGEOMETRY2D_HPP_
