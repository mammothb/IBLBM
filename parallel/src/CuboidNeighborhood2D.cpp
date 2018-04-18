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
    mpTmpInNbrNumCells{nullptr},
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
    mInNbrCuboids{rRhs.mInNbrCuboids},
    mInNbrNumCells{rRhs.mInNbrNumCells},
    mOutNbrCuboids{rRhs.mOutNbrCuboids},
    mOutNbrNumCells{rRhs.mOutNbrNumCells},
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
//  mInNbrCuboids = rRhs.mInNbrCuboids;
//  mInNbrNumCells = rRhs.mInNbrNumCells;
//  mOutNbrCuboids = rRhs.mOutNbrCuboids;
//  mOutNbrNumCells = rRhs.mOutNbrNumCells;
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
  return mInNbrCuboids[i];
}

template<typename T>
std::size_t CuboidNeighborhood2D<T>::GetInNeighborCuboidsSize() const
{
  return mInNbrCuboids.size();
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
void CuboidNeighborhood2D<T>::AddInCells(std::size_t overlap)
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
void CuboidNeighborhood2D<T>::InitializeInNeighbor()
{
  mInNbrCuboids.clear();
  mInNbrNumCells.clear();

  mpInData = new bool* [mNumCuboid];
  mpInDataCoordinates = new T* [mNumCuboid];
  mpTmpInNbrNumCells = new std::size_t [mNumCuboid];
  for (gsl::index global_idx {0}; global_idx < mNumCuboid; ++global_idx)
      mpTmpInNbrNumCells[global_idx] = 0;
  // Calculate the distribution of mInCells across global cuboids
  for (gsl::index i {0}; i < mInCells.size(); ++i)
      ++mpTmpInNbrNumCells[mInCells[i].mGlobalCuboidIndex];

  for (gsl::index global_idx {0}; global_idx < mNumCuboid; ++global_idx) {
    // If a cuboid has our mInCells, add it as a neighbor cuboid
    if (mpTmpInNbrNumCells[global_idx] != 0) {
      mInNbrCuboids.push_back(global_idx);
      mInNbrNumCells.push_back(mpTmpInNbrNumCells[global_idx]);
#ifdef IBLBM_PARALLEL_MPI
      mpInData[global_idx] = new bool [mpTmpInNbrNumCells[global_idx] *
          mNumData * mSizeofDataType];
      mpInDataCoordinates[global_idx] = new T [
          mpTmpInNbrNumCells[global_idx] * 2];
#endif  // IBLBM_PARALLEL_MPI
    }
#ifdef IBLBM_PARALLEL_MPI
    else {
      mpInData[global_idx] = nullptr;
      mpInDataCoordinates[global_idx] = nullptr;
    }
#endif  // IBLBM_PARALLEL_MPI
  }
#ifdef IBLBM_PARALLEL_MPI
  std::size_t counter {0};
  for (gsl::index global_idx {0}; global_idx < mNumCuboid; ++global_idx) {
    auto dest_rank {mrSuperStructure.rGetLoadBalancer().GetRank(global_idx)};
    if (MpiManager::Instance().GetRank() != dest_rank) ++counter;
  }
  mNonblockingHelper.Allocate(counter);
  counter = 0;
  // Send request to neighbor cuboids which contains cells we need in our
  // mInCells
  for (gsl::index global_idx {0}; global_idx < mNumCuboid; ++global_idx) {
    auto dest_rank {mrSuperStructure.rGetLoadBalancer().GetRank(global_idx)};
    if (MpiManager::Instance().GetRank() != dest_rank ) {
      MpiManager::Instance().Isend(&mpTmpInNbrNumCells[global_idx],
          /*count=*/1, dest_rank,
          &mNonblockingHelper.pGetMpiRequest()[counter], mGlobalCuboidIndex);
      counter++;
    }
  }
#endif  // IBLBM_PARALLEL_MPI
  mHasInitializedInNeighbor = true;
}

template<typename T>
void CuboidNeighborhood2D<T>::InitializeOutNeighbor()
{
  mOutNbrCuboids.clear();
  mOutNbrNumCells.clear();
  mpOutData = new bool* [mNumCuboid];
  mpOutDataCoordinates = new T* [mNumCuboid];

  std::vector<std::size_t> tmp_out_nbr_num_cells(mNumCuboid, 0);

  // Calculate the distribution of mOutCells across global cuboids
  for (gsl::index i {0}; i < mOutCells.size(); ++i)
      ++tmp_out_nbr_num_cells[mOutCells[i].mGlobalCuboidIndex];

  for (gsl::index global_idx {0}; global_idx < mNumCuboid; ++global_idx) {
#ifdef IBLBM_PARALLEL_MPI
    auto source_rank {mrSuperStructure.rGetLoadBalancer().GetRank(
        global_idx)};
    // Receive request from other cuboids for their mInCells
    if (MpiManager::Instance().GetRank() != source_rank ) {
      MpiManager::Instance().Receive(&tmp_out_nbr_num_cells[global_idx],
          /*count=*/1, source_rank, global_idx);
    }
#endif  // IBLBM_PARALLEL_MPI
    // If other cuboid request a cell from us, add them to out neighbors
    if (tmp_out_nbr_num_cells[global_idx] != 0) {
      mOutNbrCuboids.push_back(global_idx);
      mOutNbrNumCells.push_back(tmp_out_nbr_num_cells[global_idx]);
    }
    mpOutData[global_idx] = new bool [tmp_out_nbr_num_cells[global_idx] *
        mNumData * mSizeofDataType];
    mpOutDataCoordinates[global_idx] = new T [
        tmp_out_nbr_num_cells[global_idx] * 2];
  }

  mHasInitializedOutNeighbor = true;
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
    delete[] mpTmpInNbrNumCells;
    mHasInitializedInNeighbor = false;
  }
  if (mHasInitializedOutNeighbor) {
    for (gsl::index global_idx {0}; global_idx < mNumCuboid; ++global_idx) {
      delete[] mpOutData[global_idx];
      delete[] mpOutDataCoordinates[global_idx];
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
  mInNbrCuboids.clear();
  mInNbrNumCells.clear();
  mOutNbrCuboids.clear();
  mOutNbrNumCells.clear();
}

// Explicit instantiation
template struct Cell2D<double>;
template class CuboidNeighborhood2D<double>;
}  // namespace iblbm
