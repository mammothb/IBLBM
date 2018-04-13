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
   * \param p the number of cuboid to be split into
   */
  void Split(
      gsl::index index
    , std::size_t p);

  /**
   * Shrink all cuboids so that no empty planes are left
   */
  void Shrink(AbstractIndicatorFunctor2D<T>& rIndicatorFunctor);

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

  /** \return read and write acces to the i-th cuboid */
  Cuboid2D<T>& rGetCuboid(gsl::index i);

  /** \return read-only acces to the i-th cuboid */
  const Cuboid2D<T>& rGetCuboid(gsl::index i) const;

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

  /** \return the minimum coordinate in the structure */
  Vector2D<T> GetMinPhysR() const;

  /** \return the maximum coordinate in the structure */
  Vector2D<T> GetMaxPhysR() const;

  /** \return the maximum/minimum delata in the structure */
  T GetMinDeltaR() const;

  /** \return the number of cuboids in the structure */
  std::size_t GetNumberOfCuboids() const;

  /**
   * \return the smallest cuboid that includes all cuboids of the structure
   */
  Cuboid2D<T> GetMotherCuboid() const;

  /**
   * For a given point (globalXPos, globalYPos), returns the related
   * mGlobalCuboidIndex or 0 if the point is not in any of the cuboid
   */
  gsl::index GetGlobalCuboidIndex(
      T globalXPos
    , T globalYPos
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
