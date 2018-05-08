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
  friend class TestCommunicator2D;
 public:
  explicit Communicator2D(SuperStructure2D<T>& rSuperStructure);

  /** Initialize neighborhood information */
  void InitializeNeighborhood();

  /**
   * Added a cell to the cuboid neighborhood localCuboidIndex
   *
   * \param localCuboidIndex local index of the cuboid assigned to the
   *        process
   * \param xIndex x index
   * \param yIndex y index
   */
  void AddCell(
      gsl::index localCuboidIndex
    , gsl::index xIndex
    , gsl::index yIndex);

  /**
   * Adds all cells of the overlap to its associated cuboid neigbourhood
   *
   * \param overlap
   */
  void AddCells(std::size_t overlap);

  /**
   * Initialize the communicator
   */
  void Initialize();

  /**
   * Reset the communicator
   */
  void Reset();

  /** Send data */
  void Send();

  /** Receive data */
  void Receive();

  /** Write data to the block lattices of the super structure */
  void Write();

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
