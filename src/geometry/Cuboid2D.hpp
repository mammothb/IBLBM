#ifndef SRC_GEOMETRY_CUBOID2D_HPP_
#define SRC_GEOMETRY_CUBOID2D_HPP_

#include <cstddef>

#include "OstreamManager.hpp"
#include "Vector.hpp"

namespace iblbm
{
template<typename T>
class Cuboid2D
{
 public:
  /**
   * Constructor of a cuboid
   *
   * \param globalXPos global x-position of lower left corner
   * \param globalYPos global y-position of lower left corner
   * \param deltaR node spacing
   * \param nx number of nodes in the x-direction
   * \param ny number of nodes in the y-direction
   * \param refinementLevel coefficient used to reduce deltaR
   */
  Cuboid2D(
      T globalXPos
    , T globalYPos
    , T deltaR
    , std::size_t nx
    , std::size_t ny
    , int refinementLevel = 0);

  /**
   * Constructor, using Vector
   *
   * \param rOrigin coordinates of lower left corner
   * \param deltaR node spacing
   * \param rExtent number of nodes in the x- and y- direction (not to be
   *        confused with physical extent in IndicatorFunctor)
   * \param refinementLevel coefficient used to reduce deltaR
   */
  Cuboid2D(
      const Vector2D<T>& rOrigin
    , T deltaR
    , const Vector2D<std::size_t>& rExtent
    , int refinementLevel = 0);

  /** \return Global x-position of lower left corner */
  T GetGlobalXPosition() const;

  /** \return Global y-position of lower left corner */
  T GetGlobalYPosition() const;

  /** \return read-only access to coordinates of lower left corner */
  const Vector2D<T> GetOrigin() const;

  /** \return node spacing */
  T GetDeltaR() const;

  /** \return number of nodes in the x-direction */
  std::size_t GetNx() const;

  /** \return number of nodes in the y-direction */
  std::size_t GetNy() const;

  /** \return read-only access number of nodes in the x- and y- direction */
  const Vector2D<std::size_t> GetLatticeExtent() const;

  /** \return number of full cells of the cuboid for load balancer */
  std::size_t GetWeight() const;

  /** \return the perimeter of the cuboid */
  T GetPhysPerimeter() const;

  /** \return the number of nodes at the perimeter of the cuboid */
  std::size_t GetLatticePerimeter() const;

  /** \return the perimeter of the cuboid */
  T GetPhysVolume() const;

  /** \return the perimeter of the cuboid */
  std::size_t GetLatticeVolume() const;

  /** set the number of full cells of the cuboid for load balancer */
  void SetWeight(std::size_t weight);

  /**
   * Output cuboid details
   */
  void Print() const;

 private:
  /** global x-position of lower left corner */
  T mGlobalXPos;
  /** global y-position of lower left corner */
  T mGlobalYPos;
  /** node spacing */
  T mDeltaR;
  /** number of nodes in the x-direction */
  std::size_t mNx;
  /** number of nodes in the y-direction */
  std::size_t mNy;
  /** Number of full cells, used by load balancer */
  std::size_t mWeight;
  /** Refinement level */
  int mRefinementLevel;
  /** Special ostream that prepends class name in each line */
  mutable OstreamManager mOstream;
};
}

#endif  // SRC_GEOMETRY_CUBOID2D_HPP_
