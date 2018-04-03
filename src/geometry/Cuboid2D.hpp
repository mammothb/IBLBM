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
   * \param rLatticeExtent number of nodes in the x- and y- direction (not to
   *        be confused with physical extent in IndicatorFunctor)
   * \param refinementLevel coefficient used to reduce deltaR
   */
  Cuboid2D(
      const Vector2D<T>& rOrigin
    , T deltaR
    , const Vector2D<std::size_t>& rLatticeExtent
    , int refinementLevel = 0);

  /**
   * Divides the cuboid in numCol * numRow cuboids and adds them to the
   * given vector. The cuboid is inserted in the following order, e.g.
   * (nx, ny) = (2, 3)
   * rCuboids <- (0, 0), (0, 1), (0, 2), (1, 0), (1, 1), (1, 2)
   *
   * \param nx number of columns
   * \param ny number of rows
   * \param rCuboids the vector to insert the split cuboids into
   */
  void Divide(
      std::size_t numCol
    , std::size_t numRow
    , std::vector<Cuboid2D<T>>& rCuboids) const;

  /**
   * Divides the cuboid into p cuboids and adds them to the rCuboids vector.
   * If the optimal layout for child cuboid is less than q, we 1 extra cuboid
   * to the last remainder row/col of the layout,
   * e.g., p = 7, num_col = 2, num_row = 3, we have the following layout
   * | 4 | 5 | 6 |
   * |  2  |  3  |
   * |  0  |  1  |
   *
   * \param p the number of cuboids to be split into
   * \param rCuboids the vector to insert the split cuboids into
   */
  void Divide(
      std::size_t p
    , std::vector<Cuboid2D<T>>& rCuboids) const;

  /**
   * Resize the cuboid to the desired size
   */
  void Resize(
      gsl::index xIndex
    , gsl::index yIndex
    , std::size_t nx
    , std::size_t ny);

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

  /** \return the physical position of a given node in the cuboid */
  Vector2D<T> GetPhysR(
      const gsl::index& rXIndex
    , const gsl::index& rYIndex) const;

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
