#include "CuboidGeometry2D.hpp"

#include <iostream>

namespace iblbm
{
template<typename T>
CuboidGeometry2D<T>::CuboidGeometry2D()
  : mCuboids{},
    mMotherCuboid{0, 0, 0, 0, 0},
    mOstream{std::cout, "CuboidGeometry2D"}
{
  ;
}
}  // namespace iblbm
