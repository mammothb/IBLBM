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
  for (gsl::index i {0}; i < mrSuperStructure.rGetLoadBalancer().GetSize();
      ++i) {
    CuboidNeighborhood2D<T> neighborhood {mrSuperStructure,
        mrSuperStructure.rGetLoadBalancer().GetGlobalIndex(i)};
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
//  Reset();
//  for (gsl::index global_idx {0};
//      global_idx < mrSuperStructure.rGetLoadBalancer().GetSize();
//      ++global_idx) {
//    mNeighborhood[global_idx].InitializeInNeighbor();
//    for (gsl::index i {0}; i < mNeighborhood[global_idx].GetInCellsSize();
//        ++i) {
//      auto global_cuboid_idx {
//          mNeighborhood[global_idx].rGetInCell(i).mGlobalCuboidIndex};
//#ifdef IBLBM_PARALLEL_MPI
//      if (MpiManager::Instance().GetRank() ==
//          mrSuperStructure.rGetLoadBalancer().GetRank(global_cuboid_idx))
//#endif  // IBLBM_PARALLEL_MPI
//      {
//        Cell2D<T> tmp_cell;
//        tmp_cell.mPhysR = _mNeighborhood[global_idx].rGetInCell(i).mPhysR;
//        mrSuperStructure.rGetCuboidGeometry().getLatticeR(tmp_cell.mPhysR, tmp_cell.latticeR);
//        tmp_cell.latticeR[0]    = _superStructure.getLoadBalancer().glob(iC);
//        _nh[_superStructure.getLoadBalancer().loc(ID)].add_outCell(tmp_cell);
//      }
//    }
//  }

//  for (int iC=0; iC<_superStructure.getLoadBalancer().size(); iC++) {
//    _nh[iC].init_outCN();
//  }
//
//#ifdef PARALLEL_MODE_MPI
//  for (int iC=0; iC<_superStructure.getLoadBalancer().size(); iC++) {
//    _nh[iC].finish_comm();
//  }
//  for (int iC=0; iC<_superStructure.getLoadBalancer().size(); iC++) {
//    _nh[iC].bufSend_inCells();
//  }
//  for (int iC=0; iC<_superStructure.getLoadBalancer().size(); iC++) {
//    _nh[iC].recWrite_outCells();
//  }
//  for (int iC=0; iC<_superStructure.getLoadBalancer().size(); iC++) {
//    _nh[iC].finish_comm();
//  }
//#endif
}

template<typename T>
void Communicator2D<T>::Reset()
{
  if (mIsInitialized) {
    for (gsl::index i {0}; i < mrSuperStructure.rGetLoadBalancer().GetSize();
        ++i) {
      mNeighborhood[i].Reset();
    }
    mIsInitialized = false;
  }
}

// explicit instantiation
template class Communicator2D<double>;
}  // namespace iblbm
