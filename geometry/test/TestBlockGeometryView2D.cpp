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

  CHECK(block_geometry_view.rGetStatistics().rGetStatus());
  CHECK(std::as_const(block_geometry_view).rGetStatistics().rGetStatus());
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

TEST(TestBlockGeometryView2D_GetPhysR)
{
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

  gsl::index x_idx {1};
  gsl::index y_idx {2};
  double exp_phys_R[2] {x_pos + static_cast<double>(x_0 + x_idx) * delta_R,
      y_pos + static_cast<double>(y_0 + y_idx) * delta_R};

  double phys_R[2];
  block_geometry_view.GetPhysR(x_idx, y_idx, phys_R);
  CHECK_ARRAY_CLOSE(exp_phys_R, phys_R, 2, g_zero_tol);
}

TEST(TestBlockGeometryView2D_GetMaterial)
{
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

  gsl::index x_idx {1};
  gsl::index y_idx {2};
  double exp_phys_R[2] {x_pos + static_cast<double>(x_0 + x_idx) * delta_R,
      y_pos + static_cast<double>(y_0 + y_idx) * delta_R};

  // make sure material number are defaulted to zero
  for (gsl::index x {0}; x < block_geometry_view.GetNx(); ++x) {
    for (gsl::index y {0}; y < block_geometry_view.GetNy(); ++y) {
      CHECK_EQUAL(0, block_geometry_view.GetMaterial(x, y));
    }
  }
  for (gsl::index x {0}; x < block_geometry_view.GetNx(); ++x) {
    for (gsl::index y {0}; y < block_geometry_view.GetNy(); ++y) {
      block_geometry_view.rGetMaterial(x, y) = x + y;
    }
  }
  for (gsl::index x {0}; x < block_geometry_view.GetNx(); ++x) {
    for (gsl::index y {0}; y < block_geometry_view.GetNy(); ++y) {
      CHECK_EQUAL(x + y, block_geometry_view.GetMaterial(x, y));
      CHECK_EQUAL(x + y, std::as_const(block_geometry_view).rGetMaterial(x,
          y));
    }
  }
}
}
}  // namespace iblbm
