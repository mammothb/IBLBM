#ifndef GEOMETRY_SRC_SUPERGEOMETRY2D_HPP_
#define GEOMETRY_SRC_SUPERGEOMETRY2D_HPP_

#include "BlockGeometry2D.hpp"
#include "SuperStructure2D.hpp"

namespace iblbm
{
template<typename T>
class SuperGeometry2D : public SuperStructure2D<T>
{
 public:
  /**
   * Constructor
   *
   * \param rCuboidGeometry Reference to CuboidGeometry which holds the grid
   *        structure
   * \param rLoadBalancer Reference to LoadBalancer to evenly distribute the
   *        load across processes
   * \param overlap Size of ghost cell layer
   */
  SuperGeometry2D(
      CuboidGeometry2D<T>& rCuboidGeometry
    , LoadBalancer<T>& rLoadBalancer
    , std::size_t overlap = 2);

  /**
   * Interface for the communicator class: Write access to the memory of the
   * data of the super structure
   */
  bool* operator()(
      gsl::index localCuboidIndex
    , gsl::index xIndex
    , gsl::index yIndex
    , gsl::index dataIndex) override;

  /**
   * \return Read only access to the dim of the data of the super structure
   */
  std::size_t GetDataSize() const override;

  /**
   * \return Read only access to the data type dim of the data of the super
   * structure
   */
  std::size_t GetSizeofDataType() const override;

  /** Read and write access to a single extended block geometry */
  BlockGeometry2D<T>& rGetExtendedBlockGeometry(gsl::index localCuboidIndex);

 private:
  /** Vector of block geometries with overlap */
  std::vector<BlockGeometry2D<T>> mExtendedBlockGeometries;
  /** class specific output stream */
  mutable OstreamManager mOstream;
};
}  // namespace iblbm

#endif  // GEOMETRY_SRC_SUPERGEOMETRY2D_HPP_
