#ifndef GEOMETRY_SRC_ABSTRACTBLOCKGEOMETRYINTERFACE2D_HPP_
#define GEOMETRY_SRC_ABSTRACTBLOCKGEOMETRYINTERFACE2D_HPP_

#include <array>
#include <list>

#include "gsl/gsl"
#include "BlockGeometryStatistics2D.hpp"
#include "IndicatorFunctor2D.hpp"
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

  /** \return Write access to a material number */
  virtual int& rGetMaterial(
      gsl::index xIndex
    , gsl::index yIndex) = 0;

  /** \return Read-only access to a material number */
  virtual const int& rGetMaterial(
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

  /** \return Write access to a material number */
  virtual int& rGetMaterial(const std::vector<gsl::index>& rLatticeR);

  /** \return Read only access to a material number */
  virtual const int& rGetMaterial(
      const std::vector<gsl::index>& rLatticeR) const;

  /**
   * Changes all materials which are not 0 or 1 to 0 if there is no neighbor
   * with material 1. This removes things like a block of boundary nodes
   * since keeping just its outer shell has the same hydrodynamic effect
   */
  virtual std::size_t Clean(bool isVerbose = true);

  /**
   * Changes all materials with material 1 to 0 if there is a neighbor with
   * material 0
   */
  virtual std::size_t OuterClean(bool isVerbose = true);

  /**
   * Changes all materials which are not 0 or 1 to 1 if there is a non robust
   * constiallation
   */
  virtual std::size_t InnerClean(bool isVerbose = true);

  /**
   * Changes all materials with material fromM to 1 if there is a non robust
   * constiallation
   */
  virtual std::size_t InnerClean(
      int fromMaterial
    , bool isVerbose = true);

  /**
   * Checks for errors (searches for all outer voxels (=0) with an inner
   * voxel (=1) as a direct neighbor)
   */
  virtual bool CheckForErrors(bool isVerbose = true) const;

  /** Replaces all material numbers (fromMaterial) to another (toMaterial) */
  virtual void Rename(
      int fromMaterial
    , int toMaterial);

  /**
   * Replaces all material numbers (fromMaterial) to another (toMaterial) if
   * an indicator functor condition is fulfilled
   */
  virtual void Rename(
      int fromMaterial
    , int toMaterial
    , AbstractIndicatorFunctor2D<T>& rCondition);

  /**
   * Replaces all material numbers (fromMaterial) to another (toMaterial) if
   * all materials in the neighborhood (x - xOffset ... x + xOffset), are of
   * the original material number (fromMaterial)
   */
  virtual void Rename(
      int fromMaterial
    , int toMaterial
    , unsigned xOffset
    , unsigned yOffset);

  /**
   * Replaces all material numbers (fromMaterial) to another (toMaterial) if
   * all materials in (x + 1 ... x + rTestDirection[0]) are of another
   * material number (testMaterial)
   */
  virtual void Rename(
      int fromMaterial
    , int toMaterial
    , int testMaterial
    , const std::vector<int>& rTestDirection);

  /**
   * Replaces all material numbers (fromMaterial) to another (toMaterial) if
   * all materials in (x + rDiscreteNormal[0], x + 2 * rDiscreteNormal[0])
   * are of another material number (fluidMaterial) and if an indicator
   * functor condition is fulfilled
   */
  virtual void Rename(
      int fromMaterial
    , int toMaterial
    , int fluidMaterial
    , AbstractIndicatorFunctor2D<T>& rCondition
    , const std::vector<int>& rDiscreteNormal);

  /**
   * Replaces all material numbers (fromMaterial) to another (toMaterial) if
   * all materials in (iX+discreteNormal[0],iX+2*discreteNormal[0]), are of
   * another material number (fluidM) and if an indicator functor condition
   * is fulfilled, the discreteNormal is computed from all fromMaterial which
   * fulfill the indicator functor condition
   */
  virtual void Rename(
      int fromMaterial
    , int toMaterial
    , int fluidMaterial
    , AbstractIndicatorFunctor2D<T>& rCondition);

//  /**
//   * Replaces all material numbers (fromMaterial) to another (toMaterial)
//   * using a seed point and max. directions indicated by xOffset,yOffset != 0
//   */
//  virtual void RegionGrowing(
//      int fromMaterial
//    , int toMaterial
//    , int xSeed
//    , int ySeed
//    , int xOffset
//    , int yOffset
//    , std::map<std::vector<int>, int>* pTmp = nullptr);

  /**
   * Returns true if there is at least one stream direction found for a voxel
   * with coordinates (xIndex, yIndex) of material and if the neighboring
   * material is one of the rBulkMaterials in the list. It further fills the
   * array streamDirection with true if this direction is pointing to a
   * bulkMaterial.
   *
   * \param xIndex lattice index in x direction
   * \param yIndex lattice index in y direction
   * \param material target material
   * \param rBulkMaterials a list of material numbers which are considered as
   *        fluid and can be streamed to
   * \param rStreamDirections
   */
  template<typename V, template<typename U> class Lattice>
  bool FindStreamDirections(
      gsl::index xIndex
    , gsl::index yIndex
    , int material
    , const std::list<int>& rBulkMaterials
    , std::array<bool, Lattice<V>::sQ>& rStreamDirections);

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
