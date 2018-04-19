#ifndef GEOMETRY_SRC_CUBOIDGEOMETRY2D_HPP_
#define GEOMETRY_SRC_CUBOIDGEOMETRY2D_HPP_

#include <vector>

#include "gsl/gsl"
#include "Cuboid2D.hpp"
#include "OstreamManager.hpp"

namespace iblbm
{
template<typename T> class AbstractIndicatorFunctor2D;
/**
 * Cuboid geometry contains one or multiple cuboid. To represent a connected
 * domain, the distance between two neighboring cuboid must be less than the
 * smallest delta R between them.
 *
 * \param mMotherCuboid cuboid which contain all other cuboids
 * \param mCuboids vector of cuboids
 */
template<typename T>
class CuboidGeometry2D
{
  friend class TestCuboidGeometry2D;
 public:
  /**
   * Constructor, creates an empty cuboid
   */
  CuboidGeometry2D();

  /**
   * Constructs a cuboid geometry with a cubic shape of size nx times ny
   * times 1 with origin originR=(originX, originY, 0) that consists of
   * nc cuboids
   *
   * \param originX x-coordinate of cuboid origin
   * \param originY y-coordinate of cuboid origin
   * \param deltaR node spacing
   * \param nx number of nodes in the x-direction
   * \param ny number of nodes in the y-direction
   * \param nc number of sub/child cuboids
   */
  CuboidGeometry2D(
      T originX
    , T originY
    , T deltaR
    , std::size_t nx
    , std::size_t ny
    , std::size_t nc = 1);

  /**
   * Constructs a cuboid structure with a uniform spacing of deltaR which
   * consists of nc cuboid, the cuboid not needed are removed and too big
   * ones are shrunk
   *
   * \param rIndicatorFunctor indicator functor to set the area/volume of the
   *        cuboid
   * \param deltaR node spacing
   * \param nc number of sub/child cuboids
   */
  CuboidGeometry2D(
      AbstractIndicatorFunctor2D<T>& rIndicatorFunctor
    , T deltaR
    , std::size_t nc = 1);

  /** Add a cuboid to the end of mCuboids */
  void Add(Cuboid2D<T> cuboid);

  /** Remove the cuboid at the given index */
  void Remove(gsl::index index);

  /**
   * Splits cuboid at index, removes it and add p cuboids
   *
   * \param index the cuboid to be split and removed
   * \param numCuboid the number of cuboid to be split into
   */
  void Split(
      gsl::index index
    , std::size_t numCuboid);

  /**
   * Shrink all cuboids so that no empty planes are left
   */
  void Shrink(AbstractIndicatorFunctor2D<T>& rIndicatorFunctor);

  /**
   * Refine the target area of refinementLevel to one level higher
   *
   * \param xPosition0 x coordinate of lower left corner of target area
   * \param yPosition0 y coordinate of lower left corner of target area
   * \param xPosition1 x coordinate of upper right corner of target area
   * \param yPosition1 y coordinate of upper right corner of target area
   * \param refinementLevel target refinement level
   */
  void RefineArea(
      T xPosition0
    , T yPosition0
    , T xPosition1
    , T yPosition1
    , int refinementLevel);

  /**
   * Returns true and the cuboid number of the nearest lattice position to
   * the given physical position if the physical position is within any of
   * the cuboids with an overlap of 1/2 * mDeltaR belonging to the cuboid
   * geometry
   *
   * \param rPhysR vector to the physical location
   * \param rGlobalCuboidIndex
   */
  bool HasCuboid(
      const Vector2D<T>& rPhysR
    , gsl::index& rGlobalCuboidIndex);

  /**
   * If rPhysR is contained within any cuboid with an overlap of 0.5 mDeltaR
   * which belongs to the cuboid geometry, return true and write the cuboid
   * index to rIndex
   */
  bool HasCuboid(
      const std::vector<T>& rPhysR
    , gsl::index& rGlobalCuboidIndex) const;

  /** \return read and write acces to the i-th cuboid */
  Cuboid2D<T>& rGetCuboid(gsl::index i);

  /** \return read-only acces to the i-th cuboid */
  const Cuboid2D<T>& rGetCuboid(gsl::index i) const;

  /**
   * If rPhysR is contained within any cuboid with an overlap of 0.5 * mDeltaR
   * which belongs to the cuboid geometry, return true and write the nearest
   * lattice position
   */
  bool GetLatticeR(
      const std::vector<T>& rPhysR
    , gsl::index& rGlobalCuboidIndex
    , std::vector<gsl::index>& rLatticeR) const;

  /**
   * If rPhysR is contained within any cuboid with an overlap of 0.5 * mDeltaR
   * which belongs to the cuboid geometry, return true and write the nearest
   * lattice position
   */
  bool GetLatticeR(
      const T physR[]
    , gsl::index& rGlobalCuboidIndex
    , gsl::index latticeR[]) const;

  /**
   * If rPhysR is contained within any cuboid with an overlap of 0.5 * mDeltaR
   * which belongs to the cuboid geometry, return true and write the floored
   * lattice position
   */
  bool GetFloorLatticeR(
      const std::vector<T>& rPhysR
    , gsl::index& rGlobalCuboidIndex
    , std::vector<gsl::index>& rLatticeR) const;

  /**
   * If rPhysR is contained within any cuboid with an overlap of 0.5 * mDeltaR
   * which belongs to the cuboid geometry, return true and write the floored
   * lattice position
   */
  bool GetFloorLatticeR(
      const Vector2D<T>& rPhysR
    , gsl::index& rGlobalCuboidIndex
    , Vector2D<gsl::index>& rLatticeR) const;

  /**
   * \return the physical position to the given lattice position respecting
   * periodicity for the overlap nodes which are not in the mother cuboid for
   * the case the flag mIsPeriodic[d] = true if the physical position is
   * within any of the cuboids with an overlap of 1/2 * deltaR belonging to
   * the cuboid geometry
   */
  Vector2D<T> GetPhysR(
      gsl::index globalCuboidIndex
    , gsl::index xIndex
    , gsl::index yIndex) const;

  /**
   * \return the physical position to the given lattice position respecting
   * periodicity for the overlap nodes which are not in the mother cuboid for
   * the case the flag mIsPeriodic[d] = true if the physical position is
   * within any of the cuboids with an overlap of 1/2 * deltaR belonging to
   * the cuboid geometry
   */
  Vector2D<T> GetPhysR(
      gsl::index globalCuboidIndex
    , const std::vector<gsl::index>& rLatticeR) const;

  /** \return the minimum coordinate in the structure */
  Vector2D<T> GetMinPhysR() const;

  /** \return the maximum coordinate in the structure */
  Vector2D<T> GetMaxPhysR() const;

  /** \return the number of cuboids in the structure */
  std::size_t GetNumberOfCuboids() const;

  /** \return the minimum of the ratio mNx / mNy in the structure */
  T GetMinRatio() const;

  /** \return the maximum of the ratio mNx / mNy in the structure */
  T GetMaxRatio() const;

  /** \return Returns the minimum volume in the structure */
  T GetMinPhysVolume() const;

  /** \return Returns the maximum volume in the structure */
  T GetMaxPhysVolume() const;

  /** \return the minimum number of nodes in the structure */
  std::size_t GetMinLatticeVolume() const;

  /** \return the maximum number of nodes in the structure */
  std::size_t GetMaxLatticeVolume() const;

  /** \return the minimum delta in the structure */
  T GetMinDeltaR() const;

  /** \return the maximum delta in the structure */
  T GetMaxDeltaR() const;

  /**
   * \return the smallest cuboid that includes all cuboids of the structure
   */
  Cuboid2D<T> GetMotherCuboid() const;

  /**
   * For a given point (xPosition, yPosition), returns the related
   * mGlobalCuboidIndex or 0 if the point is not in any of the cuboid
   */
  gsl::index GetGlobalCuboidIndex(
      T xPosition
    , T yPosition
    , std::size_t offset = 0) const;

  /** Set flag to enable/disable periodicity */
  void SetIsPeriodic(
      bool isPeriodicX
    , bool isPeriodicY);

 private:
  /** Cuboid which contains all other cuboids */
  Cuboid2D<T> mMotherCuboid;
  /** Vector of cuboids */
  std::vector<Cuboid2D<T>> mCuboids;
  /** Flag to indicate periodicity */
  std::vector<bool> mIsPeriodic;
  /** Special ostream that prepends class name in each line */
  mutable OstreamManager mOstream;
};
}  // namespace iblbm

#endif  // GEOMETRY_SRC_CUBOIDGEOMETRY2D_HPP_
