#ifndef GEOMETRY_SRC_BLOCKGEOMETRYVIEW2D_HPP_
#define GEOMETRY_SRC_BLOCKGEOMETRYVIEW2D_HPP_

#include "gsl/gsl"

//#include "AbstractBlockGeometryInterface2D.hpp"
#include "BlockStructure2D.hpp"
#include "Vector.hpp"

namespace iblbm
{
template<typename T> class AbstractBlockGeometryInterface2D;
template<typename T> class BlockGeometryStatistics2D;

template<typename T>
class BlockGeometryView2D final
  : public AbstractBlockGeometryInterface2D<T>
  , public BlockStructure2D
{
  friend class TestBlockGeometryView2D;
 public:
  /**
   * Constructor
   *
   * \param rBlockGeometry reference to the structure where this view class
   *        is viewing
   * \param xIndex0 x coordinate of lower left corner of viewing area
   * \param yIndex0 y coordinate of lower left corner of viewing area
   * \param xIndex1 x coordinate of upper right corner of viewing area
   * \param yIndex1 y coordinate of upper right corner of viewing area
   */
  BlockGeometryView2D(
      AbstractBlockGeometryInterface2D<T>& rBlockGeometry
    , gsl::index xIndex0
    , gsl::index yIndex0
    , gsl::index xIndex1
    , gsl::index yIndex1);

  /**
   * Destructor. Removes statistics status from statistics list
   */
  ~BlockGeometryView2D();

  /**
   * Copy constructor
   */
  BlockGeometryView2D(const BlockGeometryView2D& rRhs);

  /**
   * Copy assignment
   */
  BlockGeometryView2D& operator=(const BlockGeometryView2D& rRhs);

  /** \return Read and write access to the statistic object */
  BlockGeometryStatistics2D<T>& rGetStatistics(
      bool isVerbose = true) override;

  /** \return Read-only access to the statistic object */
  const BlockGeometryStatistics2D<T>& rGetStatistics(
      bool isVerbose = true) const override;

  /**
   * \return the position of the block origin which is the node
   * (x override, y override) in physical units (meter)
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

  /** returns the (xIndex, yIndex) entry in the 2D scalar field */
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
    , gsl::index yIndex) const override;

  /**
   * Adds a pointer to the flag statistic status which is depended on the
   * data of an block geometry
   */
  void AddToStatisticsList(bool* pStatisticStatus) override;

  /**
   * Removes a pointer if it exists from the flag statistic status which is
   * depended on the data of an block geometry
   */
  void RemoveFromStatisticsList(bool* pStatisticStatus) override;

 private:
  /** Pointer to the structure where this view class is viewing */
  AbstractBlockGeometryInterface2D<T>* mpBlockGeometry;
  /**
   * Offset of the data field with respect to the data of the original
   * geometry
   */
  gsl::index mXIndex0;
  gsl::index mYIndex0;
  /** Dimension of the view cuboid */
  std::size_t mNx;
  std::size_t mNy;

};
}  // namespace iblbm

#endif  // GEOMETRY_SRC_BLOCKGEOMETRYVIEW2D_HPP_
