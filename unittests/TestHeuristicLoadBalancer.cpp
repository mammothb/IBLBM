#include "UnitTest++/UnitTest++.h"
#include "CuboidGeometry2D.hpp"
#include "HeuristicLoadBalancer.hpp"
#include "IndicatorFunctor2D.hpp"

namespace iblbm
{
class TestHeuristicLoadBalancer
{
 public:
  CuboidGeometry2D<double>* pGetCuboidGeometry2D(
      HeuristicLoadBalancer<double>& rLoadBalancer)
  {
    return rLoadBalancer.mpCuboidGeometry2D;
  }

  double GetRatioFullEmpty(HeuristicLoadBalancer<double>& rLoadBalancer)
  {
    return rLoadBalancer.mRatioFullEmpty;
  }
};

SUITE(TestParallel)
{
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestHeuristicLoadBalancer_Constructor_Default)
{
  TestHeuristicLoadBalancer tester;
  HeuristicLoadBalancer<double> load_balancer;
  CHECK(tester.pGetCuboidGeometry2D(load_balancer) == nullptr);
  CHECK_CLOSE(1.0, tester.GetRatioFullEmpty(load_balancer), g_zero_tol);
}

TEST(TestHeuristicLoadBalancer_Constructor_2D_Default)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};
  auto nc {9u};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};

  auto nx {static_cast<std::size_t>(extent[0] / delta_R + 1.5)};
  auto ny {static_cast<std::size_t>(extent[1] / delta_R + 1.5)};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny,
      nc};
  cuboid_geometry.Shrink(indicator_cuboid);
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};
}
}
}  // namespace iblbm
