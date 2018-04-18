#ifndef GEOMETRY_SRC_CUBOID2D_HPP_
#define GEOMETRY_SRC_CUBOID2D_HPP_

#include <cstddef>
#include <vector>

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
   * \param xPosition global x-position of lower left corner
   * \param yPosition global y-position of lower left corner
   * \param deltaR node spacing
   * \param nx number of nodes in the x-direction
   * \param ny number of nodes in the y-direction
   * \param refinementLevel coefficient used to reduce deltaR
   */
  Cuboid2D(
      T xPosition
    , T yPosition
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
   * Copy constructor
   */
  Cuboid2D(
      const Cuboid2D<T>& rRhs
    , std::size_t overlap = 0);

  /**
   * Copy assignment
   */
  Cuboid2D& operator=(const Cuboid2D& rRhs);

  /**
   * Initialize the cuboid
   *
   * \param xPosition global x-position of lower left corner
   * \param yPosition global y-position of lower left corner
   * \param deltaR node spacing
   * \param nx number of nodes in the x-direction
   * \param ny number of nodes in the y-direction
   * \param refinementLevel coefficient used to reduce deltaR
   */
  void Initialize(
      T xPosition
    , T yPosition
    , T deltaR
    , std::size_t nx
    , std::size_t ny
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
      std::size_t numCuboid
    , std::vector<Cuboid2D<T>>& rCuboids) const;

  /**
   * Resize the cuboid to the desired size
   */
  void Resize(
      gsl::index xIndex
    , gsl::index yIndex
    , std::size_t nx
    , std::size_t ny);

  /**
   * Checks whether a point (xPosition, yPosition) is contained in the
   * cuboid extended with an layer of size overlap * mDeltaR
   *
   * \param xPosition global x position
   * \param yPosition global y position
   * \param overlap size of overlap layer
   */
  bool ContainPoint(
      T xPosition
    , T yPosition
    , std::size_t overlap = 0) const;

  /**
   * Checks whether a point (xPosition, yPosition) is contained and is a
   * node in the cuboid extended with an layer of size overlap * mDeltaR and
   * returns the local active node
   *
   * \param xPosition global x position
   * \param yPosition global y position
   * \param rXIndex x index in the cuboid
   * \param rYIndex y index in the cuboid
   * \param overlap size of overlap layer
   */
  bool ContainPoint(
      T xPosition
    , T yPosition
    , gsl::index& rXIndex
    , gsl::index& rYIndex
    , std::size_t overlap = 0) const;

  /**
   * Checks whether the cuboid with an overlap layer intersection with another
   * cuboid volume defined by (xPosition0, yPosition0) and
   * (xPosition1, yPosition1)
   *
   * \param xPosition0 x position of lower left corner target cuboid volume
   * \param yPosition0 y position of lower left corner target cuboid volume
   * \param xPosition1 x position of upper right corner target cuboid volume
   * \param yPosition1 y position of upper right corner target cuboid volume
   */
  bool CheckIntersection(
      T xPosition0
    , T yPosition0
    , T xPosition1
    , T yPosition1
    , std::size_t overlap = 0) const;

  /**
   * Checks whether a given point intersects the cuboid extended
   * by a layer of size overlap * mDeltaR
   *
   * \param xPosition x position of given point
   * \param yPosition y position of given point
   */
  bool CheckIntersection(
      T xPosition
    , T yPosition
    , std::size_t overlap = 0) const;

  /**
   * Checks whether there is an intersection and returns the local
   * active node range which can be empty by means of rXIndex0 = 1,
   * rYIndex0 = 1, rXIndex1 = 0, rYIndex1 = 0, i.e., the lower left
   * coordinates is larger than the upper right coordinates of the cuboid
   * extended with an layer of size overlap * mDeltaR
   *
   * \param xPosition0 x position of lower left corner target cuboid volume
   * \param yPosition0 y position of lower left corner target cuboid volume
   * \param xPosition1 x position of upper right corner target cuboid volume
   * \param yPosition1 y position of upper right corner target cuboid volume
   * \param rXIndex0 x index in the cuboid of lower left corner of the
   *        intersection area
   * \param rYIndex0 y index in the cuboid of lower left corner of the
   *        intersection area
   * \param rXIndex1 x index in the cuboid of upper right corner of the
   *        intersection area
   * \param rYIndex1 y index in the cuboid of upper right corner of the
   *        intersection area
   */
  bool CheckIntersection(
      T xPosition0
    , T yPosition0
    , T xPosition1
    , T yPosition1
    , gsl::index& rXIndex0
    , gsl::index& rYIndex0
    , gsl::index& rXIndex1
    , gsl::index& rYIndex1
    , std::size_t overlap = 0) const;

  /**
   * Refines the cuboid with given refinement level. expects refinementLevel
   * to be larger then mRefinementLevel
   *
   * \param refinementLevel target refinement level
   */
  void RefineToLevel(int refinementLevel);

  /**
   * Refines one more level
   */
  void RefineIncrease();

  /**
   * Refines one less level
   */
  void RefineDecrease();

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

  /**
   * Compute the physical coordinate based on give lattice index
   *
   * \param physR output physical coordinate
   * \param latticeR input lattice index
   */
  void GetPhysR(
      const gsl::index latticeR[2]
    , T physR[2]) const;

  /**
   * Compute the physical coordinate based on give lattice index
   *
   * \param physR output physical coordinate
   * \param rXIndex input x lattice index
   * \param rYIndex input y lattice index
   */
  void GetPhysR(
      const gsl::index& rXIndex
    , const gsl::index& rYIndex
    , T physR[2]) const;

  /**
   * Compute the lattice index closest to the given physical coordinates
   * uses normal rounding
   *
   * \param latticeR output lattice index
   * \param physR input physical coordinates
   */
  void GetLatticeR(
      const T physR[2]
    , gsl::index latticeR[2]) const;

  /**
   * Compute the lattice index closest to the given physical coordinates
   * uses normal rounding
   *
   * \param latticeR output lattice index
   * \param physR input physical coordinates
   */
  void GetLatticeR(
      const Vector2D<T>& rPhysR
    , gsl::index latticeR[2]) const;

  /**
   * Compute the lattice index closest to the given physical coordinates
   * rounds down
   *
   * \param latticeR output lattice index
   * \param physR input physical coordinates
   */
  void GetFloorLatticeR(
      const std::vector<T>& rPhysR
    , std::vector<gsl::index>& rLatticeR) const;

  /**
   * Compute the lattice index closest to the given physical coordinates
   * rounds down
   *
   * \param latticeR output lattice index
   * \param physR input physical coordinates
   */
  void GetFloorLatticeR(
      const T physR[2]
    , gsl::index latticeR[2]) const;

  /** \return mRefinementLevel */
  int GetRefinementLevel() const;

  /** set the number of full cells of the cuboid for load balancer */
  void SetWeight(std::size_t weight);

  /**
   * Output cuboid details
   */
  void Print() const;

 private:
  /** global x-position of lower left corner */
  T mXPosition;
  /** global y-position of lower left corner */
  T mYPosition;
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
}  // namespace iblbm

#endif  // GEOMETRY_SRC_CUBOID2D_HPP_
