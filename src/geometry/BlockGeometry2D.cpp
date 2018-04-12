#include "BlockGeometry2D.hpp"

namespace iblbm
{
template<typename T>
BlockGeometry2D<T>::BlockGeometry2D(
    T globalXPos
  , T globalYPos
  , T deltaR
  , std::size_t nx
  , std::size_t ny
  , gsl::index globalCuboidIndex/*=-1*/)
  : BlockData2D<T, std::size_t>{nx, ny},
    AbstractBlockGeometryInterface2D<T>{globalCuboidIndex},
    mCuboid{globalXPos, globalYPos, deltaR, nx, ny}
{
  ;
}
}  // namespace iblbm
