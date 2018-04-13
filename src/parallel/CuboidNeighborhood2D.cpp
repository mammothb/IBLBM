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
