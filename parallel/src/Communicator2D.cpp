#include "Communicator2D.hpp"

#include "CuboidNeighborhood2D.hpp"
#include "SuperStructure2D.hpp"

namespace iblbm
{
template<typename T>
Communicator2D<T>::Communicator2D(SuperStructure2D<T>& rSuperStructure)
  : mrSuperStructure{rSuperStructure},
    mNumCuboid{0},
    mIsInitialized{false}
{}

template<typename T>
void Communicator2D<T>::InitializeNeighborhood()
{
  mNumCuboid = mrSuperStructure.rGetCuboidGeometry().GetNumberOfCuboids();
  for (gsl::index local_idx {0};
      local_idx < mrSuperStructure.rGetLoadBalancer().GetSize();
      ++local_idx) {
    CuboidNeighborhood2D<T> neighborhood {mrSuperStructure,
        mrSuperStructure.rGetLoadBalancer().GetGlobalIndex(local_idx)};
    mNeighborhood.push_back(neighborhood);
  }
}

template<typename T>
void Communicator2D<T>::AddCell(
    gsl::index localCuboidIndex
  , gsl::index xIndex
  , gsl::index yIndex)
{
  mNeighborhood[localCuboidIndex].AddInCell(xIndex, yIndex);
}

template<typename T>
void Communicator2D<T>::AddCells(std::size_t overlap)
{
  for (gsl::index i {0}; i < mrSuperStructure.rGetLoadBalancer().GetSize();
      ++i) {
    mNeighborhood[i].AddInCells(overlap);
  }
}

template<typename T>
void Communicator2D<T>::Initialize()
{
  Reset();
  for (gsl::index local_idx {0};
      local_idx < mrSuperStructure.rGetLoadBalancer().GetSize();
      ++local_idx) {
    mNeighborhood[local_idx].InitializeInNeighbor();
    for (gsl::index i {0}; i < mNeighborhood[local_idx].GetInCellsSize();
        ++i) {
      auto global_idx {mNeighborhood[local_idx].rGetInCell(i)
          .mGlobalCuboidIndex};
      if (MpiManager::Instance().GetRank() ==
          mrSuperStructure.rGetLoadBalancer().GetRank(global_idx)) {
        Cell2D<T> tmp_cell;
        tmp_cell.mPhysR = mNeighborhood[local_idx].rGetInCell(i).mPhysR;
        mrSuperStructure.rGetCuboidGeometry().GetLatticeR(tmp_cell.mPhysR,
            tmp_cell.mGlobalCuboidIndex, tmp_cell.mLatticeR);
        tmp_cell.mGlobalCuboidIndex = mrSuperStructure.rGetLoadBalancer()
            .GetGlobalIndex(local_idx);
        mNeighborhood[mrSuperStructure.rGetLoadBalancer().GetLocalIndex(
            global_idx)].AddExCell(tmp_cell);
      }
    }
  }

  for (auto& r_it : mNeighborhood) r_it.InitializeExNeighbor();

#ifdef IBLBM_PARALLEL_MPI
  for (auto& r_it : mNeighborhood) r_it.FinishComm();
  for (auto& r_it : mNeighborhood) r_it.SendInDataCoordinates();
  for (auto& r_it : mNeighborhood) r_it.ReceiveExDataCoordinates();
  for (auto& r_it : mNeighborhood) r_it.FinishComm();
#endif
}

template<typename T>
void Communicator2D<T>::Reset()
{
  if (mIsInitialized) {
    for (auto& r_it : mNeighborhood) r_it.Reset();
    mIsInitialized = false;
  }
}

// explicit instantiation
template class Communicator2D<double>;
}  // namespace iblbm
