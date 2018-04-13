#ifndef FUNCTOR_SRC_LATTICE_INDICATOR_INDICATORFUNCTOR2D_HPP_
#define FUNCTOR_SRC_LATTICE_INDICATOR_INDICATORFUNCTOR2D_HPP_

#include <vector>

#include "AbstractIndicatorFunctor2D.hpp"
#include "Vector.hpp"

namespace iblbm
{
template<typename S>
class IndicatorCuboid2D : public AbstractIndicatorFunctor2D<S>
{
  friend class TestIndicatorCuboid2D;
 public:
  /**
   * Constructs a cuboid with x-axis dimension from rOrigin[0] to
   * rOrigin[0] + rExtent[0], y-axis dimension from rOrigin[1] to
   * rOrigin[1] + rExtent[1].
   *
   * \param rExtent extent of the cuboid
   * \param rOrigin origin of the cuboid, defaults to (0, 0)
   * \param theta rotation of the cuboid above its center (not origin)
   */
  IndicatorCuboid2D(
      const Vector2D<S>& rExtent
    , const Vector2D<S>& rOrigin = Vector2D<S>()
    , S theta = 0);

  /**
   * Constructs an cuboid with x axis dimension from -xLength/2 to xLength/2
   *
   * \param xLength length of cuboid along x-axis
   * \param yLength length of cuboid along y-axis
   * \param rCenter center of cuboid, defaults to (0, 0)
   * \param theta rotation of the cuboid above its center (not origin)
   */
  IndicatorCuboid2D(
      S xLength
    , S yLength
    , const Vector2D<S>& rCenter = Vector2D<S>()
    , S theta = 0);

  /**
   * Return true if input is inside, else return false
   */
  void operator()(
      gsl::span<bool> output
    , const std::vector<S>& rInput) override;

 private:
  /** Coordinate of the center of the cuboid */
  Vector2D<S> mCenter;
  /** Maximum value along x-axis */
  S mXLength;
  /** Maximum value along y-axis */
  S mYLength;
  /** Angle of rotation above mCenter, radian */
  S mTheta;
};
}  // namespace iblbm

#endif  // FUNCTOR_SRC_LATTICE_INDICATOR_INDICATORFUNCTOR2D_HPP_
