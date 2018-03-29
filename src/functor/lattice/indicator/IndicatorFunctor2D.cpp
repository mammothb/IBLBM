#include "IndicatorFunctor2D.hpp"

#include <cmath>

#include "CoreUtilities.hpp"

namespace iblbm
{
template<typename S>
IndicatorCuboid2D<S>::IndicatorCuboid2D(
    const std::vector<S>& rExtend
  , const std::vector<S>& rOrigin
  , S theta/*=0*/)
  : mXLength{rExtend[0]},
    mYLength{rExtend[1]},
    mTheta{theta}
{
  auto my_max = rOrigin;
  auto my_center = rOrigin;
  // 2 since it's 2D
  for (gsl::index d = 0; d < 2; ++d) {
    my_max[d] += rExtend[d];
    my_center[d] += S{0.5} * rExtend[d];
  }
  this->mMin = rOrigin;
  this->mMax = my_max;
  mCenter = my_center;
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
