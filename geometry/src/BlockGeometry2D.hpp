#ifndef GEOMETRY_SRC_BLOCKGEOMETRY2D_HPP_
#define GEOMETRY_SRC_BLOCKGEOMETRY2D_HPP_

#include "AbstractBlockGeometryInterface2D.hpp"
#include "BlockData2D.hpp"
#include "Cuboid2D.hpp"

namespace iblbm
{
template<typename T>
class BlockGeometry2D final
  : public BlockData2D<T, std::size_t>
  , public AbstractBlockGeometryInterface2D<T>
{
 public:
  /**
   * Constructor
   *
   * \param globalXPos global x-position of lower left corner
   * \param globalYPos global y-position of lower left corner
   * \param deltaR node spacing
   * \param nx number of nodes in the x-direction
   * \param ny number of nodes in the y-direction
   */
  BlockGeometry2D(
      T globalXPos
    , T globalYPos
    , T deltaR
    , std::size_t nx
    , std::size_t ny
    , gsl::index globalCuboidIndex = -1);

 private:
  /** Cuboid which characterizes the block geometry */
  Cuboid2D<T> mCuboid;
};
}  // namespace iblbm

#endif  // GEOMETRY_SRC_BLOCKGEOMETRY2D_HPP_
