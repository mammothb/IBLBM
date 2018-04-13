#ifndef PARALLEL_SRC_CUBOIDNEIGHTBORHOOD2D_HPP_
#define PARALLEL_SRC_CUBOIDNEIGHTBORHOOD2D_HPP_

#include "gsl/gsl"
#include "MpiManager.hpp"
#include "Vector.hpp"

namespace iblbm
{
template<typename T> class SuperStructure2D;

template<typename T>
struct Cell2D
{
  /** Copy constructor */
  Cell2D(const Cell2D& rRhs) = default;

  /** Copy assignment */
  Cell2D& operator=(const Cell2D& rRhs) = default;

  /**
   * is equal to operator. check that both local position and cuboid number
   * are equal
   */
  bool operator==(const Cell2D& rRhs) const
  {
    return mLatticePosition == rRhs.mLatticePosition &&
        mCuboidIndex == rRhs.mCuboidIndex;
  }

  /** cuboid number */
  gsl::index mCuboidIndex;
  /** local position */
  Vector2D<gsl::index> mLatticePosition;
  /** global position */
  Vector2D<T> mPhysPosition;
};

template<typename T>
class CuboidNeighborhood2D
{
  friend class TestCuboidNeighborhood2D;
 public:
  /**
   * Constructor
   *
   * \param rSuperStructure reference to superstructure
   * \param cuboidIndex cuboid number
   */
  CuboidNeighborhood2D(
      SuperStructure2D<T>& rSuperStructure
    , gsl::index cuboidIndex);

  /**
   * Destructor
   */
  ~CuboidNeighborhood2D();

  /**
   * Copy constructor
   */
  CuboidNeighborhood2D(const CuboidNeighborhood2D<T>& rRhs);

//  /**
//   * Copy assignment
//   */
//  CuboidNeighborhood2D& operator=(const CuboidNeighborhood2D<T>& rRhs);

  /** \return read-only access to mInCells */
  const Cell2D<T>& rGetInCell(gsl::index i) const;

  /** \return number of cells in mInCells */
  std::size_t GetInCellsSize() const;

  /** \return read-only access to mInNeighborCuboid */
  const gsl::index& rGetInNeighborCuboid(gsl::index i) const;

  /** \return number of cuboids in mInNeighborCuboids */
  std::size_t GetInNeighborCuboidsSize() const;

  /**
   * Resets neighborhood after initialization (InitializeInternal and
   * InitializeExternal)
   */
  void Reset();

 private:
  /** Reference to the super structure */
  SuperStructure2D<T>& mrSuperStructure;
  /** global cuboid index */
  gsl::index mGlobalCuboidIndex;
  /** Number of cuboids in this structure */
  std::size_t mNumCuboid;
  /** node spacing of the cuboid */
  T mDeltaR;
  /** Number of data to be transferred */
  std::size_t mNumData;
  /** Size of the underlying data type */
  std::size_t mSizeofDataType;
  /** Internal needed cells */
  std::vector<Cell2D<T>> mInCells;
  /** External needed cells */
  std::vector<Cell2D<T>> mOutCells;
  /** Internal needed neighbor cuboid */
  std::vector<gsl::index> mInNeighborCuboids;
  /** Internal needed neighbor number of cells */
  std::vector<std::size_t> mInNeighborNumCells;
  /** External needed neighbor cuboid */
  std::vector<gsl::index> mOutNeighborCuboids;
  /** External needed neighbor number of cells */
  std::vector<std::size_t> mOutNeighborNumCells;
  /** Buffer for internal needed data */
  bool** mpInData;
  /** Buffer for external needed data */
  bool** mpOutData;
  /** Buffer for internal needed data coordinates */
  T** mpInDataCoordinates;
  /** Buffer for external needed data coordinates */
  T** mpOutDataCoordinates;
  /** Temporary buffer to compute neighbor cuboid index and num cells */
  std::size_t* mpTempInNeighbor;
  /**
   * Flag to indicate if internal neighbor cuboid and number of cells is
   * initialized
   */
  bool mHasInitializedInNeighbor;
  /**
   * Flag to indicate if external neighbor cuboid and number of cells is
   * initialized
   */
  bool mHasInitializedOutNeighbor;
#ifdef IBLBM_PARALLEL_MPI
  MpiNonblockingHelper mNonblockingHelper;
#endif  // IBLBM_PARALLEL_MPI
};
}  // namespace iblbm

#endif  // PARALLEL_SRC_CUBOIDNEIGHTBORHOOD2D_HPP_
