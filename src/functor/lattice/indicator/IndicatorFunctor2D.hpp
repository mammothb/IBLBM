#ifndef SRC_FUNCTOR_LATTICE_INDICATOR_INDICATORFUNCTOR2D_HPP_
#define SRC_FUNCTOR_LATTICE_INDICATOR_INDICATORFUNCTOR2D_HPP_

#include "AbstractIndicatorFunctor2D.hpp"

namespace iblbm
{
template<typename S>
class IndicatorCuboid2D : public AbstractIndicatorFunctor2D<S>
{
  friend class TestIndicatorCuboid2D;
 public:
  /**
   * Constructs a cuboid with x-axis dimension from rOrigin[0] to rExtend[0],
   * y-axis dimension from rOrigin[1] to rExtend[1],
   *
   * \param rExtend extend of the cuboid
   * \param rOrigin origin of the cuboid
   * \param theta rotation of the cuboid above its center (not origin)
   */
  IndicatorCuboid2D(
      const std::vector<S>& rExtend
    , const std::vector<S>& rOrigin
    , S theta = 0);

  /**
   * Constructs an cuboid with x axis dimension from -xLength/2 to xLength/2
   *
   * \param rExtend extend of the cuboid
   * \param rOrigin origin of the cuboid
   * \param theta rotation of the cuboid above its center (not origin)
   */
//  IndicatorCuboid2D(
//      S xLength
//    , S yLength
//    , Vector)

  /**
   * Return true if input is inside, else return false
   */
  bool operator()(
      gsl::span<bool> output
    , const std::vector<S>& rInput) override;

 private:
  /** Coordinate of the center of the cuboid */
  std::vector<S> mCenter;
  /** Maximum value along x-axis */
  S mXLength;
  /** Maximum value along y-axis */
  S mYLength;
  /** Angle of rotation above mCenter, radian */
  S mTheta;
};
}  // namespace iblbm

#endif  // SRC_FUNCTOR_LATTICE_INDICATOR_INDICATORFUNCTOR2D_HPP_
