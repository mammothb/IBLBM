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
const Vector2D<gsl::index> AbstractBlockGeometryInterface2D<T>::
    GetLatticeExtent() const
{
  return Vector2D<gsl::index>{static_cast<gsl::index>(GetNx()),
      static_cast<gsl::index>(GetNy())};
}

template<typename T>
void AbstractBlockGeometryInterface2D<T>::GetPhysR(
    const gsl::index latticeR[2]
  , T physR[2]) const
{
  GetPhysR(latticeR[0], latticeR[1], physR);
}

// explicit instantiation
template class AbstractBlockGeometryInterface2D<double>;
}  // namespace iblbm
