#include "SuperGeometry2D.hpp"

namespace iblbm
{
template<typename T>
SuperGeometry2D<T>::SuperGeometry2D(
    CuboidGeometry2D<T>& rCuboidGeometry
  , LoadBalancer<T>& rLoadBalancer
  , std::size_t overlap/*=2*/)
  : SuperStructure2D<T>{rCuboidGeometry, rLoadBalancer, overlap},
    mStatistics{this},
    mOstream{std::cout, "SuperGeometry2D"}
{
  this->mCommunicator.InitializeNeighborhood();
  this->mCommunicator.AddCells(this->mOverlap);
  this->mCommunicator.Initialize();
  this->mNeedCommunication = true;
}

template<typename T>
bool* SuperGeometry2D<T>::operator()(
    gsl::index localCuboidIndex
  , gsl::index xIndex
  , gsl::index yIndex
  , gsl::index dataIndex)
{
  return nullptr;
//  return (bool*)&getExtendedBlockGeometry(localCuboidIndex).get(xIndex + this->mOverlap,
//      yIndex + this->mOverlap);
}

template<typename T>
std::size_t SuperGeometry2D<T>::GetDataSize() const
{
  return 1;
}

template<typename T>
std::size_t SuperGeometry2D<T>::GetSizeofDataType() const
{
  return sizeof(std::size_t);
}

template<typename T>
BlockGeometry2D<T>& SuperGeometry2D<T>::rGetExtendedBlockGeometry(
    gsl::index localCuboidIndex)
{
//  mStatistics.getStatisticsStatus() = true;
  return mExtendedBlockGeometries[localCuboidIndex];
}
// explicit instantiation
template class SuperGeometry2D<double>;
}  // namespace iblbm
