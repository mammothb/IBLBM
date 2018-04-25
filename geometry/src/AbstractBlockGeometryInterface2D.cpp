#include "AbstractBlockGeometryInterface2D.hpp"

#include <iostream>

namespace iblbm
{
template<typename T>
AbstractBlockGeometryInterface2D<T>::AbstractBlockGeometryInterface2D(
    gsl::index globalCuboidIndex/*=-1*/)
  : mGlobalCuboidIndex{globalCuboidIndex},
    mStatistics{this},
    mOstream{std::cout, "AbstractBlockGeometryInterface2D"}
{}

template<typename T>
AbstractBlockGeometryInterface2D<T>::~AbstractBlockGeometryInterface2D()
{}

template<typename T>
const gsl::index& AbstractBlockGeometryInterface2D<T>::
    rGetGlobalCuboidIndex() const
{
  return mGlobalCuboidIndex;
}

template<typename T>
void AbstractBlockGeometryInterface2D<T>::GetPhysR(
    T physR[2]
  , const gsl::index latticeR[2]) const
{
  GetPhysR(physR, latticeR[0], latticeR[1]);
}


// explicit instantiation
template class AbstractBlockGeometryInterface2D<double>;
}  // namespace iblbm
