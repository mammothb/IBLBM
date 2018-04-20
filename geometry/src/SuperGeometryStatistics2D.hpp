#ifndef GEOMETRY_SRC_SUPERGEOMETRYSTATISTICS2D_HPP_
#define GEOMETRY_SRC_SUPERGEOMETRYSTATISTICS2D_HPP_

#include "OstreamManager.hpp"

namespace iblbm
{
template<typename T> class SuperGeometry2D;

template<typename T>
class SuperGeometryStatistics2D
{
 public:
  SuperGeometryStatistics2D(SuperGeometry2D<T>* pSuperGeometry);

 private:
  /** Pointer to the underlying data for the statistics */
  SuperGeometry2D<T>* mpSuperGeometry;
  /** Flag to determine if an update is needed */
  bool mNeedUpdate;
  /** Size of ghost layer */
  std::size_t mOverlap;
  /** Number of different material numbers */
  std::size_t mNumMaterials;
  /** Class specific output stream */
  mutable OstreamManager mOstream;
};
}  // namespace iblbm

#endif  // GEOMETRY_SRC_SUPERGEOMETRYSTATISTICS2D_HPP_
