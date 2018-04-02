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
static const auto g_pi = 3.14159265358979323846;

TEST(TestIndicatorCuboid2D_Constructor_OriginCorner_Default)
{
  TestIndicatorCuboid2D tester;
  Vector2D<double> extent = {1, 2};
  IndicatorCuboid2D<double> cuboid(extent);

  Vector2D<double> exp_center = 0.5 * extent;

  CHECK(testutil::CheckCloseVector(tester.GetCenter(cuboid), exp_center,
      g_loose_tol));
  CHECK_CLOSE(extent[0], tester.GetXLength(cuboid), g_loose_tol);
  CHECK_CLOSE(extent[1], tester.GetYLength(cuboid), g_loose_tol);
  CHECK_CLOSE(0.0, tester.GetTheta(cuboid), g_loose_tol);
}

TEST(TestIndicatorCuboid2D_Constructor_OriginCorner_UserDefined)
{
  TestIndicatorCuboid2D tester;
  Vector2D<double> extent = {1, 2};
  Vector2D<double> origin = {3, 4};
  auto theta = 5.6;
  IndicatorCuboid2D<double> cuboid(extent, origin, theta);

  auto exp_center = origin + 0.5 * extent;

  CHECK(testutil::CheckCloseVector(tester.GetCenter(cuboid), exp_center,
      g_loose_tol));
  CHECK_CLOSE(extent[0], tester.GetXLength(cuboid), g_loose_tol);
  CHECK_CLOSE(extent[1], tester.GetYLength(cuboid), g_loose_tol);
  CHECK_CLOSE(theta, tester.GetTheta(cuboid), g_loose_tol);
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

TEST(TestIndicatorCuboid2D_Constructor_OriginCenter_UserDefined)
{
  TestIndicatorCuboid2D tester;
  auto x_length = 1.2;
  auto y_length = 3.4;
  Vector2D<double> center = {5, 6};
  auto theta = 7.8;
  IndicatorCuboid2D<double> cuboid(x_length, y_length, center, theta);

  CHECK(testutil::CheckCloseVector(tester.GetCenter(cuboid), center,
      g_loose_tol));
  CHECK_CLOSE(x_length, tester.GetXLength(cuboid), g_loose_tol);
  CHECK_CLOSE(y_length, tester.GetYLength(cuboid), g_loose_tol);
  CHECK_CLOSE(theta, tester.GetTheta(cuboid), g_loose_tol);
}

TEST(TestIndicatorCuboid2D_CheckBounds_ZeroOrigin)
{
  Vector2D<double> extent = {1, 2};
  IndicatorCuboid2D<double> cuboid(extent);

  // Lower left corner
  std::vector<double> point_1 = {0.1, 0.1};
  std::vector<double> point_2 = {-0.1, 0.1};
  std::vector<double> point_3 = {-0.1, -0.1};
  std::vector<double> point_4 = {0.1, -0.1};

  // Lower right corner
  std::vector<double> point_5 = {extent[0] + 0.1, 0.1};
  std::vector<double> point_6 = {extent[0] + -0.1, 0.1};
  std::vector<double> point_7 = {extent[0] + -0.1, -0.1};
  std::vector<double> point_8 = {extent[0] + 0.1, -0.1};

  // Upper left corner
  std::vector<double> point_9 = {0.1, extent[1] + 0.1};
  std::vector<double> point_10 = {-0.1, extent[1] + 0.1};
  std::vector<double> point_11 = {-0.1, extent[1] + -0.1};
  std::vector<double> point_12 = {0.1, extent[1] + -0.1};

  // Upper right corner
  std::vector<double> point_13 = {extent[0] + 0.1, extent[1] + 0.1};
  std::vector<double> point_14 = {extent[0] + -0.1, extent[1] + 0.1};
  std::vector<double> point_15 = {extent[0] + -0.1, extent[1] + -0.1};
  std::vector<double> point_16 = {extent[0] + 0.1, extent[1] + -0.1};

  auto is_inside = true;
  cuboid(gsl::make_span<bool>(&is_inside), point_1);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_2);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_3);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_4);
  CHECK(is_inside == false);

  cuboid(gsl::make_span<bool>(&is_inside), point_5);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_6);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_7);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_8);
  CHECK(is_inside == false);

  cuboid(gsl::make_span<bool>(&is_inside), point_9);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_10);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_11);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_12);
  CHECK(is_inside == true);

  cuboid(gsl::make_span<bool>(&is_inside), point_13);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_14);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_15);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_16);
  CHECK(is_inside == false);

  auto theta = g_pi / 6.0;
  Vector2D<double> origin;
  IndicatorCuboid2D<double> rotated_cuboid(extent, origin, theta);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_1);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_2);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_3);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_4);
  CHECK(is_inside == false);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_5);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_6);
  CHECK(is_inside == true);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_7);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_8);
  CHECK(is_inside == false);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_9);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_10);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_11);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_12);
  CHECK(is_inside == true);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_13);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_14);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_15);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_16);
  CHECK(is_inside == false);
}

TEST(TestIndicatorCuboid2D_CheckBounds_UserDefinedOrigin)
{
  Vector2D<double> extent = {1, 2};
  Vector2D<double> origin = {3, 4};
  IndicatorCuboid2D<double> cuboid(extent, origin);

  // Lower left corner
  std::vector<double> point_1 = {origin[0] + 0.1, origin[1] + 0.1};
  std::vector<double> point_2 = {origin[0] - 0.1, origin[1] + 0.1};
  std::vector<double> point_3 = {origin[0] - 0.1, origin[1] - 0.1};
  std::vector<double> point_4 = {origin[0] + 0.1, origin[1] - 0.1};

  // Lower right corner
  std::vector<double> point_5 = {extent[0] + origin[0] + 0.1,
      origin[1] + 0.1};
  std::vector<double> point_6 = {extent[0] + origin[0] - 0.1,
      origin[1] + 0.1};
  std::vector<double> point_7 = {extent[0] + origin[0] - 0.1,
      origin[1] - 0.1};
  std::vector<double> point_8 = {extent[0] + origin[0] + 0.1,
      origin[1] - 0.1};

  // Upper left corner
  std::vector<double> point_9 = {origin[0] + 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_10 = {origin[0] - 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_11 = {origin[0] - 0.1,
      extent[1] + origin[1] - 0.1};
  std::vector<double> point_12 = {origin[0] + 0.1,
      extent[1] + origin[1] - 0.1};

  // Upper right corner
  std::vector<double> point_13 = {extent[0] + origin[0] + 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_14 = {extent[0] + origin[0] - 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_15 = {extent[0] + origin[0] - 0.1,
      extent[1] + origin[1] - 0.1};
  std::vector<double> point_16 = {extent[0] + origin[0] + 0.1,
      extent[1] + origin[1] - 0.1};

  auto is_inside = true;
  cuboid(gsl::make_span<bool>(&is_inside), point_1);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_2);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_3);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_4);
  CHECK(is_inside == false);

  cuboid(gsl::make_span<bool>(&is_inside), point_5);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_6);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_7);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_8);
  CHECK(is_inside == false);

  cuboid(gsl::make_span<bool>(&is_inside), point_9);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_10);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_11);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_12);
  CHECK(is_inside == true);

  cuboid(gsl::make_span<bool>(&is_inside), point_13);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_14);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_15);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_16);
  CHECK(is_inside == false);

  auto theta = g_pi / 6.0;
  IndicatorCuboid2D<double> rotated_cuboid(extent, origin, theta);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_1);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_2);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_3);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_4);
  CHECK(is_inside == false);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_5);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_6);
  CHECK(is_inside == true);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_7);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_8);
  CHECK(is_inside == false);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_9);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_10);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_11);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_12);
  CHECK(is_inside == true);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_13);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_14);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_15);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_16);
  CHECK(is_inside == false);
}

TEST(TestIndicatorCuboid2D_CheckBounds_ZeroCenter)
{
  auto x_length = 1.0;
  auto y_length = 2.0;
  IndicatorCuboid2D<double> cuboid(x_length, y_length);

  Vector2D<double> extent = {1, 2};
  Vector2D<double> origin = {-0.5, -1.0};

  // Lower left corner
  std::vector<double> point_1 = {origin[0] + 0.1, origin[1] + 0.1};
  std::vector<double> point_2 = {origin[0] - 0.1, origin[1] + 0.1};
  std::vector<double> point_3 = {origin[0] - 0.1, origin[1] - 0.1};
  std::vector<double> point_4 = {origin[0] + 0.1, origin[1] - 0.1};

  // Lower right corner
  std::vector<double> point_5 = {extent[0] + origin[0] + 0.1,
      origin[1] + 0.1};
  std::vector<double> point_6 = {extent[0] + origin[0] - 0.1,
      origin[1] + 0.1};
  std::vector<double> point_7 = {extent[0] + origin[0] - 0.1,
      origin[1] - 0.1};
  std::vector<double> point_8 = {extent[0] + origin[0] + 0.1,
      origin[1] - 0.1};

  // Upper left corner
  std::vector<double> point_9 = {origin[0] + 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_10 = {origin[0] - 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_11 = {origin[0] - 0.1,
      extent[1] + origin[1] - 0.1};
  std::vector<double> point_12 = {origin[0] + 0.1,
      extent[1] + origin[1] - 0.1};

  // Upper right corner
  std::vector<double> point_13 = {extent[0] + origin[0] + 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_14 = {extent[0] + origin[0] - 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_15 = {extent[0] + origin[0] - 0.1,
      extent[1] + origin[1] - 0.1};
  std::vector<double> point_16 = {extent[0] + origin[0] + 0.1,
      extent[1] + origin[1] - 0.1};

  auto is_inside = true;
  cuboid(gsl::make_span<bool>(&is_inside), point_1);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_2);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_3);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_4);
  CHECK(is_inside == false);

  cuboid(gsl::make_span<bool>(&is_inside), point_5);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_6);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_7);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_8);
  CHECK(is_inside == false);

  cuboid(gsl::make_span<bool>(&is_inside), point_9);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_10);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_11);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_12);
  CHECK(is_inside == true);

  cuboid(gsl::make_span<bool>(&is_inside), point_13);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_14);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_15);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_16);
  CHECK(is_inside == false);

  auto theta = g_pi / 6.0;
  Vector2D<double> center;
  IndicatorCuboid2D<double> rotated_cuboid(x_length, y_length, center,
      theta);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_1);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_2);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_3);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_4);
  CHECK(is_inside == false);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_5);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_6);
  CHECK(is_inside == true);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_7);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_8);
  CHECK(is_inside == false);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_9);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_10);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_11);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_12);
  CHECK(is_inside == true);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_13);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_14);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_15);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_16);
  CHECK(is_inside == false);
}

TEST(TestIndicatorCuboid2D_CheckBounds_UserDefinedCenter)
{
  auto x_length = 1.0;
  auto y_length = 2.0;
  Vector2D<double> center = {3, 4};
  IndicatorCuboid2D<double> cuboid(x_length, y_length, center);

  Vector2D<double> extent = {1, 2};
  Vector2D<double> origin = {2.5, 3};

  // Lower left corner
  std::vector<double> point_1 = {origin[0] + 0.1, origin[1] + 0.1};
  std::vector<double> point_2 = {origin[0] - 0.1, origin[1] + 0.1};
  std::vector<double> point_3 = {origin[0] - 0.1, origin[1] - 0.1};
  std::vector<double> point_4 = {origin[0] + 0.1, origin[1] - 0.1};

  // Lower right corner
  std::vector<double> point_5 = {extent[0] + origin[0] + 0.1,
      origin[1] + 0.1};
  std::vector<double> point_6 = {extent[0] + origin[0] - 0.1,
      origin[1] + 0.1};
  std::vector<double> point_7 = {extent[0] + origin[0] - 0.1,
      origin[1] - 0.1};
  std::vector<double> point_8 = {extent[0] + origin[0] + 0.1,
      origin[1] - 0.1};

  // Upper left corner
  std::vector<double> point_9 = {origin[0] + 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_10 = {origin[0] - 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_11 = {origin[0] - 0.1,
      extent[1] + origin[1] - 0.1};
  std::vector<double> point_12 = {origin[0] + 0.1,
      extent[1] + origin[1] - 0.1};

  // Upper right corner
  std::vector<double> point_13 = {extent[0] + origin[0] + 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_14 = {extent[0] + origin[0] - 0.1,
      extent[1] + origin[1] + 0.1};
  std::vector<double> point_15 = {extent[0] + origin[0] - 0.1,
      extent[1] + origin[1] - 0.1};
  std::vector<double> point_16 = {extent[0] + origin[0] + 0.1,
      extent[1] + origin[1] - 0.1};

  auto is_inside = true;
  cuboid(gsl::make_span<bool>(&is_inside), point_1);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_2);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_3);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_4);
  CHECK(is_inside == false);

  cuboid(gsl::make_span<bool>(&is_inside), point_5);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_6);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_7);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_8);
  CHECK(is_inside == false);

  cuboid(gsl::make_span<bool>(&is_inside), point_9);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_10);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_11);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_12);
  CHECK(is_inside == true);

  cuboid(gsl::make_span<bool>(&is_inside), point_13);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_14);
  CHECK(is_inside == false);
  cuboid(gsl::make_span<bool>(&is_inside), point_15);
  CHECK(is_inside == true);
  cuboid(gsl::make_span<bool>(&is_inside), point_16);
  CHECK(is_inside == false);

  auto theta = g_pi / 6.0;
  IndicatorCuboid2D<double> rotated_cuboid(x_length, y_length, center,
      theta);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_1);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_2);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_3);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_4);
  CHECK(is_inside == false);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_5);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_6);
  CHECK(is_inside == true);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_7);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_8);
  CHECK(is_inside == false);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_9);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_10);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_11);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_12);
  CHECK(is_inside == true);

  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_13);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_14);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_15);
  CHECK(is_inside == false);
  rotated_cuboid(gsl::make_span<bool>(&is_inside), point_16);
  CHECK(is_inside == false);
}
}
}  // namespace iblbm
