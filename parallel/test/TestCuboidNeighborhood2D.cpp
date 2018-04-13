#include "UnitTest++/UnitTest++.h"
#include "CuboidGeometry2D.hpp"
#include "CuboidNeighborhood2D.hpp"
#include "HeuristicLoadBalancer.hpp"
#include "IndicatorFunctor2D.hpp"
#include "SuperGeometry2D.hpp"

namespace iblbm
{
class TestCuboidNeighborhood2D
{
 public:
  gsl::index GetGlobalCuboidIndex(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mGlobalCuboidIndex;
  }

  std::size_t GetNumCuboid(CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mNumCuboid;
  }

  double GetDeltaR(CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mDeltaR;
  }

  std::size_t GetNumData(CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mNumData;
  }

  std::size_t GetSizeofDataType(CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mSizeofDataType;
  }

  bool GetHasInitializedInNeighbor(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mHasInitializedInNeighbor;
  }

  bool GetHasInitializedOutNeighbor(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mHasInitializedOutNeighbor;
  }
};

SUITE(TestParallel)
{
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestCuboidNeighborhood2D_Constructor)
{
  TestCuboidNeighborhood2D tester;

  auto delta_R {0.5};
  Vector2D<double> origin {1.2, 3.4};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid(extent, origin);
  CuboidGeometry2D<double> cuboid_geometry(indicator_cuboid, delta_R, nc);
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D super_geometry(cuboid_geometry, load_balancer);
  CuboidNeighborhood2D neighborhood(super_geometry, 3);

  CHECK_EQUAL(3, tester.GetGlobalCuboidIndex(neighborhood));
  CHECK_EQUAL(nc, tester.GetNumCuboid(neighborhood));
  CHECK_CLOSE(delta_R, tester.GetDeltaR(neighborhood), g_zero_tol);
  CHECK_EQUAL(1, tester.GetNumData(neighborhood));
  CHECK_EQUAL(sizeof(std::size_t), tester.GetSizeofDataType(neighborhood));
  CHECK(tester.GetHasInitializedInNeighbor(neighborhood) == false);
  CHECK(tester.GetHasInitializedOutNeighbor(neighborhood) == false);
}
}
}  // namespace iblbm
