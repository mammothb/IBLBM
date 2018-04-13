#include "AbstractBlockGeometryInterface2D.hpp"

#include <iostream>

namespace iblbm
{
template<typename T>
AbstractBlockGeometryInterface2D<T>::AbstractBlockGeometryInterface2D(
    gsl::index globalCuboidIndex/*=-1*/)
  : mGlobalCuboidIndex{globalCuboidIndex},
//    mStatistics(this),
    mOstream{std::cout, "AbstractBlockGeometryInterface2D"}
{}
}  // namespace iblbm
