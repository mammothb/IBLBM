#include "UnitTest++/UnitTest++.h"
#include "CuboidGeometry2D.hpp"
#include "HeuristicLoadBalancer.hpp"
#include "IndicatorFunctor2D.hpp"
#include "SuperGeometry2D.hpp"

namespace iblbm
{
class TestSuperGeometry2D
{
 public:
  bool GetNeedCommunication(SuperGeometry2D<double>& rGeometry)
  {
    return rGeometry.mNeedCommunication;
  }
};

SUITE(TestGeometry)
{
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestSuperGeometry2D_Constructor_Default)
{
  TestSuperGeometry2D tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  std::size_t overlap {2};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer};

  CHECK(tester.GetNeedCommunication(super_geometry));
  CHECK(super_geometry.rGetStatisticsStatus());
  CHECK(std::as_const(super_geometry).rGetStatisticsStatus());

  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    auto x_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0] -
        static_cast<double>(overlap) * delta_R};
    auto y_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1] -
        static_cast<double>(overlap) * delta_R};

    CHECK_EQUAL(global_idx, super_geometry.rGetExtendedBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(x_origin, super_geometry.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[0], g_zero_tol);
    CHECK_CLOSE(y_origin, super_geometry.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[1], g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx() + 2 * overlap,
        super_geometry.rGetExtendedBlockGeometry(local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy() + 2 * overlap,
        super_geometry.rGetExtendedBlockGeometry(local_idx).GetNy());

    CHECK_EQUAL(global_idx, super_geometry.rGetBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0],
        super_geometry.rGetBlockGeometry(local_idx).GetOrigin()[0],
        g_zero_tol);
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1],
        super_geometry.rGetBlockGeometry(local_idx).GetOrigin()[1],
        g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx(),
        super_geometry.rGetBlockGeometry(local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy(),
        super_geometry.rGetBlockGeometry(local_idx).GetNy());
  }
}

TEST(TestSuperGeometry2D_Constructor_UserDefined)
{
  TestSuperGeometry2D tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  std::size_t overlap {1};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer,
      overlap};

  CHECK(tester.GetNeedCommunication(super_geometry));
  CHECK(super_geometry.rGetStatisticsStatus());
  CHECK(std::as_const(super_geometry).rGetStatisticsStatus());


  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    auto x_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0] -
        static_cast<double>(overlap) * delta_R};
    auto y_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1] -
        static_cast<double>(overlap) * delta_R};

    CHECK_EQUAL(global_idx, super_geometry.rGetExtendedBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(x_origin, super_geometry.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[0], g_zero_tol);
    CHECK_CLOSE(y_origin, super_geometry.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[1], g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx() + 2 * overlap,
        super_geometry.rGetExtendedBlockGeometry(local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy() + 2 * overlap,
        super_geometry.rGetExtendedBlockGeometry(local_idx).GetNy());

    CHECK_EQUAL(global_idx, super_geometry.rGetBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0],
        super_geometry.rGetBlockGeometry(local_idx).GetOrigin()[0],
        g_zero_tol);
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1],
        super_geometry.rGetBlockGeometry(local_idx).GetOrigin()[1],
        g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx(),
        super_geometry.rGetBlockGeometry(local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy(),
        super_geometry.rGetBlockGeometry(local_idx).GetNy());
  }
}

TEST(TestSuperGeometry2D_Constructor_Copy_Default)
{
  TestSuperGeometry2D tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  std::size_t overlap {2};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer};
  SuperGeometry2D<double> super_geometry_2 {super_geometry};

  CHECK(tester.GetNeedCommunication(super_geometry_2));
  CHECK(super_geometry_2.rGetStatisticsStatus());
  CHECK(std::as_const(super_geometry_2).rGetStatisticsStatus());

  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    auto x_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0] -
        static_cast<double>(overlap) * delta_R};
    auto y_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1] -
        static_cast<double>(overlap) * delta_R};

    CHECK_EQUAL(global_idx, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(x_origin, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[0], g_zero_tol);
    CHECK_CLOSE(y_origin, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[1], g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx() + 2 * overlap,
        super_geometry_2.rGetExtendedBlockGeometry(local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy() + 2 * overlap,
        super_geometry_2.rGetExtendedBlockGeometry(local_idx).GetNy());

    CHECK_EQUAL(global_idx, super_geometry_2.rGetBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0],
        super_geometry_2.rGetBlockGeometry(local_idx).GetOrigin()[0],
        g_zero_tol);
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1],
        super_geometry_2.rGetBlockGeometry(local_idx).GetOrigin()[1],
        g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx(),
        super_geometry_2.rGetBlockGeometry(local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy(),
        super_geometry_2.rGetBlockGeometry(local_idx).GetNy());
  }
}

TEST(TestSuperGeometry2D_Constructor_Copy_UserDefined)
{
  TestSuperGeometry2D tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  std::size_t overlap {1};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer,
      overlap};
  SuperGeometry2D<double> super_geometry_2 {super_geometry};

  CHECK(tester.GetNeedCommunication(super_geometry_2));
  CHECK(super_geometry_2.rGetStatisticsStatus());
  CHECK(std::as_const(super_geometry_2).rGetStatisticsStatus());

  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    auto x_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0] -
        static_cast<double>(overlap) * delta_R};
    auto y_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1] -
        static_cast<double>(overlap) * delta_R};

    CHECK_EQUAL(global_idx, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(x_origin, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[0], g_zero_tol);
    CHECK_CLOSE(y_origin, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[1], g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx() + 2 * overlap,
        super_geometry_2.rGetExtendedBlockGeometry(local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy() + 2 * overlap,
        super_geometry_2.rGetExtendedBlockGeometry(local_idx).GetNy());

    CHECK_EQUAL(global_idx, super_geometry_2.rGetBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0],
        super_geometry_2.rGetBlockGeometry(local_idx).GetOrigin()[0],
        g_zero_tol);
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1],
        super_geometry_2.rGetBlockGeometry(local_idx).GetOrigin()[1],
        g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx(),
        super_geometry_2.rGetBlockGeometry(local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy(),
        super_geometry_2.rGetBlockGeometry(local_idx).GetNy());
  }
}

TEST(TestSuperGeometry2D_CopyAssignment)
{
  TestSuperGeometry2D tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  std::size_t overlap {2};
  std::size_t overlap_1 {1};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer,
      overlap_1};
  SuperGeometry2D<double> super_geometry_2 {cuboid_geometry, load_balancer};

  CHECK(tester.GetNeedCommunication(super_geometry_2));
  CHECK(super_geometry_2.rGetStatisticsStatus());
  CHECK(std::as_const(super_geometry_2).rGetStatisticsStatus());

  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    auto x_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0] -
        static_cast<double>(overlap) * delta_R};
    auto y_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1] -
        static_cast<double>(overlap) * delta_R};

    CHECK_EQUAL(global_idx, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(x_origin, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[0], g_zero_tol);
    CHECK_CLOSE(y_origin, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[1], g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx() + 2 * overlap,
        super_geometry_2.rGetExtendedBlockGeometry(local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy() + 2 * overlap,
        super_geometry_2.rGetExtendedBlockGeometry(local_idx).GetNy());

    CHECK_EQUAL(global_idx, super_geometry_2.rGetBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0],
        super_geometry_2.rGetBlockGeometry(local_idx).GetOrigin()[0],
        g_zero_tol);
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1],
        super_geometry_2.rGetBlockGeometry(local_idx).GetOrigin()[1],
        g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx(),
        super_geometry_2.rGetBlockGeometry(local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy(),
        super_geometry_2.rGetBlockGeometry(local_idx).GetNy());
  }

  super_geometry_2 = super_geometry;

  CHECK(tester.GetNeedCommunication(super_geometry_2));
  CHECK(super_geometry_2.rGetStatisticsStatus());
  CHECK(std::as_const(super_geometry_2).rGetStatisticsStatus());

  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    auto x_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0] -
        static_cast<double>(overlap_1) * delta_R};
    auto y_origin {cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1] -
        static_cast<double>(overlap_1) * delta_R};

    CHECK_EQUAL(global_idx, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(x_origin, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[0], g_zero_tol);
    CHECK_CLOSE(y_origin, super_geometry_2.rGetExtendedBlockGeometry(
        local_idx).GetOrigin()[1], g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx() + 2 *
        overlap_1, super_geometry_2.rGetExtendedBlockGeometry(
            local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy() + 2 *
        overlap_1, super_geometry_2.rGetExtendedBlockGeometry(
            local_idx).GetNy());

    CHECK_EQUAL(global_idx, super_geometry_2.rGetBlockGeometry(
        local_idx).rGetGlobalCuboidIndex());
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[0],
        super_geometry_2.rGetBlockGeometry(local_idx).GetOrigin()[0],
        g_zero_tol);
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(global_idx).GetOrigin()[1],
        super_geometry_2.rGetBlockGeometry(local_idx).GetOrigin()[1],
        g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNx(),
        super_geometry_2.rGetBlockGeometry(local_idx).GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(global_idx).GetNy(),
        super_geometry_2.rGetBlockGeometry(local_idx).GetNy());
  }
}
}
}  // namespace iblbm
