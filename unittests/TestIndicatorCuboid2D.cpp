#include <iostream>

#include "gsl/gsl"
#include "UnitTest++/UnitTest++.h"
#include "IndicatorFunctor2D.hpp"
#include "UnitTestCustomUtilities.hpp"
#include "Vector.hpp"

namespace iblbm
{
class TestIndicatorCuboid2D
{
 public:
  Vector2D<double> GetCenter(const IndicatorCuboid2D<double>& rCuboid)
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

TEST(TestIndicatorCuboid2D_Constructor_OriginCorner_Default)
{
  TestIndicatorCuboid2D tester;
  Vector2D<double> extend = {1, 2};
  Vector2D<double> origin = {0, 0};
  IndicatorCuboid2D<double> cuboid(extend, origin);

  auto exp_center = origin;
  for (gsl::index d = 0; d < 2; ++d) exp_center[d] += 0.5 * extend[d];

  CHECK(testutil::CheckCloseVector(tester.GetCenter(cuboid), exp_center,
      g_loose_tol));
  CHECK_CLOSE(origin[0] + extend[0], tester.GetXLength(cuboid), g_loose_tol);
  CHECK_CLOSE(origin[1] + extend[1], tester.GetYLength(cuboid), g_loose_tol);
  CHECK_CLOSE(0.0, tester.GetTheta(cuboid), g_loose_tol);
}

TEST(TestIndicatorCuboid2D_Constructor_OriginCenter_Default)
{
  TestIndicatorCuboid2D tester;
  auto x_length = 1.2;
  auto y_length = 3.4;
  IndicatorCuboid2D<double> cuboid(x_length, y_length);

  Vector2D<double> exp_center;

  CHECK(testutil::CheckCloseVector(tester.GetCenter(cuboid), exp_center,
      g_loose_tol));
  CHECK_CLOSE(x_length, tester.GetXLength(cuboid), g_loose_tol);
  CHECK_CLOSE(y_length, tester.GetYLength(cuboid), g_loose_tol);
  CHECK_CLOSE(0.0, tester.GetTheta(cuboid), g_loose_tol);
}
}
}  // namespace iblbm
