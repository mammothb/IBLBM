#ifndef GEOMETRY_SRC_ABSTRACTBLOCKGEOMETRYINTERFACE2D_HPP_
#define GEOMETRY_SRC_ABSTRACTBLOCKGEOMETRYINTERFACE2D_HPP_

#include "gsl/gsl"
#include "BlockGeometryStatistics2D.hpp"
#include "OstreamManager.hpp"
#include "Vector.hpp"

namespace iblbm
{
template<typename T>
class AbstractBlockGeometryInterface2D
{
 public:
  /**
   * Constructor
   */
  explicit AbstractBlockGeometryInterface2D(
      gsl::index globalCuboidIndex = -1);

  /**
   * Virtual destructor
   */
  virtual ~AbstractBlockGeometryInterface2D();

  /** \return Read and write access to the statistic object */
  virtual BlockGeometryStatistics2D<T>& rGetStatistics(
      bool isVerbose = true) = 0;

  /** \return Read-only access to the statistic object */
  virtual const BlockGeometryStatistics2D<T>& rGetStatistics(
      bool isVerbose = true) const = 0;

  /**
   * \return the position of the block origin which is the node
   * (x = 0, y = 0) in physical units (meter)
   */
  virtual Vector2D<T> GetOrigin() const = 0;

  /** \return the lattice extent in x direction of the block */
  virtual std::size_t GetNx() const = 0;

  /** \return the lattice extent in y direction of the block */
  virtual std::size_t GetNy() const = 0;

  /** \return physical node spacing */
  virtual const T GetDeltaR() const = 0;

  /**
   * Transforms lattice to physical coordinates (wrapped from cuboid
   * geometry)
   */
  virtual void GetPhysR(
      const gsl::index& rXIndex
    , const gsl::index& rYIndex
    , T physR[2]) const = 0;

  /** returns the (xIndex, yIndex) entry in the 2D scalar field */
  virtual int GetMaterial(
      gsl::index xIndex
    , gsl::index yIndex) const = 0;

  /**
   * Adds a pointer to the flag statistic status which is depended on the
   * data of an block geometry
   */
  virtual void AddToStatisticsList(bool* pStatisticStatus) = 0;

  /**
   * Removes a pointer if it exists from the flag statistic status which is
   * depended on the data of an block geometry
   */
  virtual void RemoveFromStatisticsList(bool* pStatisticStatus) = 0;

  /**
   * \return Read-only access to the global cuboid number which is given
   * not -1 if the block geometries are part of a super geometry
   */
  virtual const gsl::index& rGetGlobalCuboidIndex() const;

  /** \return the extent of the block in lattice units */
  virtual const Vector2D<gsl::index> GetLatticeExtent() const;

  /**
   * Transforms lattice to physical coordinates (wrapped from cuboid
   * geometry)
   */
  virtual void GetPhysR(
      const gsl::index latticeR[2]
    , T physR[2]) const;

 protected:
  /** global cuboid number, default = -1 */
  gsl::index mGlobalCuboidIndex;
  /** Statistics class */
  BlockGeometryStatistics2D<T> mStatistics;
  /** Class speciic output stream */
  mutable OstreamManager mOstream;
};
}  // namespace iblbm

#endif  // GEOMETRY_SRC_ABSTRACTBLOCKGEOMETRYINTERFACE2D_HPP_
