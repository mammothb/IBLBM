#ifndef PARALLEL_SRC_COMMUNICATOR2D_HPP_
#define PARALLEL_SRC_COMMUNICATOR2D_HPP_

#include <cstddef>

#include "CuboidNeighborhood2D.hpp"

namespace iblbm
{
// Forward declaration
template<typename T> class SuperStructure2D;

template<typename T>
class Communicator2D
{
 public:
  explicit Communicator2D(SuperStructure2D<T>& rSuperStructure);

  /** Initialize neighborhood information */
  void InitializeNeighborhood();

 private:
  /** Neighborhood information for a single cuboid */
  std::vector<CuboidNeighborhood2D<T>> mNeighborhood;
  /** Reference to the super structure */
  SuperStructure2D<T>& mrSuperStructure;
  /** Number of cuboids in this structure */
  std::size_t mNumCuboid;
  /** Flag to indicate if initialization was done */
  bool mIsInitialized;
};
}  // namespace iblbm

#endif  // PARALLEL_SRC_COMMUNICATOR2D_HPP_
