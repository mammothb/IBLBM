#include "SuperStructure2D.hpp"

namespace iblbm
{
template<typename T>
SuperStructure2D<T>::SuperStructure2D(std::size_t overlap/*=1*/)
  : SuperStructure2D{*(new CuboidGeometry2D<T>{}), *(new LoadBalancer<T>{}),
        overlap}
{}

template<typename T>
SuperStructure2D<T>::~SuperStructure2D()
{}

template<typename T>
SuperStructure2D<T>::SuperStructure2D(
    CuboidGeometry2D<T>& rCuboidGeometry
  , LoadBalancer<T>& rLoadBalancer
  , std::size_t overlap/*=1*/)
  : mrCuboidGeometry{rCuboidGeometry},
    mrLoadBalancer{rLoadBalancer},
    mOverlap{overlap},
    mCommunicator{*this},
    mNeedCommunication{true},
    mOstream{std::cout, "SuperStructure2D"}
{}

template<typename T>
CuboidGeometry2D<T>& SuperStructure2D<T>::rGetCuboidGeometry()
{
  return mrCuboidGeometry;
}

template<typename T>
const CuboidGeometry2D<T>& SuperStructure2D<T>::rGetCuboidGeometry() const
{
  return mrCuboidGeometry;
}

// explicit instantiation
template class SuperStructure2D<double>;
}  // namespace iblbm
