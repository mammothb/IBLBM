#include "CuboidNeighborhood2D.hpp"

#include "SuperStructure2D.hpp"

namespace iblbm
{
template<typename T>
CuboidNeighborhood2D<T>::CuboidNeighborhood2D(
    SuperStructure2D<T>& rSuperStructure
  , gsl::index globalCuboidIndex)
  : mrSuperStructure{rSuperStructure},
    mGlobalCuboidIndex{globalCuboidIndex},
    mNumCuboid{mrSuperStructure.rGetCuboidGeometry().GetNumberOfCuboids()},
    mDeltaR{mrSuperStructure.rGetCuboidGeometry().rGetCuboid(
        globalCuboidIndex).GetDeltaR()},
    mNumData{mrSuperStructure.GetDataSize()},
    mSizeofDataType{mrSuperStructure.GetSizeofDataType()},
    mpInData{nullptr},
    mpOutData{nullptr},
    mpInDataCoordinates{nullptr},
    mpOutDataCoordinates{nullptr},
    mpTempInNeighbor{nullptr},
    mHasInitializedInNeighbor{false},
    mHasInitializedOutNeighbor{false}
{}

template<typename T>
CuboidNeighborhood2D<T>::~CuboidNeighborhood2D()
{
  Reset();
}

template<typename T>
CuboidNeighborhood2D<T>::CuboidNeighborhood2D(
    const CuboidNeighborhood2D<T>& rRhs)
  : mrSuperStructure{rRhs.mrSuperStructure},
    mGlobalCuboidIndex{rRhs.mGlobalCuboidIndex},
    mNumCuboid{rRhs.mNumCuboid},
    mDeltaR{rRhs.mDeltaR},
    mNumData{rRhs.mNumData},
    mSizeofDataType{rRhs.mSizeofDataType},
    mInCells{rRhs.mInCells},
    mOutCells{rRhs.mOutCells},
    mInNeighborCuboids{rRhs.mInNeighborCuboids},
    mInNeighborNumCells{rRhs.mInNeighborNumCells},
    mOutNeighborCuboids{rRhs.mOutNeighborCuboids},
    mOutNeighborNumCells{rRhs.mOutNeighborNumCells},
    mHasInitializedInNeighbor{false},
    mHasInitializedOutNeighbor{false}
{}

//template<typename T>
//CuboidNeighborhood2D<T>& CuboidNeighborhood2D<T>::operator=(
//    const CuboidNeighborhood2D<T>& rRhs)
//{
//  mrSuperStructure = rRhs.mrSuperStructure;
//  mGlobalCuboidIndex = rRhs.mGlobalCuboidIndex;
//  mNumCuboid = rRhs.mNumCuboid;
//  mDeltaR = rRhs.mDeltaR;
//  mNumData = rRhs.mNumData;
//  mSizeofDataType = rRhs.mSizeofDataType;
//  mInCells = rRhs.mInCells;
//  mOutCells = rRhs.mOutCells;
//  mInNeighborCuboids = rRhs.mInNeighborCuboids;
//  mInNeighborNumCells = rRhs.mInNeighborNumCells;
//  mOutNeighborCuboids = rRhs.mOutNeighborCuboids;
//  mOutNeighborNumCells = rRhs.mOutNeighborNumCells;
//  mHasInitializedInNeighbor = false;
//  mHasInitializedOutNeighbor = false;
//
//  return *this;
//}

template<typename T>
const Cell2D<T>& CuboidNeighborhood2D<T>::rGetInCell(gsl::index i) const
{
  return mInCells[i];
}

template<typename T>
std::size_t CuboidNeighborhood2D<T>::GetInCellsSize() const
{
  return mInCells.size();
}

template<typename T>
const gsl::index& CuboidNeighborhood2D<T>::rGetInNeighborCuboid(
    gsl::index i) const
{
  return mInNeighborCuboids[i];
}

template<typename T>
std::size_t CuboidNeighborhood2D<T>::GetInNeighborCuboidsSize() const
{
  return mInNeighborCuboids.size();
}

template<typename T>
bool** CuboidNeighborhood2D<T>::pGetInData()
{
  return mpInData;
}

template<typename T>
bool** CuboidNeighborhood2D<T>::pGetOutData()
{
  return mpOutData;
}

template<typename T>
void CuboidNeighborhood2D<T>::AddInCell(const Cell2D<T>& rCell)
{
  mInCells.push_back(rCell);
}

template<typename T>
void CuboidNeighborhood2D<T>::AddOutCell(const Cell2D<T>& rCell)
{
  mOutCells.push_back(rCell);
}

template<typename T>
void CuboidNeighborhood2D<T>::AddInCell(
    gsl::index xIndex
  , gsl::index yIndex)
{
  Cell2D<T> cell {mGlobalCuboidIndex, xIndex, yIndex,
      mrSuperStructure.rGetCuboidGeometry().GetPhysR(mGlobalCuboidIndex,
          xIndex, yIndex)};
  if (mrSuperStructure.rGetCuboidGeometry().HasCuboid(cell.mPhysR,
      cell.mGlobalCuboidIndex)) {
    for (gsl::index i {0}; i < mInCells.size(); ++i) {
      if (mInCells[i] == cell) return;
    }
    mInCells.push_back(cell);
  }
}

template<typename T>
void CuboidNeighborhood2D<T>::AddInCell(std::size_t overlap)
{
  auto nx {mrSuperStructure.rGetCuboidGeometry().rGetCuboid(
      mGlobalCuboidIndex).GetNx()};
  auto ny {mrSuperStructure.rGetCuboidGeometry().rGetCuboid(
      mGlobalCuboidIndex).GetNy()};

  for (gsl::index x {0}; x < nx + 2 * overlap; ++x) {
    for (gsl::index y {0}; y < ny + 2 * overlap; ++y) {
      if (x < overlap || x > nx + overlap - 1 ||
          y < overlap || y > ny + overlap - 1) {
        AddInCell(x - overlap, y - overlap);
      }
    }
  }
}

template<typename T>
void CuboidNeighborhood2D<T>::Reset()
{
  if (mHasInitializedInNeighbor) {
#ifdef IBLBM_PARALLEL_MPI
    for (gsl::index i {0}; i < mNumCuboid; ++i) {
      delete[] mpInData[i];
      delete[] mpInDataCoordinates[i];
    }
#endif  // IBLBM_PARALLEL_MPI
    delete[] mpInData;
    delete[] mpInDataCoordinates;
    delete[] mpTempInNeighbor;
    mHasInitializedInNeighbor = false;
  }
  if (mHasInitializedOutNeighbor) {
    for (gsl::index i {0}; i < mNumCuboid; ++i) {
      delete[] mpOutData[i];
      delete[] mpOutDataCoordinates[i];
    }
    delete[] mpOutData;
    delete[] mpOutDataCoordinates;
#ifdef IBLBM_PARALLEL_MPI
    mNonblockingHelper.Free();
#endif  // IBLBM_PARALLEL_MPI
    mHasInitializedOutNeighbor = false;
  }
  mInCells.clear();
  mOutCells.clear();
  mInNeighborCuboids.clear();
  mInNeighborNumCells.clear();
  mOutNeighborCuboids.clear();
  mOutNeighborNumCells.clear();
}

// Explicit instantiation
template struct Cell2D<double>;
template class CuboidNeighborhood2D<double>;
}  // namespace iblbm
