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

  /**
   * \return Read-only access to the global cuboid number which is given
   * not -1 if the block geometries are part of a super geometry
   */
  virtual const gsl::index& rGetGlobalCuboidIndex() const;

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
      T physR[2]
    , const gsl::index& rXIndex
    , const gsl::index& rYIndex) const = 0;

  /**
   * Transforms lattice to physical coordinates (wrapped from cuboid
   * geometry)
   */
  virtual void GetPhysR(
      T physR[2]
    , const gsl::index latticeR[2]) const;

  /** returns the (x, y) entry in the 2D scalar field */
  virtual gsl::index GetMaterial(
      gsl::index x
    , gsl::index y) const = 0;

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
