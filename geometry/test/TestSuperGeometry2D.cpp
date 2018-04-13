#include "UnitTest++/UnitTest++.h"
#include "CuboidGeometry2D.hpp"
#include "HeuristicLoadBalancer.hpp"
#include "IndicatorFunctor2D.hpp"
#include "SuperGeometry2D.hpp"

namespace iblbm
{
SUITE(TestGeometry)
{
TEST(TestSuperGeometry2D_Constructor)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  auto nx {static_cast<std::size_t>(extent[0] / delta_R + 1.5)};
  auto ny {static_cast<std::size_t>(extent[1] / delta_R + 1.5)};

  IndicatorCuboid2D<double> indicator_cuboid(extent, origin);
  CuboidGeometry2D<double> cuboid_geometry(indicator_cuboid, delta_R, nc);
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D super_geometry(cuboid_geometry, load_balancer);
}
}
}  // namespace iblbm
