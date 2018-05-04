#include <array>
#include <iostream>
#include <list>

#include "UnitTest++/UnitTest++.h"
#include "BlockGeometry2D.hpp"
#include "BlockGeometryView2D.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
class TestBlockGeometryView2D
{
 public:
  AbstractBlockGeometryInterface2D<double>* pGetBlockGeometry(
      BlockGeometryView2D<double>& rBlockGeometryView)
  {
    return rBlockGeometryView.mpBlockGeometry;
  }

  gsl::index GetXIndex0(BlockGeometryView2D<double>& rBlockGeometryView)
  {
    return rBlockGeometryView.mXIndex0;
  }

  gsl::index GetYIndex0(BlockGeometryView2D<double>& rBlockGeometryView)
  {
    return rBlockGeometryView.mYIndex0;
  }
};

SUITE(TestGeometry)
{
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestBlockGeometryView2D_Constructor)
{
  TestBlockGeometryView2D tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  gsl::index x_0 {1};
  gsl::index y_0 {2};
  gsl::index x_1 {3};
  gsl::index y_1 {4};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
  BlockGeometryView2D<double> block_geometry_view {block_geometry, x_0, y_0,
      x_1, y_1};

  auto nx_view {static_cast<std::size_t>(x_1 - x_0 + 1)};
  auto ny_view {static_cast<std::size_t>(y_1 - y_0 + 1)};
  Vector2D<double> exp_origin {x_pos + static_cast<double>(x_0) * delta_R,
      y_pos + static_cast<double>(y_0) * delta_R};

  CHECK(tester.pGetBlockGeometry(block_geometry_view) == &block_geometry);
  CHECK_EQUAL(x_0, tester.GetXIndex0(block_geometry_view));
  CHECK_EQUAL(y_0, tester.GetYIndex0(block_geometry_view));
  CHECK(testutil::CheckCloseVector(block_geometry_view.GetOrigin(),
      exp_origin, g_loose_tol));
  CHECK_EQUAL(nx_view, block_geometry_view.GetNx());
  CHECK_EQUAL(ny_view, block_geometry_view.GetNy());
  CHECK_CLOSE(delta_R, block_geometry_view.GetDeltaR(), g_zero_tol);
}

TEST(TestBlockGeometryView2D_Constructor_Copy)
{
  TestBlockGeometryView2D tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  gsl::index x_0 {1};
  gsl::index y_0 {2};
  gsl::index x_1 {3};
  gsl::index y_1 {4};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
  BlockGeometryView2D<double> block_geometry_view {block_geometry, x_0, y_0,
      x_1, y_1};
  BlockGeometryView2D<double> block_geometry_view_2 {block_geometry_view};

  auto nx_view {static_cast<std::size_t>(x_1 - x_0 + 1)};
  auto ny_view {static_cast<std::size_t>(y_1 - y_0 + 1)};
  Vector2D<double> exp_origin {x_pos + static_cast<double>(x_0) * delta_R,
      y_pos + static_cast<double>(y_0) * delta_R};

  CHECK(tester.pGetBlockGeometry(block_geometry_view_2) == &block_geometry);
  CHECK_EQUAL(x_0, tester.GetXIndex0(block_geometry_view_2));
  CHECK_EQUAL(y_0, tester.GetYIndex0(block_geometry_view_2));
  CHECK(testutil::CheckCloseVector(block_geometry_view_2.GetOrigin(),
      exp_origin, g_loose_tol));
  CHECK_EQUAL(nx_view, block_geometry_view_2.GetNx());
  CHECK_EQUAL(ny_view, block_geometry_view_2.GetNy());
  CHECK_CLOSE(delta_R, block_geometry_view_2.GetDeltaR(), g_zero_tol);
}

TEST(TestBlockGeometryView2D_CopyAssignment)
{
  TestBlockGeometryView2D tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  gsl::index x_0 {1};
  gsl::index y_0 {2};
  gsl::index x_1 {3};
  gsl::index y_1 {4};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
  BlockGeometryView2D<double> block_geometry_view {block_geometry, x_0, y_0,
      x_1, y_1};
  BlockGeometryView2D<double> block_geometry_view_2 {block_geometry, x_0 + 1,
      y_0 + 1, x_1, y_1};

  auto nx_view {static_cast<std::size_t>(x_1 - x_0 + 1)};
  auto ny_view {static_cast<std::size_t>(y_1 - y_0 + 1)};
  Vector2D<double> exp_origin {x_pos + static_cast<double>(x_0) * delta_R,
      y_pos + static_cast<double>(y_0) * delta_R};
  Vector2D<double> exp_origin_2 {x_pos + static_cast<double>(x_0 + 1) *
      delta_R, y_pos + static_cast<double>(y_0 + 1) * delta_R};

  CHECK(tester.pGetBlockGeometry(block_geometry_view_2) == &block_geometry);
  CHECK_EQUAL(x_0 + 1, tester.GetXIndex0(block_geometry_view_2));
  CHECK_EQUAL(y_0 + 1, tester.GetYIndex0(block_geometry_view_2));
  CHECK(testutil::CheckCloseVector(block_geometry_view_2.GetOrigin(),
      exp_origin_2, g_loose_tol));
  CHECK_EQUAL(nx_view - 1, block_geometry_view_2.GetNx());
  CHECK_EQUAL(ny_view - 1, block_geometry_view_2.GetNy());
  CHECK_CLOSE(delta_R, block_geometry_view_2.GetDeltaR(), g_zero_tol);

  block_geometry_view_2 = block_geometry_view;

  CHECK(tester.pGetBlockGeometry(block_geometry_view_2) == &block_geometry);
  CHECK_EQUAL(x_0, tester.GetXIndex0(block_geometry_view_2));
  CHECK_EQUAL(y_0, tester.GetYIndex0(block_geometry_view_2));
  CHECK(testutil::CheckCloseVector(block_geometry_view_2.GetOrigin(),
      exp_origin, g_loose_tol));
  CHECK_EQUAL(nx_view, block_geometry_view_2.GetNx());
  CHECK_EQUAL(ny_view, block_geometry_view_2.GetNy());
  CHECK_CLOSE(delta_R, block_geometry_view_2.GetDeltaR(), g_zero_tol);
}
}
}  // namespace iblbm
