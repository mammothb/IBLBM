#include <iostream>

#include "gsl/gsl"
#include "UnitTest++/UnitTest++.h"
#include "IndicatorFunctor2D.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
class TestIndicatorCuboid2D
{
 public:
  std::vector<double> GetCenter(const IndicatorCuboid2D<double>& rCuboid)
  {
    return rCuboid.mCenter;
  }

  double GetXLength(const IndicatorCuboid2D<double>& rCuboid)
  {
    return rCuboid.mXLength;
  }

  double GetYLength(const IndicatorCuboid2D<double>& rCuboid)
  {
    return rCuboid.mYLength;
  }

  double GetTheta(const IndicatorCuboid2D<double>& rCuboid)
  {
    return rCuboid.mTheta;
  }
};

SUITE(TestFunctor)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;

TEST(TestIndicatorCuboid2D_Constructor_Default)
{
  TestIndicatorCuboid2D tester;
  std::vector<double> extend = {1, 2};
  std::vector<double> origin = {0, 0};
  IndicatorCuboid2D<double> cuboid(extend, origin);

  auto exp_center = origin;
  for (gsl::index d = 0; d < 2; ++d) exp_center[d] += 0.5 * extend[d];

  CHECK(testutil::CheckCloseVector(tester.GetCenter(cuboid), exp_center,
      g_loose_tol));
  CHECK_CLOSE(origin[0] + extend[0], tester.GetXLength(cuboid), g_loose_tol);
  CHECK_CLOSE(origin[1] + extend[1], tester.GetYLength(cuboid), g_loose_tol);
  CHECK_CLOSE(0.0, tester.GetTheta(cuboid), g_loose_tol);
}
}
}  // namespace iblbm
