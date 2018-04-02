#include "IndicatorFunctor2D.hpp"

#include <cmath>

#include "CoreUtilities.hpp"
#include "IblbmDebug.hpp"

namespace iblbm
{
template<typename S>
IndicatorCuboid2D<S>::IndicatorCuboid2D(
    const Vector2D<S>& rExtent
  , const Vector2D<S>& rOrigin/*=Vector2D<S>()*/
  , S theta/*=0*/)
  : mCenter{rOrigin + S{0.5} * rExtent},
    mXLength{rExtent[0]},
    mYLength{rExtent[1]},
    mTheta{theta}
{
  this->mMin = rOrigin;
  this->mMax = rOrigin + rExtent;
}

template<typename S>
IndicatorCuboid2D<S>::IndicatorCuboid2D(
    S xLength
  , S yLength
  , const Vector2D<S>& rCenter/*=Vector2D<S>()*/
  , S theta/*=0*/)
  : mCenter{rCenter},
    mXLength{xLength},
    mYLength{yLength},
    mTheta{theta}
{
  this->mMin = {mCenter[0] - 0.5 * mXLength, mCenter[1] - 0.5 * mYLength};
  this->mMax = {mCenter[0] + 0.5 * mXLength, mCenter[1] + 0.5 * mYLength};
}

template<typename S>
bool IndicatorCuboid2D<S>::operator()(
    gsl::span<bool> output
  , const std::vector<S>& rInput)
{
  // Clockwise rotation of input to see if it still fits in the cuboid
  auto x = (rInput[0] - mCenter[0]) * std::cos(-mTheta) -
      (rInput[1] - mCenter[1]) * std::sin(-mTheta);
  auto y = (rInput[0] - mCenter[0]) * std::sin(-mTheta) +
      (rInput[1] - mCenter[1]) * std::cos(-mTheta);

  output[0] = (std::abs(x) < 0.5 * mXLength ||
      util::IsNearZero(std::abs(x) - 0.5 * mXLength)) &&
      (std::abs(y) < 0.5 * mYLength ||
       util::IsNearZero(std::abs(y) - 0.5 * mYLength));
  return true;
}

// Explicit instantiation
template class IndicatorCuboid2D<double>;
}  // namespace iblbm
