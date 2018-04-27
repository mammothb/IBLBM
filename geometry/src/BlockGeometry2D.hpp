#ifndef GEOMETRY_SRC_BLOCKGEOMETRY2D_HPP_
#define GEOMETRY_SRC_BLOCKGEOMETRY2D_HPP_

#include <list>

#include "AbstractBlockGeometryInterface2D.hpp"
#include "BlockData2D.hpp"
#include "Cuboid2D.hpp"

namespace iblbm
{
/**
 * This class is derived from block geometry structure. It holds the actual
 * data with the materials. It stores pointers to all dependent block
 * geometry views. It presents a volume of voxels where different types are
 * given by material numbers which is important e.g. to work with different
 * boundaries (like for inflow/output regions).
 */
template<typename T>
class BlockGeometry2D final
  : public BlockData2D<T, int>
  , public AbstractBlockGeometryInterface2D<T>
{
  friend class TestBlockGeometry2D;
 public:
  /**
   * Constructor
   *
   * \param globalXPos global x-position of lower left corner
   * \param globalYPos global y-position of lower left corner
   * \param deltaR node spacing
   * \param nx number of nodes in the x-direction
   * \param ny number of nodes in the y-direction
   * \param globalCuboidIndex cuboid number
   */
  BlockGeometry2D(
      T xPosition
    , T yPosition
    , T deltaR
    , std::size_t nx
    , std::size_t ny
    , gsl::index globalCuboidIndex = -1);

  /**
   * Constructor
   *
   * \param rCuboid cuboid structure
   * \param globalCuboidIndex cuboid number
   */
  BlockGeometry2D(
      const Cuboid2D<T>& rCuboid
    , gsl::index globalCuboidIndex = -1);

  /**
   * Prints a chosen part of the block geometry
   *
   * \param xIndex0 start x index
   * \param yIndex0 start y index
   * \param xIndex1 end x index
   * \param yIndex1 end y index
   * \param showLineNumber flag to determine if we print the line number
   */
  void PrintLayer(
      gsl::index xIndex0
    , gsl::index yIndex0
    , gsl::index xIndex1
    , gsl::index yIndex1
    , bool showLineNumber = false);

  /**
   * Prints a chosen part of the block geometry
   *
   * \param direction
   */
  void PrintLayer(
      int direction
    , int layer
    , bool showLineNumber = false);

  /**
   * Prints a chosen node and its neighborhood
   */
  void PrintNode(
      gsl::index xIndex
    , gsl::index yIndex);

  /** \return Read and write access to the statistic object */
  BlockGeometryStatistics2D<T>& rGetStatistics(
      bool isVerbose = true) override;

  /** \return Read-only access to the statistic object */
  const BlockGeometryStatistics2D<T>& rGetStatistics(
      bool isVerbose = true) const override;

  /**
   * \return the position of the block origin which is the node
   * (x = 0, y = 0) in physical units (meter)
   */
  Vector2D<T> GetOrigin() const override;

  /** \return the lattice extent in x direction of the block */
  std::size_t GetNx() const override;

  /** \return the lattice extent in y direction of the block */
  std::size_t GetNy() const override;

  /** \return physical node spacing */
  const T GetDeltaR() const override;

  /**
   * Transforms lattice to physical coordinates (wrapped from cuboid
   * geometry)
   */
  void GetPhysR(
      const gsl::index& rXIndex
    , const gsl::index& rYIndex
    , T physR[2]) const override;

  /** returns the (x, y) entry in the 2D scalar field */
  int GetMaterial(
      gsl::index xIndex
    , gsl::index yIndex) const override;

  /** \return Write access to a material number */
  int& rGetMaterial(
      gsl::index xIndex
    , gsl::index yIndex) override;

  /** \return Read-only access to a material number */
  const int& rGetMaterial(
      gsl::index xIndex
    , gsl::index yIndex) const;

  /**
   * Adds a pointer to the list of dependent statistic classes
   */
  void AddToStatisticsList(bool* pStatisticStatus) override;
  /**
   * Removes a pointer from the list of dependent statistic classes if
   * existing
   */
  void RemoveFromStatisticsList(bool* pStatisticStatus) override;

 private:
  void ResetStatistics();

  /** Cuboid which characterizes the block geometry */
  Cuboid2D<T> mCuboid;
  /** List to all depending statistic status objects */
  std::list<bool*> mNeedStatisticsUpdate;
};
}  // namespace iblbm

#endif  // GEOMETRY_SRC_BLOCKGEOMETRY2D_HPP_
