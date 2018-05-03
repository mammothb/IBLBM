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
    mpExData{nullptr},
    mpInDataCoords{nullptr},
    mpExDataCoords{nullptr},
    mpTmpInNbrNumCells{nullptr},
    mHasInitializedInNeighbor{false},
    mHasInitializedExNeighbor{false}
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
    mExCells{rRhs.mExCells},
    mInNbrCuboids{rRhs.mInNbrCuboids},
    mInNbrNumCells{rRhs.mInNbrNumCells},
    mExNbrCuboids{rRhs.mExNbrCuboids},
    mExNbrNumCells{rRhs.mExNbrNumCells},
    mHasInitializedInNeighbor{false},
    mHasInitializedExNeighbor{false}
{}

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
bool** CuboidNeighborhood2D<T>::pGetExData()
{
  return mpExData;
}

template<typename T>
void CuboidNeighborhood2D<T>::AddInCell(const Cell2D<T>& rCell)
{
  mInCells.push_back(rCell);
}

template<typename T>
void CuboidNeighborhood2D<T>::AddExCell(const Cell2D<T>& rCell)
{
  mExCells.push_back(rCell);
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
  mpInDataCoords = new T* [mNumCuboid];
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
    }
#ifdef IBLBM_PARALLEL_MPI
    if (mpTmpInNbrNumCells[global_idx] != 0) {
      mpInData[global_idx] = new bool [mpTmpInNbrNumCells[global_idx] *
          mNumData * mSizeofDataType];
      mpInDataCoords[global_idx] = new T [mpTmpInNbrNumCells[global_idx] * 2];
    }
    else {
      mpInData[global_idx] = nullptr;
      mpInDataCoords[global_idx] = nullptr;
    }
#endif  // IBLBM_PARALLEL_MPI
  }
#ifdef IBLBM_PARALLEL_MPI
  std::size_t counter {0};
  // Find out how many other processes there are, and allocate memory for
  // requests and status
  for (gsl::index global_idx {0}; global_idx < mNumCuboid; ++global_idx) {
    if (MpiManager::Instance().GetRank() !=
        mrSuperStructure.rGetLoadBalancer().GetRank(global_idx)) {
      ++counter;
    }
  }
  mNonblockingHelper.Allocate(counter);
  counter = 0;
  // Send request to neighbor cuboids which contains cells we need in our
  // mInCells
  for (gsl::index global_idx {0}; global_idx < mNumCuboid; ++global_idx) {
    auto dest_rank {mrSuperStructure.rGetLoadBalancer().GetRank(global_idx)};
    if (MpiManager::Instance().GetRank() != dest_rank) {
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
void CuboidNeighborhood2D<T>::InitializeExNeighbor()
{
  mExNbrCuboids.clear();
  mExNbrNumCells.clear();
  mpExData = new bool* [mNumCuboid];
  mpExDataCoords = new T* [mNumCuboid];

  std::vector<std::size_t> tmp_ex_nbr_num_cells(mNumCuboid, 0);

  // Calculate the distribution of mExCells across global cuboids
  for (gsl::index i {0}; i < mExCells.size(); ++i)
      ++tmp_ex_nbr_num_cells[mExCells[i].mGlobalCuboidIndex];

  for (gsl::index global_idx {0}; global_idx < mNumCuboid; ++global_idx) {
#ifdef IBLBM_PARALLEL_MPI
    auto source_rank {mrSuperStructure.rGetLoadBalancer().GetRank(
        global_idx)};
    // Receive request from other cuboids for their mInCells
    if (MpiManager::Instance().GetRank() != source_rank) {
      MpiManager::Instance().Receive(&tmp_ex_nbr_num_cells[global_idx],
          /*count=*/1, source_rank, global_idx);
    }
#endif  // IBLBM_PARALLEL_MPI
    // If other cuboid request a cell from us, add them to out neighbors
    if (tmp_ex_nbr_num_cells[global_idx] != 0) {
      mExNbrCuboids.push_back(global_idx);
      mExNbrNumCells.push_back(tmp_ex_nbr_num_cells[global_idx]);
    }
    mpExData[global_idx] = new bool [tmp_ex_nbr_num_cells[global_idx] *
        mNumData * mSizeofDataType];
    mpExDataCoords[global_idx] = new T [tmp_ex_nbr_num_cells[global_idx] * 2];
  }

  mHasInitializedExNeighbor = true;
}

template<typename T>
void CuboidNeighborhood2D<T>::SendInDataCoordinates()
{
#ifdef IBLBM_PARALLEL_MPI
  mNonblockingHelper.Free();
  std::vector<int> tmp(mNumCuboid, 0);
  for (gsl::index i {0}; i < mInCells.size(); ++i) {
    auto global_idx {mInCells[i].mGlobalCuboidIndex};
    if (MpiManager::Instance().GetRank() !=
        mrSuperStructure.rGetLoadBalancer().GetRank(global_idx)) {
      mpInDataCoords[global_idx][2 * tmp[global_idx]] = mInCells[i].mPhysR[0];
      mpInDataCoords[global_idx][2 * tmp[global_idx] + 1] =
          mInCells[i].mPhysR[0];
      ++tmp[global_idx];
    }
  }
  mNonblockingHelper.Allocate(mInNbrCuboids.size());
  for (gsl::index i {0}; i < mInNbrCuboids.size(); ++i) {
    auto dest_rank {mrSuperStructure.rGetLoadBalancer().GetRank(
        mInNbrCuboids[i])};
    MpiManager::Instance().Isend(mpInDataCoords[mInNbrCuboids[i]],
        mInNbrNumCells[i] * 2, dest_rank,
        &mNonblockingHelper.pGetMpiRequest()[i], mGlobalCuboidIndex);
  }
#endif  // IBLBM_PARALLEL_MPI
}

template<typename T>
void CuboidNeighborhood2D<T>::ReceiveExDataCoordinates()
{
#ifdef IBLBM_PARALLEL_MPI
  for (gsl::index i {0}; i < mExNbrCuboids.size(); ++i) {
    auto source_rank {mrSuperStructure.rGetLoadBalancer().GetRank(
        mExNbrCuboids[i])};
    MpiManager::Instance().Receive(mpExDataCoords[mExNbrCuboids[i]],
        mExNbrNumCells[i] * 2, source_rank, mExNbrCuboids[i]);
    if (MpiManager::Instance().GetRank() != source_rank) {
      Cell2D<T> cell;
      for (gsl::index j {0}; j < mExNbrNumCells[i]; ++j) {
        cell.mGlobalCuboidIndex = mExNbrCuboids[i];
        cell.mPhysR[0] = mpExDataCoords[mExNbrCuboids[i]][2 * j];
        cell.mPhysR[1] = mpExDataCoords[mExNbrCuboids[i]][2 * j + 1];
        mrSuperStructure.rGetCuboidGeometry().GetLatticeR(cell.mPhysR,
            cell.mGlobalCuboidIndex, cell.mLatticeR);
        mExCells.push_back(cell);
      }
    }
  }
#endif  // IBLBM_PARALLEL_MPI
}

template<typename T>
void CuboidNeighborhood2D<T>::FinishComm()
{
#ifdef IBLBM_PARALLEL_MPI
  MpiManager::Instance().WaitAll(mNonblockingHelper);
#endif  // IBLBM_PARALLEL_MPI
}

template<typename T>
void CuboidNeighborhood2D<T>::BufferExData()
{
  std::vector<int> tmp(mNumCuboid, 0);
  auto local_idx {mrSuperStructure.rGetLoadBalancer().GetLocalIndex(
      mGlobalCuboidIndex)};
  for (gsl::index i {0}; i < mExCells.size(); ++i) {
    auto global_idx {mExCells[i].mGlobalCuboidIndex};
    std::memcpy(mpExData[global_idx] + tmp[global_idx] * mNumData *
        mSizeofDataType, mrSuperStructure(local_idx,
            mExCells[i].mLatticeR[0], mExCells[i].mLatticeR[1], 0),
        mNumData * mSizeofDataType);
    ++tmp[global_idx];
  }
}

template<typename T>
void CuboidNeighborhood2D<T>::SendExData()
{
#ifdef IBLBM_PARALLEL_MPI
  for (gsl::index i {0}; i < mExNbrCuboids.size(); ++i) {
    auto dest_rank {mrSuperStructure.rGetLoadBalancer().GetRank(
        mExNbrCuboids[i])};
    MpiManager::Instance().Isend(mpExData[mExNbrCuboids[i]],
        mExNbrNumCells[i] * mNumData * mSizeofDataType, dest_rank,
        &mNonblockingHelper.pGetMpiRequest()[i], mGlobalCuboidIndex);
  }
#endif  // IBLBM_PARALLEL_MPI
}

template<typename T>
void CuboidNeighborhood2D<T>::ReceiveInData()
{
#ifdef IBLBM_PARALLEL_MPI
  for (gsl::index i {0}; i < mInNbrCuboids.size(); ++i) {
    auto source_rank {mrSuperStructure.rGetLoadBalancer().GetRank(
        mInNbrCuboids[i])};
    MpiManager::Instance().Receive(mpInData[mInNbrCuboids[i]],
        mInNbrNumCells[i] * mNumData * mSizeofDataType, source_rank,
        mInNbrCuboids[i]);
  }
#endif  // IBLBM_PARALLEL_MPI
}

template<typename T>
void CuboidNeighborhood2D<T>::WriteInData()
{
  auto local_idx {mrSuperStructure.rGetLoadBalancer().GetLocalIndex(
      mGlobalCuboidIndex)};
  std::vector<int> tmp(mNumCuboid, 0);
  for (gsl::index i {0}; i < mInCells.size(); ++i) {
    std::memcpy(mrSuperStructure(local_idx, mInCells[i].mLatticeR[0],
        mInCells[i].mLatticeR[1], 0), mpInData[local_idx] + tmp[local_idx] *
        mNumData * mSizeofDataType, mNumData * mSizeofDataType);
    ++tmp[local_idx];
  }
#ifdef IBLBM_PARALLEL_MPI
  for (gsl::index i {0}; i < mInNbrCuboids.size(); ++i) {
    auto source_rank {mrSuperStructure.rGetLoadBalancer().GetRank(
        mInNbrCuboids[i])};
    MpiManager::Instance().Receive(mpInData[mInNbrCuboids[i]],
        mInNbrNumCells[i] * mNumData * mSizeofDataType, source_rank,
        mInNbrCuboids[i]);
  }
#endif  // IBLBM_PARALLEL_MPI
}

template<typename T>
void CuboidNeighborhood2D<T>::Reset()
{
  if (mHasInitializedInNeighbor) {
#ifdef IBLBM_PARALLEL_MPI
    for (gsl::index i {0}; i < mNumCuboid; ++i) {
      delete[] mpInData[i];
      delete[] mpInDataCoords[i];
    }
#endif  // IBLBM_PARALLEL_MPI
    delete[] mpInData;
    delete[] mpInDataCoords;
    delete[] mpTmpInNbrNumCells;
    mHasInitializedInNeighbor = false;
  }
  if (mHasInitializedExNeighbor) {
    for (gsl::index global_idx {0}; global_idx < mNumCuboid; ++global_idx) {
      delete[] mpExData[global_idx];
      delete[] mpExDataCoords[global_idx];
    }
    delete[] mpExData;
    delete[] mpExDataCoords;
#ifdef IBLBM_PARALLEL_MPI
    mNonblockingHelper.Free();
#endif  // IBLBM_PARALLEL_MPI
    mHasInitializedExNeighbor = false;
  }
  mInCells.clear();
  mExCells.clear();
  mInNbrCuboids.clear();
  mInNbrNumCells.clear();
  mExNbrCuboids.clear();
  mExNbrNumCells.clear();
}

// Explicit instantiation
template struct Cell2D<double>;
template class CuboidNeighborhood2D<double>;
}  // namespace iblbm
