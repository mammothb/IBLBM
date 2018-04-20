#include "SuperGeometryStatistics2D.hpp"

#include <iostream>

#include "SuperGeometry2D.hpp"

namespace iblbm
{
template<typename T>
SuperGeometryStatistics2D<T>::SuperGeometryStatistics2D(
    SuperGeometry2D<T>* pSuperGeometry)
  : mpSuperGeometry{pSuperGeometry},
    mNeedUpdate{true},
//    mOverlap{pSuperGeometry->GetOverlap()},
    mOstream{std::cout, "SuperGeometryStatistics2D"}
{}

// explicit instantiation
template class SuperGeometryStatistics2D<double>;
}  // namespace iblbm
