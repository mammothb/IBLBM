#include "IndicatorFunctor2D.hpp"

#include <cmath>

#include "CoreUtilities.hpp"

namespace iblbm
{
template<typename S>
IndicatorCuboid2D<S>::IndicatorCuboid2D(
    const Vector2D<S>& rExtend
  , const Vector2D<S>& rOrigin
  , S theta/*=0*/)
  : mXLength{rExtend[0]},
    mYLength{rExtend[1]},
    mTheta{theta}
{
  this->mMin = rOrigin;
  this->mMax = rOrigin + rExtend;
  mCenter = rOrigin + S{0.5} * rExtend;
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
    mTheta{-theta}
{
  this->mMin = {mCenter[0] - 0.5 * mXLength, mCenter[1] - 0.5 * mYLength};
  this->mMax = {mCenter[0] + 0.5 * mXLength, mCenter[1] + 0.5 * mYLength};
}

template<typename S>
bool IndicatorCuboid2D<S>::operator()(
    gsl::span<bool> output
  , const std::vector<S>& rInput)
{
  auto x = mCenter[0] + (rInput[0] - mCenter[0]) * std::cos(mTheta) -
      (rInput[1] - mCenter[1]) * std::sin(mTheta);
  auto y = mCenter[1] + (rInput[0] - mCenter[0]) * std::sin(mTheta) +
      (rInput[1] - mCenter[1]) * std::cos(mTheta);

  output[0] = (std::abs(mCenter[0] - x) < 0.5 * mXLength ||
      util::IsNearZero(std::abs(mCenter[0] - x) - 0.5 * mXLength)) &&
      (std::abs(mCenter[1] - y) < 0.5 * mYLength ||
       util::IsNearZero(std::abs(mCenter[1] - y) - 0.5 * mYLength));
  return true;
}

template class IndicatorCuboid2D<double>;
}  // namespace iblbm
