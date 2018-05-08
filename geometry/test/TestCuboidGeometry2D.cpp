#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

#include "UnitTest++/UnitTest++.h"
#include "CuboidGeometry2D.hpp"
#include "IndicatorFunctor2D.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
class TestCuboidGeometry2D
{
 public:
  std::vector<Cuboid2D<double>> GetCuboids(
      const CuboidGeometry2D<double>& rCuboidGeometry)
  {
    return rCuboidGeometry.mCuboids;
  }

  std::vector<bool> GetIsPeriodic(
      const CuboidGeometry2D<double>& rCuboidGeometry)
  {
    return rCuboidGeometry.mIsPeriodic;
  }
};

SUITE(TestGeometry)
{
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestCuboidGeometry2D_Constructor_Default)
{
  TestCuboidGeometry2D tester;
  CuboidGeometry2D<double> cuboid_geometry;

  auto mother_cuboid {cuboid_geometry.GetMotherCuboid()};

  auto x_pos {0.0};
  auto y_pos {0.0};
  auto delta_R {0.0};
  auto nx {0u};
  auto ny {0u};
  auto exp_phys_perimeter {2.0 * delta_R * nx + 2.0 * delta_R * ny};
//  auto exp_lattice_perimenter {2 * nx + 2 * ny - 4};
  auto exp_phys_volume {delta_R * nx * delta_R * ny};
  auto exp_lattice_volume {nx * ny};

  CHECK_CLOSE(x_pos, mother_cuboid.GetGlobalXPosition(), g_zero_tol);
  CHECK_CLOSE(y_pos, mother_cuboid.GetGlobalYPosition(), g_zero_tol);
  CHECK_CLOSE(x_pos, mother_cuboid.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, mother_cuboid.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, mother_cuboid.GetNx());
  CHECK_EQUAL(ny, mother_cuboid.GetNy());
  CHECK_EQUAL(nx, mother_cuboid.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, mother_cuboid.GetLatticeExtent()[1]);
  CHECK_EQUAL(std::numeric_limits<std::size_t>::max(),
      mother_cuboid.GetWeight());
  CHECK_CLOSE(exp_phys_perimeter, mother_cuboid.GetPhysPerimeter(),
      g_loose_tol);
  // Cannot check this since the -4 to account for removing overlapping nodes
  // causes underflow
//  CHECK_EQUAL(exp_lattice_perimenter, mother_cuboid.GetLatticePerimeter());
  CHECK_CLOSE(exp_phys_volume, mother_cuboid.GetPhysVolume(), g_loose_tol);
  CHECK_EQUAL(exp_lattice_volume, mother_cuboid.GetLatticeVolume());

  auto cuboid_vec {tester.GetCuboids(cuboid_geometry)};

  CHECK_EQUAL(1u, cuboid_vec.size());
  for (gsl::index i = 0; i < cuboid_vec.size(); ++i) {
    CHECK_CLOSE(x_pos, cuboid_vec[i].GetGlobalXPosition(), g_zero_tol);
    CHECK_CLOSE(y_pos, cuboid_vec[i].GetGlobalYPosition(), g_zero_tol);
    CHECK_CLOSE(x_pos, cuboid_vec[i].GetOrigin()[0], g_zero_tol);
    CHECK_CLOSE(y_pos, cuboid_vec[i].GetOrigin()[1], g_zero_tol);
    CHECK_EQUAL(nx, cuboid_vec[i].GetNx());
    CHECK_EQUAL(ny, cuboid_vec[i].GetNy());
    CHECK_EQUAL(nx, cuboid_vec[i].GetLatticeExtent()[0]);
    CHECK_EQUAL(ny, cuboid_vec[i].GetLatticeExtent()[1]);
    CHECK_EQUAL(std::numeric_limits<std::size_t>::max(),
        cuboid_vec[i].GetWeight());
    CHECK_CLOSE(exp_phys_perimeter, cuboid_vec[i].GetPhysPerimeter(),
        g_loose_tol);
    // Cannot check this since the -4 to account for removing overlapping nodes
    // causes underflow
  //  CHECK_EQUAL(exp_lattice_perimenter, cuboid_vec[i].GetLatticePerimeter());
    CHECK_CLOSE(exp_phys_volume, cuboid_vec[i].GetPhysVolume(), g_loose_tol);
    CHECK_EQUAL(exp_lattice_volume, cuboid_vec[i].GetLatticeVolume());
  }
}

TEST(TestCuboidGeometry2D_Constructor_Coordinates_Default)
{
  TestCuboidGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {5.6};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  auto mother_cuboid = cuboid_geometry.GetMotherCuboid();

  auto exp_phys_perimeter {2.0 * delta_R * nx + 2.0 * delta_R * ny};
  auto exp_lattice_perimenter {2 * nx + 2 * ny - 4};
  auto exp_phys_volume {delta_R * nx * delta_R * ny};
  auto exp_lattice_volume {nx * ny};

  CHECK_CLOSE(x_pos, mother_cuboid.GetGlobalXPosition(), g_zero_tol);
  CHECK_CLOSE(y_pos, mother_cuboid.GetGlobalYPosition(), g_zero_tol);
  CHECK_CLOSE(x_pos, mother_cuboid.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, mother_cuboid.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, mother_cuboid.GetNx());
  CHECK_EQUAL(ny, mother_cuboid.GetNy());
  CHECK_EQUAL(nx, mother_cuboid.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, mother_cuboid.GetLatticeExtent()[1]);
  CHECK_EQUAL(std::numeric_limits<std::size_t>::max(),
      mother_cuboid.GetWeight());
  CHECK_CLOSE(exp_phys_perimeter, mother_cuboid.GetPhysPerimeter(),
      g_loose_tol);
  CHECK_EQUAL(exp_lattice_perimenter, mother_cuboid.GetLatticePerimeter());
  CHECK_CLOSE(exp_phys_volume, mother_cuboid.GetPhysVolume(), g_loose_tol);
  CHECK_EQUAL(exp_lattice_volume, mother_cuboid.GetLatticeVolume());

  auto cuboid_vec {tester.GetCuboids(cuboid_geometry)};

  CHECK_EQUAL(1u, cuboid_vec.size());
  for (gsl::index i = 0; i < cuboid_vec.size(); ++i) {
    CHECK_CLOSE(x_pos, cuboid_vec[i].GetGlobalXPosition(), g_zero_tol);
    CHECK_CLOSE(y_pos, cuboid_vec[i].GetGlobalYPosition(), g_zero_tol);
    CHECK_CLOSE(x_pos, cuboid_vec[i].GetOrigin()[0], g_zero_tol);
    CHECK_CLOSE(y_pos, cuboid_vec[i].GetOrigin()[1], g_zero_tol);
    CHECK_EQUAL(nx, cuboid_vec[i].GetNx());
    CHECK_EQUAL(ny, cuboid_vec[i].GetNy());
    CHECK_EQUAL(nx, cuboid_vec[i].GetLatticeExtent()[0]);
    CHECK_EQUAL(ny, cuboid_vec[i].GetLatticeExtent()[1]);
    CHECK_EQUAL(std::numeric_limits<std::size_t>::max(),
        cuboid_vec[i].GetWeight());
    CHECK_CLOSE(exp_phys_perimeter, cuboid_vec[i].GetPhysPerimeter(),
        g_loose_tol);
    CHECK_EQUAL(exp_lattice_perimenter, cuboid_vec[i].GetLatticePerimeter());
    CHECK_CLOSE(exp_phys_volume, cuboid_vec[i].GetPhysVolume(), g_loose_tol);
    CHECK_EQUAL(exp_lattice_volume, cuboid_vec[i].GetLatticeVolume());
  }
}

TEST(TestCuboidGeometry2D_Constructor_Coordinates_UserDefined)
{
  TestCuboidGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {5.6};
  auto nx {7u};
  auto ny {8u};
  auto nc {9u};

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny,
      nc);
  auto mother_cuboid = cuboid_geometry.GetMotherCuboid();

  auto exp_phys_perimeter {2.0 * delta_R * nx + 2.0 * delta_R * ny};
  auto exp_lattice_perimenter {2 * nx + 2 * ny - 4};
  auto exp_phys_volume {delta_R * nx * delta_R * ny};
  auto exp_lattice_volume {nx * ny};

  CHECK_CLOSE(x_pos, mother_cuboid.GetGlobalXPosition(), g_zero_tol);
  CHECK_CLOSE(y_pos, mother_cuboid.GetGlobalYPosition(), g_zero_tol);
  CHECK_CLOSE(x_pos, mother_cuboid.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, mother_cuboid.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, mother_cuboid.GetNx());
  CHECK_EQUAL(ny, mother_cuboid.GetNy());
  CHECK_EQUAL(nx, mother_cuboid.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, mother_cuboid.GetLatticeExtent()[1]);
  CHECK_EQUAL(std::numeric_limits<std::size_t>::max(),
      mother_cuboid.GetWeight());
  CHECK_CLOSE(exp_phys_perimeter, mother_cuboid.GetPhysPerimeter(),
      g_loose_tol);
  CHECK_EQUAL(exp_lattice_perimenter, mother_cuboid.GetLatticePerimeter());
  CHECK_CLOSE(exp_phys_volume, mother_cuboid.GetPhysVolume(), g_loose_tol);
  CHECK_EQUAL(exp_lattice_volume, mother_cuboid.GetLatticeVolume());

  auto cuboid_vec {tester.GetCuboids(cuboid_geometry)};
  // Split expected cuboid with mother_cuboid
  std::vector<Cuboid2D<double>> exp_cuboid_vec;
  mother_cuboid.Divide(nc, exp_cuboid_vec);

  CHECK_EQUAL(nc, cuboid_vec.size());
  for (gsl::index i = 0; i < cuboid_vec.size(); ++i) {
    CHECK_CLOSE(exp_cuboid_vec[i].GetGlobalXPosition(),
        cuboid_vec[i].GetGlobalXPosition(), g_zero_tol);
    CHECK_CLOSE(exp_cuboid_vec[i].GetGlobalYPosition(),
        cuboid_vec[i].GetGlobalYPosition(), g_zero_tol);
    CHECK(testutil::CheckCloseVector(cuboid_vec[i].GetOrigin(),
        exp_cuboid_vec[i].GetOrigin(), g_loose_tol));
    CHECK_EQUAL(cuboid_vec[i].GetNx(), cuboid_vec[i].GetNx());
    CHECK_EQUAL(cuboid_vec[i].GetNy(), cuboid_vec[i].GetNy());
    CHECK(testutil::CheckEqualVector(cuboid_vec[i].GetLatticeExtent(),
        exp_cuboid_vec[i].GetLatticeExtent()));
    CHECK_EQUAL(exp_cuboid_vec[i].GetWeight(), cuboid_vec[i].GetWeight());
    CHECK_CLOSE(exp_cuboid_vec[i].GetPhysPerimeter(),
        cuboid_vec[i].GetPhysPerimeter(), g_loose_tol);
    CHECK_EQUAL(exp_cuboid_vec[i].GetLatticePerimeter(),
        cuboid_vec[i].GetLatticePerimeter());
    CHECK_CLOSE(exp_cuboid_vec[i].GetPhysVolume(),
        cuboid_vec[i].GetPhysVolume(), g_loose_tol);
    CHECK_EQUAL(exp_cuboid_vec[i].GetLatticeVolume(),
        cuboid_vec[i].GetLatticeVolume());
  }
}

TEST(TestCuboidGeometry2D_Constructor_Indicator_Default)
{
  TestCuboidGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};

  auto nx {static_cast<std::size_t>(extent[0] / delta_R + 1.5)};
  auto ny {static_cast<std::size_t>(extent[1] / delta_R + 1.5)};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry(indicator_cuboid, delta_R);
  auto mother_cuboid {cuboid_geometry.GetMotherCuboid()};

  auto exp_phys_perimeter {2.0 * delta_R * nx + 2.0 * delta_R * ny};
  auto exp_lattice_perimenter {2 * nx + 2 * ny - 4};
  auto exp_phys_volume {delta_R * nx * delta_R * ny};
  auto exp_lattice_volume {nx * ny};

  CHECK_CLOSE(x_pos, mother_cuboid.GetGlobalXPosition(), g_zero_tol);
  CHECK_CLOSE(y_pos, mother_cuboid.GetGlobalYPosition(), g_zero_tol);
  CHECK_CLOSE(x_pos, mother_cuboid.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, mother_cuboid.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, mother_cuboid.GetNx());
  CHECK_EQUAL(ny, mother_cuboid.GetNy());
  CHECK_EQUAL(nx, mother_cuboid.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, mother_cuboid.GetLatticeExtent()[1]);
  CHECK_EQUAL(std::numeric_limits<std::size_t>::max(),
      mother_cuboid.GetWeight());
  CHECK_CLOSE(exp_phys_perimeter, mother_cuboid.GetPhysPerimeter(),
      g_loose_tol);
  CHECK_EQUAL(exp_lattice_perimenter, mother_cuboid.GetLatticePerimeter());
  CHECK_CLOSE(exp_phys_volume, mother_cuboid.GetPhysVolume(), g_loose_tol);
  CHECK_EQUAL(exp_lattice_volume, mother_cuboid.GetLatticeVolume());

  auto cuboid_vec {tester.GetCuboids(cuboid_geometry)};
  CHECK_EQUAL(1u, cuboid_vec.size());
  for (gsl::index i = 0; i < cuboid_vec.size(); ++i) {
    CHECK_CLOSE(x_pos, cuboid_vec[i].GetGlobalXPosition(), g_zero_tol);
    CHECK_CLOSE(y_pos, cuboid_vec[i].GetGlobalYPosition(), g_zero_tol);
    CHECK_CLOSE(x_pos, cuboid_vec[i].GetOrigin()[0], g_zero_tol);
    CHECK_CLOSE(y_pos, cuboid_vec[i].GetOrigin()[1], g_zero_tol);
    CHECK_EQUAL(nx, cuboid_vec[i].GetNx());
    CHECK_EQUAL(ny, cuboid_vec[i].GetNy());
    CHECK_EQUAL(nx, cuboid_vec[i].GetLatticeExtent()[0]);
    CHECK_EQUAL(ny, cuboid_vec[i].GetLatticeExtent()[1]);
    CHECK_EQUAL(nx * ny, cuboid_vec[i].GetWeight());
    CHECK_CLOSE(exp_phys_perimeter, cuboid_vec[i].GetPhysPerimeter(),
        g_loose_tol);
    CHECK_EQUAL(exp_lattice_perimenter, cuboid_vec[i].GetLatticePerimeter());
    CHECK_CLOSE(exp_phys_volume, cuboid_vec[i].GetPhysVolume(), g_loose_tol);
    CHECK_EQUAL(exp_lattice_volume, cuboid_vec[i].GetLatticeVolume());
  }
}

TEST(TestCuboidGeometry2D_Constructor_Indicator_UserDefined)
{
  TestCuboidGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  auto nx {static_cast<std::size_t>(extent[0] / delta_R + 1.5)};
  auto ny {static_cast<std::size_t>(extent[1] / delta_R + 1.5)};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  auto mother_cuboid {cuboid_geometry.GetMotherCuboid()};

  auto exp_phys_perimeter {2.0 * delta_R * nx + 2.0 * delta_R * ny};
  auto exp_lattice_perimenter {2 * nx + 2 * ny - 4};
  auto exp_phys_volume {delta_R * nx * delta_R * ny};
  auto exp_lattice_volume {nx * ny};

  CHECK_CLOSE(x_pos, mother_cuboid.GetGlobalXPosition(), g_zero_tol);
  CHECK_CLOSE(y_pos, mother_cuboid.GetGlobalYPosition(), g_zero_tol);
  CHECK_CLOSE(x_pos, mother_cuboid.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, mother_cuboid.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, mother_cuboid.GetNx());
  CHECK_EQUAL(ny, mother_cuboid.GetNy());
  CHECK_EQUAL(nx, mother_cuboid.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, mother_cuboid.GetLatticeExtent()[1]);
  CHECK_EQUAL(std::numeric_limits<std::size_t>::max(),
      mother_cuboid.GetWeight());
  CHECK_CLOSE(exp_phys_perimeter, mother_cuboid.GetPhysPerimeter(),
      g_loose_tol);
  CHECK_EQUAL(exp_lattice_perimenter, mother_cuboid.GetLatticePerimeter());
  CHECK_CLOSE(exp_phys_volume, mother_cuboid.GetPhysVolume(), g_loose_tol);
  CHECK_EQUAL(exp_lattice_volume, mother_cuboid.GetLatticeVolume());

  auto cuboid_vec {tester.GetCuboids(cuboid_geometry)};
  // Split expected cuboid with mother_cuboid
  std::vector<Cuboid2D<double>> exp_cuboid_vec;
  mother_cuboid.Divide(nc, exp_cuboid_vec);

  CHECK_EQUAL(nc, cuboid_vec.size());
  for (gsl::index i = 0; i < cuboid_vec.size(); ++i) {
    CHECK_CLOSE(exp_cuboid_vec[i].GetGlobalXPosition(),
        cuboid_vec[i].GetGlobalXPosition(), g_zero_tol);
    CHECK_CLOSE(exp_cuboid_vec[i].GetGlobalYPosition(),
        cuboid_vec[i].GetGlobalYPosition(), g_zero_tol);
    CHECK(testutil::CheckCloseVector(cuboid_vec[i].GetOrigin(),
        exp_cuboid_vec[i].GetOrigin(), g_loose_tol));
    CHECK_EQUAL(cuboid_vec[i].GetNx(), cuboid_vec[i].GetNx());
    CHECK_EQUAL(cuboid_vec[i].GetNy(), cuboid_vec[i].GetNy());
    CHECK(testutil::CheckEqualVector(cuboid_vec[i].GetLatticeExtent(),
        exp_cuboid_vec[i].GetLatticeExtent()));
    CHECK_EQUAL(cuboid_vec[i].GetNx() * cuboid_vec[i].GetNy(),
        cuboid_vec[i].GetWeight());
    CHECK_CLOSE(exp_cuboid_vec[i].GetPhysPerimeter(),
        cuboid_vec[i].GetPhysPerimeter(), g_loose_tol);
    CHECK_EQUAL(exp_cuboid_vec[i].GetLatticePerimeter(),
        cuboid_vec[i].GetLatticePerimeter());
    CHECK_CLOSE(exp_cuboid_vec[i].GetPhysVolume(),
        cuboid_vec[i].GetPhysVolume(), g_loose_tol);
    CHECK_EQUAL(exp_cuboid_vec[i].GetLatticeVolume(),
        cuboid_vec[i].GetLatticeVolume());
  }
}

TEST(TestCuboidGeometry2D_Add)
{
  TestCuboidGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  CHECK_EQUAL(nc, cuboid_geometry.GetNumCuboid());

  auto cuboid_x_pos {9.0};
  auto cuboid_y_pos {11.12};
  auto cuboid_delta_R {0.1};
  auto cuboid_nx {20u};
  auto cuboid_ny {60u};
  Cuboid2D<double> cuboid {cuboid_x_pos, cuboid_y_pos, cuboid_delta_R,
      cuboid_nx, cuboid_ny};

  cuboid_geometry.Add(cuboid);

  CHECK_EQUAL(nc + 1, cuboid_geometry.GetNumCuboid());
  CHECK_CLOSE(cuboid_x_pos,
      tester.GetCuboids(cuboid_geometry).back().GetGlobalXPosition(),
      g_zero_tol);
  CHECK_CLOSE(cuboid_y_pos,
      tester.GetCuboids(cuboid_geometry).back().GetGlobalYPosition(),
      g_zero_tol);
  CHECK_CLOSE(cuboid_delta_R,
      tester.GetCuboids(cuboid_geometry).back().GetDeltaR(), g_zero_tol);
  CHECK_EQUAL(cuboid_nx, tester.GetCuboids(cuboid_geometry).back().GetNx());
  CHECK_EQUAL(cuboid_ny, tester.GetCuboids(cuboid_geometry).back().GetNy());
}

TEST(TestCuboidGeometry2D_Remove)
{
  TestCuboidGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  CHECK_EQUAL(nc, cuboid_geometry.GetNumCuboid());

  // We remove the cuboids at index 3 and 6, and check cuboids at (old) index
  // 2, 4, 7
  auto cuboid_2 {tester.GetCuboids(cuboid_geometry)[2]};
  auto cuboid_4 {tester.GetCuboids(cuboid_geometry)[4]};
  auto cuboid_7 {tester.GetCuboids(cuboid_geometry)[7]};

  // Make sure cuboid 4 and 3, and 7 and 5 are actually different
  CHECK(std::abs(cuboid_4.GetGlobalXPosition() -
      tester.GetCuboids(cuboid_geometry)[3].GetGlobalXPosition()) >
      g_zero_tol);
  CHECK(std::abs(cuboid_4.GetGlobalYPosition() -
      tester.GetCuboids(cuboid_geometry)[3].GetGlobalYPosition()) >
      g_zero_tol);
  CHECK(std::abs(cuboid_7.GetGlobalYPosition() -
      tester.GetCuboids(cuboid_geometry)[5].GetGlobalYPosition()) >
      g_zero_tol);

  cuboid_geometry.Remove(3);
  // old index 6 got shifted up by 1
  cuboid_geometry.Remove(5);

  CHECK_EQUAL(nc - 2, cuboid_geometry.GetNumCuboid());

  // Check that cuboid_2 remains unchanged since we removed cuboids behind it
  CHECK_CLOSE(cuboid_2.GetGlobalXPosition(),
      tester.GetCuboids(cuboid_geometry)[2].GetGlobalXPosition(),
      g_zero_tol);
  CHECK_CLOSE(cuboid_2.GetGlobalYPosition(),
      tester.GetCuboids(cuboid_geometry)[2].GetGlobalYPosition(),
      g_zero_tol);
  CHECK_CLOSE(cuboid_2.GetDeltaR(),
      tester.GetCuboids(cuboid_geometry)[2].GetDeltaR(), g_zero_tol);
  CHECK_EQUAL(cuboid_2.GetNx(),
      tester.GetCuboids(cuboid_geometry)[2].GetNx());
  CHECK_EQUAL(cuboid_2.GetNy(),
      tester.GetCuboids(cuboid_geometry)[2].GetNy());

  // Check that cuboid at old index 4 got shifted up by 1 since we removed
  // cuboid at index 3
  CHECK_CLOSE(cuboid_4.GetGlobalXPosition(),
      tester.GetCuboids(cuboid_geometry)[3].GetGlobalXPosition(),
      g_zero_tol);
  CHECK_CLOSE(cuboid_4.GetGlobalYPosition(),
      tester.GetCuboids(cuboid_geometry)[3].GetGlobalYPosition(),
      g_zero_tol);
  CHECK_CLOSE(cuboid_4.GetDeltaR(),
      tester.GetCuboids(cuboid_geometry)[3].GetDeltaR(), g_zero_tol);
  CHECK_EQUAL(cuboid_4.GetNx(),
      tester.GetCuboids(cuboid_geometry)[3].GetNx());
  CHECK_EQUAL(cuboid_4.GetNy(),
      tester.GetCuboids(cuboid_geometry)[3].GetNy());

  // Check that cuboid at old index 7 got shifted up by 2 since we removed
  // cuboid at index 3 and old index 6
  CHECK_CLOSE(cuboid_7.GetGlobalXPosition(),
      tester.GetCuboids(cuboid_geometry)[5].GetGlobalXPosition(),
      g_zero_tol);
  CHECK_CLOSE(cuboid_7.GetGlobalYPosition(),
      tester.GetCuboids(cuboid_geometry)[5].GetGlobalYPosition(),
      g_zero_tol);
  CHECK_CLOSE(cuboid_7.GetDeltaR(),
      tester.GetCuboids(cuboid_geometry)[5].GetDeltaR(), g_zero_tol);
  CHECK_EQUAL(cuboid_7.GetNx(),
      tester.GetCuboids(cuboid_geometry)[5].GetNx());
  CHECK_EQUAL(cuboid_7.GetNy(),
      tester.GetCuboids(cuboid_geometry)[5].GetNy());
}

TEST(TestCuboidGeometry2D_Shrink)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  auto mother_cuboid {cuboid_geometry.GetMotherCuboid()};

  Vector2D<double> extent_2 {2, 3};
  IndicatorCuboid2D<double> indicator_cuboid_2 {extent_2, origin};
  cuboid_geometry.Shrink(indicator_cuboid_2);


  CHECK_CLOSE(1.2, cuboid_geometry.rGetCuboid(0).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(3.4, cuboid_geometry.rGetCuboid(0).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(0).GetRefinementLevel());

  CHECK_CLOSE(1.2, cuboid_geometry.rGetCuboid(1).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(5.4, cuboid_geometry.rGetCuboid(1).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(1).GetRefinementLevel());
}

TEST(TestCuboidGeometry2D_RefineArea)
{
  TestCuboidGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  CHECK_EQUAL(nc, cuboid_geometry.GetNumCuboid());

  cuboid_geometry.RefineArea(3.2, 4.4, 5.7, 6.4, 0);

  CHECK_EQUAL(16, cuboid_geometry.GetNumCuboid());

  CHECK_CLOSE(3.2, cuboid_geometry.rGetCuboid(0).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(4.4, cuboid_geometry.rGetCuboid(0).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(1, cuboid_geometry.rGetCuboid(0).GetRefinementLevel());

  CHECK_CLOSE(3.2, cuboid_geometry.rGetCuboid(1).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(5.4, cuboid_geometry.rGetCuboid(1).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(1, cuboid_geometry.rGetCuboid(1).GetRefinementLevel());

  CHECK_CLOSE(1.2, cuboid_geometry.rGetCuboid(2).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(7.4, cuboid_geometry.rGetCuboid(2).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(2).GetRefinementLevel());

  CHECK_CLOSE(1.2, cuboid_geometry.rGetCuboid(3).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(9.4, cuboid_geometry.rGetCuboid(3).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(3).GetRefinementLevel());

  CHECK_CLOSE(4.7, cuboid_geometry.rGetCuboid(4).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(4.4, cuboid_geometry.rGetCuboid(4).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(1, cuboid_geometry.rGetCuboid(4).GetRefinementLevel());

  CHECK_CLOSE(4.7, cuboid_geometry.rGetCuboid(5).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(5.4, cuboid_geometry.rGetCuboid(5).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(1, cuboid_geometry.rGetCuboid(5).GetRefinementLevel());

  CHECK_CLOSE(4.7, cuboid_geometry.rGetCuboid(6).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(7.4, cuboid_geometry.rGetCuboid(6).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(6).GetRefinementLevel());

  CHECK_CLOSE(4.7, cuboid_geometry.rGetCuboid(7).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(9.4, cuboid_geometry.rGetCuboid(7).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(7).GetRefinementLevel());

  CHECK_CLOSE(1.2, cuboid_geometry.rGetCuboid(8).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(3.4, cuboid_geometry.rGetCuboid(8).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(8).GetRefinementLevel());

  CHECK_CLOSE(3.2, cuboid_geometry.rGetCuboid(9).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(3.4, cuboid_geometry.rGetCuboid(9).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(9).GetRefinementLevel());

  CHECK_CLOSE(1.2, cuboid_geometry.rGetCuboid(10).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(5.4, cuboid_geometry.rGetCuboid(10).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(10).GetRefinementLevel());

  CHECK_CLOSE(3.2, cuboid_geometry.rGetCuboid(11).GetOrigin()[0], g_zero_tol);
  // One delta_R up
  CHECK_CLOSE(6.9, cuboid_geometry.rGetCuboid(11).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(11).GetRefinementLevel());

  CHECK_CLOSE(4.7, cuboid_geometry.rGetCuboid(12).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(3.4, cuboid_geometry.rGetCuboid(12).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(12).GetRefinementLevel());

  CHECK_CLOSE(6.2, cuboid_geometry.rGetCuboid(13).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(4.4, cuboid_geometry.rGetCuboid(13).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(13).GetRefinementLevel());

  CHECK_CLOSE(6.2, cuboid_geometry.rGetCuboid(14).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(5.4, cuboid_geometry.rGetCuboid(14).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(14).GetRefinementLevel());

  CHECK_CLOSE(4.7, cuboid_geometry.rGetCuboid(15).GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(6.9, cuboid_geometry.rGetCuboid(15).GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(0, cuboid_geometry.rGetCuboid(15).GetRefinementLevel());
}

TEST(TestCuboidGeometry2D_HasCuboid_Vector)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  gsl::index global_cuboid_index {-1};
  Vector2D<double> phys_R_0 {1.3, 3.5};
  CHECK(cuboid_geometry.HasCuboid(phys_R_0, global_cuboid_index));
  CHECK_EQUAL(0, global_cuboid_index);

  Vector2D<double> phys_R_1 {1.3, 5.3};
  CHECK(cuboid_geometry.HasCuboid(phys_R_1, global_cuboid_index));
  CHECK_EQUAL(1, global_cuboid_index);

  Vector2D<double> phys_R_2 {1.3, 7.1};
  CHECK(cuboid_geometry.HasCuboid(phys_R_2, global_cuboid_index));
  CHECK_EQUAL(2, global_cuboid_index);

  Vector2D<double> phys_R_3 {1.3, 8.9};
  CHECK(cuboid_geometry.HasCuboid(phys_R_3, global_cuboid_index));
  CHECK_EQUAL(3, global_cuboid_index);

  Vector2D<double> phys_R_4 {4.4, 3.5};
  CHECK(cuboid_geometry.HasCuboid(phys_R_4, global_cuboid_index));
  CHECK_EQUAL(4, global_cuboid_index);

  Vector2D<double> phys_R_5 {4.4, 5.3};
  CHECK(cuboid_geometry.HasCuboid(phys_R_5, global_cuboid_index));
  CHECK_EQUAL(5, global_cuboid_index);

  Vector2D<double> phys_R_6 {4.4, 7.1};
  CHECK(cuboid_geometry.HasCuboid(phys_R_6, global_cuboid_index));
  CHECK_EQUAL(6, global_cuboid_index);

  Vector2D<double> phys_R_7 {4.4, 8.9};
  CHECK(cuboid_geometry.HasCuboid(phys_R_7, global_cuboid_index));
  CHECK_EQUAL(7, global_cuboid_index);

  Vector2D<double> phys_R_8 {44.4, 88.9};
  CHECK(!cuboid_geometry.HasCuboid(phys_R_8, global_cuboid_index));
}

TEST(TestCuboidGeometry2D_HasCuboid_StdVector)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  gsl::index global_cuboid_index {-1};
  std::vector<double> phys_R_0 {1.3, 3.5};
  CHECK(cuboid_geometry.HasCuboid(phys_R_0, global_cuboid_index));
  CHECK_EQUAL(0, global_cuboid_index);

  std::vector<double> phys_R_1 {1.3, 5.3};
  CHECK(cuboid_geometry.HasCuboid(phys_R_1, global_cuboid_index));
  CHECK_EQUAL(1, global_cuboid_index);

  std::vector<double> phys_R_2 {1.3, 7.1};
  CHECK(cuboid_geometry.HasCuboid(phys_R_2, global_cuboid_index));
  CHECK_EQUAL(2, global_cuboid_index);

  std::vector<double> phys_R_3 {1.3, 8.9};
  CHECK(cuboid_geometry.HasCuboid(phys_R_3, global_cuboid_index));
  CHECK_EQUAL(3, global_cuboid_index);

  std::vector<double> phys_R_4 {4.4, 3.5};
  CHECK(cuboid_geometry.HasCuboid(phys_R_4, global_cuboid_index));
  CHECK_EQUAL(4, global_cuboid_index);

  std::vector<double> phys_R_5 {4.4, 5.3};
  CHECK(cuboid_geometry.HasCuboid(phys_R_5, global_cuboid_index));
  CHECK_EQUAL(5, global_cuboid_index);

  std::vector<double> phys_R_6 {4.4, 7.1};
  CHECK(cuboid_geometry.HasCuboid(phys_R_6, global_cuboid_index));
  CHECK_EQUAL(6, global_cuboid_index);

  std::vector<double> phys_R_7 {4.4, 8.9};
  CHECK(cuboid_geometry.HasCuboid(phys_R_7, global_cuboid_index));
  CHECK_EQUAL(7, global_cuboid_index);

  std::vector<double> phys_R_8 {44.4, 88.9};
  CHECK(!cuboid_geometry.HasCuboid(phys_R_8, global_cuboid_index));
}

TEST(TestCuboidGeometry2D_GetCuboid)
{
  TestCuboidGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  for (gsl::index i {0}; i < nc; ++i) {
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(i).GetGlobalXPosition(),
        tester.GetCuboids(cuboid_geometry)[i].GetGlobalXPosition(),
        g_zero_tol);
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(i).GetGlobalYPosition(),
        tester.GetCuboids(cuboid_geometry)[i].GetGlobalYPosition(),
        g_zero_tol);
    CHECK_CLOSE(cuboid_geometry.rGetCuboid(i).GetDeltaR(),
        tester.GetCuboids(cuboid_geometry)[i].GetDeltaR(), g_zero_tol);
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(i).GetNx(),
        tester.GetCuboids(cuboid_geometry)[i].GetNx());
    CHECK_EQUAL(cuboid_geometry.rGetCuboid(i).GetNy(),
        tester.GetCuboids(cuboid_geometry)[i].GetNy());
  }
}

TEST(TestCuboidGeometry2D_GetCuboid_Const)
{
  TestCuboidGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  for (gsl::index i {0}; i < nc; ++i) {
    CHECK_CLOSE(
        std::as_const(cuboid_geometry).rGetCuboid(i).GetGlobalXPosition(),
        tester.GetCuboids(cuboid_geometry)[i].GetGlobalXPosition(),
        g_zero_tol);
    CHECK_CLOSE(
        std::as_const(cuboid_geometry).rGetCuboid(i).GetGlobalYPosition(),
        tester.GetCuboids(cuboid_geometry)[i].GetGlobalYPosition(),
        g_zero_tol);
    CHECK_CLOSE(std::as_const(cuboid_geometry).rGetCuboid(i).GetDeltaR(),
        tester.GetCuboids(cuboid_geometry)[i].GetDeltaR(), g_zero_tol);
    CHECK_EQUAL(std::as_const(cuboid_geometry).rGetCuboid(i).GetNx(),
        tester.GetCuboids(cuboid_geometry)[i].GetNx());
    CHECK_EQUAL(std::as_const(cuboid_geometry).rGetCuboid(i).GetNy(),
        tester.GetCuboids(cuboid_geometry)[i].GetNy());
  }
}

TEST(TestCuboidGeometry2D_GetLatticeR)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  gsl::index global_cuboid_index {-1};
  gsl::index lattice_R[2];

  // Rounds down
  {
    gsl::index exp_lattice_R[] {2, 3};
    double phys_R_0[] {1.44, 3.74};
    cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(0, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_1[] {1.44, 5.54};
    cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(1, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_2[] {1.44, 7.34};
    cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(2, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_3[] {1.44, 9.14};
    cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(3, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_4[] {4.54, 3.74};
    cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(4, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_5[] {4.54, 5.54};
    cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(5, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_6[] {4.54, 7.34};
    cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(6, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_7[] {4.54, 9.14};
    cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(7, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);
  }

  // Rounds up
  {
    gsl::index exp_lattice_R[] {3, 4};
    double phys_R_0[] {1.46, 3.76};
    cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(0, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_1[] {1.46, 5.56};
    cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(1, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_2[] {1.46, 7.36};
    cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(2, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_3[] {1.46, 9.16};
    cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(3, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_4[] {4.56, 3.76};
    cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(4, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_5[] {4.56, 5.56};
    cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(5, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_6[] {4.56, 7.36};
    cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(6, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);

    double phys_R_7[] {4.56, 9.16};
    cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(7, global_cuboid_index);
    CHECK_ARRAY_EQUAL(exp_lattice_R, lattice_R, 2);
  }

  double phys_R[] {44.56, 99.16};
  CHECK(!cuboid_geometry.GetLatticeR(phys_R, global_cuboid_index,
      lattice_R));
}

TEST(TestCuboidGeometry2D_GetLatticeR_StdVector)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  gsl::index global_cuboid_index {-1};
  std::vector<gsl::index> lattice_R(2);

  // Rounds down
  {
    std::vector<gsl::index> exp_lattice_R {2, 3};
    std::vector<double> phys_R_0 {1.44, 3.74};
    cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(0, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_1 {1.44, 5.54};
    cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(1, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_2 {1.44, 7.34};
    cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(2, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_3 {1.44, 9.14};
    cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(3, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_4 {4.54, 3.74};
    cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(4, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_5 {4.54, 5.54};
    cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(5, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_6 {4.54, 7.34};
    cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(6, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_7 {4.54, 9.14};
    cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(7, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));
  }

  // Rounds up
  {
    std::vector<gsl::index> exp_lattice_R {3, 4};
    std::vector<double> phys_R_0 {1.46, 3.76};
    cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(0, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_1 {1.46, 5.56};
    cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(1, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_2 {1.46, 7.36};
    cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(2, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_3 {1.46, 9.16};
    cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(3, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_4 {4.56, 3.76};
    cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(4, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_5 {4.56, 5.56};
    cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(5, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_6 {4.56, 7.36};
    cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(6, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_7 {4.56, 9.16};
    cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(7, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));
  }
}

TEST(TestCuboidGeometry2D_GetLatticeR_Vector2D)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  gsl::index global_cuboid_index {-1};
  Vector2D<gsl::index> lattice_R(2);

  // Rounds down
  {
    Vector2D<gsl::index> exp_lattice_R {2, 3};
    Vector2D<double> phys_R_0 {1.44, 3.74};
    cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(0, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_1 {1.44, 5.54};
    cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(1, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_2 {1.44, 7.34};
    cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(2, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_3 {1.44, 9.14};
    cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(3, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_4 {4.54, 3.74};
    cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(4, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_5 {4.54, 5.54};
    cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(5, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_6 {4.54, 7.34};
    cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(6, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_7 {4.54, 9.14};
    cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(7, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));
  }

  // Rounds up
  {
    Vector2D<gsl::index> exp_lattice_R {3, 4};
    Vector2D<double> phys_R_0 {1.46, 3.76};
    cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_0, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(0, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_1 {1.46, 5.56};
    cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_1, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(1, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_2 {1.46, 7.36};
    cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_2, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(2, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_3 {1.46, 9.16};
    cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_3, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(3, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_4 {4.56, 3.76};
    cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_4, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(4, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_5 {4.56, 5.56};
    cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_5, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(5, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_6 {4.56, 7.36};
    cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_6, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(6, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_7 {4.56, 9.16};
    cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index, lattice_R);
    CHECK(cuboid_geometry.GetLatticeR(phys_R_7, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(7, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));
  }

  Vector2D<double> phys_R {11.44, 33.74};
    CHECK(!cuboid_geometry.GetLatticeR(phys_R, global_cuboid_index,
        lattice_R));
}

TEST(TestCuboidGeometry2D_GetFloorLatticeR)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  gsl::index global_cuboid_index {-1};
  Vector2D<gsl::index> lattice_R;

  // Rounds down
  {
    Vector2D<gsl::index> exp_lattice_R {2, 3};
    Vector2D<double> phys_R_0 {1.44, 3.74};
    cuboid_geometry.GetFloorLatticeR(phys_R_0, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_0, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(0, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_1 {1.44, 5.54};
    cuboid_geometry.GetFloorLatticeR(phys_R_1, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_1, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(1, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_2 {1.44, 7.34};
    cuboid_geometry.GetFloorLatticeR(phys_R_2, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_2, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(2, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_3 {1.44, 9.14};
    cuboid_geometry.GetFloorLatticeR(phys_R_3, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_3, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(3, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_4 {4.54, 3.74};
    cuboid_geometry.GetFloorLatticeR(phys_R_4, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_4, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(4, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_5 {4.54, 5.54};
    cuboid_geometry.GetFloorLatticeR(phys_R_5, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_5, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(5, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_6 {4.54, 7.34};
    cuboid_geometry.GetFloorLatticeR(phys_R_6, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_6, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(6, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_7 {4.54, 9.14};
    cuboid_geometry.GetFloorLatticeR(phys_R_7, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_7, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(7, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));
  }

  // Rounds down
  {
    Vector2D<gsl::index> exp_lattice_R {2, 3};
    Vector2D<double> phys_R_0 {1.46, 3.76};
    cuboid_geometry.GetFloorLatticeR(phys_R_0, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_0, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(0, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_1 {1.46, 5.56};
    cuboid_geometry.GetFloorLatticeR(phys_R_1, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_1, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(1, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_2 {1.46, 7.36};
    cuboid_geometry.GetFloorLatticeR(phys_R_2, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_2, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(2, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_3 {1.46, 9.16};
    cuboid_geometry.GetFloorLatticeR(phys_R_3, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_3, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(3, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_4 {4.56, 3.76};
    cuboid_geometry.GetFloorLatticeR(phys_R_4, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_4, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(4, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_5 {4.56, 5.56};
    cuboid_geometry.GetFloorLatticeR(phys_R_5, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_5, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(5, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_6 {4.56, 7.36};
    cuboid_geometry.GetFloorLatticeR(phys_R_6, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_6, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(6, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    Vector2D<double> phys_R_7 {4.56, 9.16};
    cuboid_geometry.GetFloorLatticeR(phys_R_7, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_7, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(7, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));
  }

    Vector2D<double> phys_R {11.44, 33.74};
    CHECK(!cuboid_geometry.GetFloorLatticeR(phys_R, global_cuboid_index,
        lattice_R));
}

TEST(TestCuboidGeometry2D_GetFloorLatticeR_StdVector)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  gsl::index global_cuboid_index {-1};
  std::vector<gsl::index> lattice_R(2);

  // Rounds down
  {
    std::vector<gsl::index> exp_lattice_R {2, 3};
    std::vector<double> phys_R_0 {1.44, 3.74};
    cuboid_geometry.GetFloorLatticeR(phys_R_0, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_0, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(0, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_1 {1.44, 5.54};
    cuboid_geometry.GetFloorLatticeR(phys_R_1, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_1, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(1, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_2 {1.44, 7.34};
    cuboid_geometry.GetFloorLatticeR(phys_R_2, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_2, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(2, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_3 {1.44, 9.14};
    cuboid_geometry.GetFloorLatticeR(phys_R_3, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_3, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(3, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_4 {4.54, 3.74};
    cuboid_geometry.GetFloorLatticeR(phys_R_4, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_4, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(4, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_5 {4.54, 5.54};
    cuboid_geometry.GetFloorLatticeR(phys_R_5, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_5, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(5, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_6 {4.54, 7.34};
    cuboid_geometry.GetFloorLatticeR(phys_R_6, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_6, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(6, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_7 {4.54, 9.14};
    cuboid_geometry.GetFloorLatticeR(phys_R_7, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_7, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(7, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));
  }

  // Rounds down
  {
    std::vector<gsl::index> exp_lattice_R {2, 3};
    std::vector<double> phys_R_0 {1.46, 3.76};
    cuboid_geometry.GetFloorLatticeR(phys_R_0, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_0, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(0, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_1 {1.46, 5.56};
    cuboid_geometry.GetFloorLatticeR(phys_R_1, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_1, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(1, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_2 {1.46, 7.36};
    cuboid_geometry.GetFloorLatticeR(phys_R_2, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_2, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(2, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_3 {1.46, 9.16};
    cuboid_geometry.GetFloorLatticeR(phys_R_3, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_3, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(3, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_4 {4.56, 3.76};
    cuboid_geometry.GetFloorLatticeR(phys_R_4, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_4, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(4, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_5 {4.56, 5.56};
    cuboid_geometry.GetFloorLatticeR(phys_R_5, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_5, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(5, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_6 {4.56, 7.36};
    cuboid_geometry.GetFloorLatticeR(phys_R_6, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_6, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(6, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));

    std::vector<double> phys_R_7 {4.56, 9.16};
    cuboid_geometry.GetFloorLatticeR(phys_R_7, global_cuboid_index,
        lattice_R);
    CHECK(cuboid_geometry.GetFloorLatticeR(phys_R_7, global_cuboid_index,
        lattice_R));
    CHECK_EQUAL(7, global_cuboid_index);
    CHECK(testutil::CheckEqualVector(lattice_R, exp_lattice_R));
  }

  std::vector<double> phys_R {44.54, 99.14};
  CHECK(!cuboid_geometry.GetFloorLatticeR(phys_R, global_cuboid_index,
      lattice_R));
}

TEST(TestCuboidGeometry2D_GetPhysR_Index)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {5.6};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};

  // Another cuboid with the same position and size as cuboid_geometry
  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);
  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      CHECK(testutil::CheckCloseVector(cuboid_geometry.GetPhysR(0, x, y),
          cuboid.GetPhysR(x, y), g_loose_tol));
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysR_Index_Periodic)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(true, true);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      auto phys_x {fmod(cuboid.GetPhysR(x, y)[0] - x_pos + delta_R *
          nx, delta_R * nx)};
      auto phys_y {fmod(cuboid.GetPhysR(x, y)[1] - y_pos + delta_R *
          ny, delta_R * ny)};
      if (phys_x * phys_x < 1e-3 * delta_R * delta_R) phys_x = 0;
      if (phys_y * phys_y < 1e-3 * delta_R * delta_R) phys_y = 0;
      phys_x += x_pos;
      phys_y += y_pos;

      CHECK_CLOSE(phys_x, cuboid_geometry.GetPhysR(1, x, y)[0], g_loose_tol);
      CHECK_CLOSE(phys_y, cuboid_geometry.GetPhysR(1, x, y)[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysR_Index_Periodic_XOnly)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(true, false);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      auto phys_x {fmod(cuboid.GetPhysR(x, y)[0] - x_pos + delta_R *
          nx, delta_R * nx)};
      if (phys_x * phys_x < 1e-3 * delta_R * delta_R) phys_x = 0;
      phys_x += x_pos;

      CHECK_CLOSE(phys_x, cuboid_geometry.GetPhysR(1, x, y)[0], g_loose_tol);
      CHECK_CLOSE(cuboid.GetPhysR(x, y)[1],
          cuboid_geometry.GetPhysR(1, x, y)[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysR_Index_Periodic_YOnly)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(false, true);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      auto phys_y {fmod(cuboid.GetPhysR(x, y)[1] - y_pos + delta_R *
          ny, delta_R * ny)};
      if (phys_y * phys_y < 1e-3 * delta_R * delta_R) phys_y = 0;
      phys_y += y_pos;

      CHECK_CLOSE(cuboid.GetPhysR(x, y)[0],
          cuboid_geometry.GetPhysR(1, x, y)[0], g_loose_tol);
      CHECK_CLOSE(phys_y, cuboid_geometry.GetPhysR(1, x, y)[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysR_StdVector)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {5.6};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};

  // Another cuboid with the same position and size as cuboid_geometry
  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);
  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      std::vector<gsl::index> lattice_R {x, y};
      CHECK(testutil::CheckCloseVector(cuboid_geometry.GetPhysR(0,
          lattice_R), cuboid.GetPhysR(x, y), g_loose_tol));
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysR_StdVector_Periodic)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(true, true);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      std::vector<gsl::index> lattice_R {x, y};
      auto phys_x {fmod(cuboid.GetPhysR(x, y)[0] - x_pos + delta_R *
          nx, delta_R * nx)};
      auto phys_y {fmod(cuboid.GetPhysR(x, y)[1] - y_pos + delta_R *
          ny, delta_R * ny)};
      if (phys_x * phys_x < 1e-3 * delta_R * delta_R) phys_x = 0;
      if (phys_y * phys_y < 1e-3 * delta_R * delta_R) phys_y = 0;
      phys_x += x_pos;
      phys_y += y_pos;

      CHECK_CLOSE(phys_x, cuboid_geometry.GetPhysR(1, lattice_R)[0],
          g_loose_tol);
      CHECK_CLOSE(phys_y, cuboid_geometry.GetPhysR(1, lattice_R)[1],
          g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysR_StdVector_Periodic_XOnly)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(true, false);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      std::vector<gsl::index> lattice_R {x, y};
      auto phys_x {fmod(cuboid.GetPhysR(x, y)[0] - x_pos + delta_R *
          nx, delta_R * nx)};
      if (phys_x * phys_x < 1e-3 * delta_R * delta_R) phys_x = 0;
      phys_x += x_pos;

      CHECK_CLOSE(phys_x, cuboid_geometry.GetPhysR(1, lattice_R)[0],
          g_loose_tol);
      CHECK_CLOSE(cuboid.GetPhysR(x, y)[1], cuboid_geometry.GetPhysR(1,
          lattice_R)[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysR_StdVector_Periodic_YOnly)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(false, true);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      std::vector<gsl::index> lattice_R {x, y};
      auto phys_y {fmod(cuboid.GetPhysR(x, y)[1] - y_pos + delta_R *
          ny, delta_R * ny)};
      if (phys_y * phys_y < 1e-3 * delta_R * delta_R) phys_y = 0;
      phys_y += y_pos;

      CHECK_CLOSE(cuboid.GetPhysR(x, y)[0], cuboid_geometry.GetPhysR(1,
          lattice_R)[0], g_loose_tol);
      CHECK_CLOSE(phys_y, cuboid_geometry.GetPhysR(1, lattice_R)[1],
          g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysROutput_Index)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {5.6};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};

  // Another cuboid with the same position and size as cuboid_geometry
  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);
  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      double phys_R[2];
      auto exp_phys_R {cuboid.GetPhysR(x, y)};
      cuboid_geometry.GetPhysR(0, x, y, phys_R);

      CHECK_CLOSE(exp_phys_R[0], phys_R[0], g_loose_tol);
      CHECK_CLOSE(exp_phys_R[1], phys_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysROutput_Index_Periodic)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(true, true);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      auto phys_x {fmod(cuboid.GetPhysR(x, y)[0] - x_pos + delta_R *
          nx, delta_R * nx)};
      auto phys_y {fmod(cuboid.GetPhysR(x, y)[1] - y_pos + delta_R *
          ny, delta_R * ny)};
      if (phys_x * phys_x < 1e-3 * delta_R * delta_R) phys_x = 0;
      if (phys_y * phys_y < 1e-3 * delta_R * delta_R) phys_y = 0;
      phys_x += x_pos;
      phys_y += y_pos;

      double phys_R[2];
      cuboid_geometry.GetPhysR(1, x, y, phys_R);

      CHECK_CLOSE(phys_x, phys_R[0], g_loose_tol);
      CHECK_CLOSE(phys_y, phys_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysROutput_Index_Periodic_XOnly)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(true, false);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      auto phys_x {fmod(cuboid.GetPhysR(x, y)[0] - x_pos + delta_R *
          nx, delta_R * nx)};
      if (phys_x * phys_x < 1e-3 * delta_R * delta_R) phys_x = 0;
      phys_x += x_pos;

      double phys_R[2];
      cuboid_geometry.GetPhysR(1, x, y, phys_R);

      CHECK_CLOSE(phys_x, phys_R[0], g_loose_tol);
      CHECK_CLOSE(cuboid.GetPhysR(x, y)[1], phys_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysROutput_Index_Periodic_YOnly)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(false, true);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      auto phys_y {fmod(cuboid.GetPhysR(x, y)[1] - y_pos + delta_R *
          ny, delta_R * ny)};
      if (phys_y * phys_y < 1e-3 * delta_R * delta_R) phys_y = 0;
      phys_y += y_pos;

      double phys_R[2];
      cuboid_geometry.GetPhysR(1, x, y, phys_R);

      CHECK_CLOSE(cuboid.GetPhysR(x, y)[0], phys_R[0], g_loose_tol);
      CHECK_CLOSE(phys_y, phys_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysROutput_Array)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {5.6};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};

  // Another cuboid with the same position and size as cuboid_geometry
  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);
  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      double phys_R[2];
      gsl::index lattice_R[2] {x, y};
      auto exp_phys_R {cuboid.GetPhysR(x, y)};
      cuboid_geometry.GetPhysR(0, lattice_R, phys_R);

      CHECK_CLOSE(exp_phys_R[0], phys_R[0], g_loose_tol);
      CHECK_CLOSE(exp_phys_R[1], phys_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysROutput_Array_Periodic)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(true, true);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      auto phys_x {fmod(cuboid.GetPhysR(x, y)[0] - x_pos + delta_R *
          nx, delta_R * nx)};
      auto phys_y {fmod(cuboid.GetPhysR(x, y)[1] - y_pos + delta_R *
          ny, delta_R * ny)};
      if (phys_x * phys_x < 1e-3 * delta_R * delta_R) phys_x = 0;
      if (phys_y * phys_y < 1e-3 * delta_R * delta_R) phys_y = 0;
      phys_x += x_pos;
      phys_y += y_pos;

      double phys_R[2];
      gsl::index lattice_R[2] {x, y};
      cuboid_geometry.GetPhysR(1, lattice_R, phys_R);

      CHECK_CLOSE(phys_x, phys_R[0], g_loose_tol);
      CHECK_CLOSE(phys_y, phys_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysROutput_Array_Periodic_XOnly)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(true, false);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      auto phys_x {fmod(cuboid.GetPhysR(x, y)[0] - x_pos + delta_R *
          nx, delta_R * nx)};
      if (phys_x * phys_x < 1e-3 * delta_R * delta_R) phys_x = 0;
      phys_x += x_pos;

      double phys_R[2];
      gsl::index lattice_R[2] {x, y};
      cuboid_geometry.GetPhysR(1, lattice_R, phys_R);

      CHECK_CLOSE(phys_x, phys_R[0], g_loose_tol);
      CHECK_CLOSE(cuboid.GetPhysR(x, y)[1], phys_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetPhysROutput_Array_Periodic_YOnly)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  // Another cuboid 2 * delta_R away from mother_cuboid
  Cuboid2D<double> cuboid(x_pos + delta_R * 2, y_pos + delta_R * 2, delta_R,
      nx, ny);
  cuboid_geometry.Add(cuboid);
  cuboid_geometry.SetIsPeriodic(false, true);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      auto phys_y {fmod(cuboid.GetPhysR(x, y)[1] - y_pos + delta_R *
          ny, delta_R * ny)};
      if (phys_y * phys_y < 1e-3 * delta_R * delta_R) phys_y = 0;
      phys_y += y_pos;

      double phys_R[2];
      gsl::index lattice_R[2] {x, y};
      cuboid_geometry.GetPhysR(1, lattice_R, phys_R);

      CHECK_CLOSE(cuboid.GetPhysR(x, y)[0], phys_R[0], g_loose_tol);
      CHECK_CLOSE(phys_y, phys_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboidGeometry2D_GetMinPhysR)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};
  auto nx {6u};
  auto ny {7u};

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny, 8);

  CHECK_CLOSE(x_pos, cuboid_geometry.GetMinPhysR()[0], g_loose_tol);
  CHECK_CLOSE(y_pos, cuboid_geometry.GetMinPhysR()[1], g_loose_tol);

  cuboid_geometry.rGetCuboid(1).Initialize(x_pos - 0.1, y_pos - 0.1, delta_R,
      nx, ny);
  CHECK_CLOSE(x_pos - 0.1, cuboid_geometry.GetMinPhysR()[0], g_loose_tol);
  CHECK_CLOSE(y_pos - 0.1, cuboid_geometry.GetMinPhysR()[1], g_loose_tol);
}

TEST(TestCuboidGeometry2D_GetMaxPhysR)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};
  auto nx {6u};
  auto ny {7u};

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny, 8);

  auto exp_max_x = x_pos + delta_R * nx;
  auto exp_max_y = y_pos + delta_R * ny;

  CHECK_CLOSE(exp_max_x, cuboid_geometry.GetMaxPhysR()[0], g_loose_tol);
  CHECK_CLOSE(exp_max_y, cuboid_geometry.GetMaxPhysR()[1], g_loose_tol);
}

TEST(TestCuboidGeometry2D_GetMinRatio)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {11u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  CHECK_CLOSE(17.0 / 24.0, cuboid_geometry.GetMinRatio(), g_loose_tol);
}

TEST(TestCuboidGeometry2D_GetMaxRatio)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {11u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  CHECK_CLOSE(22.0 / 17.0, cuboid_geometry.GetMaxRatio(), g_loose_tol);
}

TEST(TestCuboidGeometry2D_GetMinPhysVolume)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {11u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  CHECK_CLOSE(3.74, cuboid_geometry.GetMinPhysVolume(), g_loose_tol);
}

TEST(TestCuboidGeometry2D_GetMaxPhysVolume)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {11u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  CHECK_CLOSE(4.08, cuboid_geometry.GetMaxPhysVolume(), g_loose_tol);

  cuboid_geometry.rGetCuboid(1).Initialize(x_pos, y_pos, delta_R,
      100, 100);
  CHECK_CLOSE(100.0, cuboid_geometry.GetMaxPhysVolume(), g_loose_tol);
}

TEST(TestCuboidGeometry2D_GetMinLatticeVolume)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {11u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  CHECK_CLOSE(374, cuboid_geometry.GetMinLatticeVolume(), g_loose_tol);
}

TEST(TestCuboidGeometry2D_GetMaxLatticeVolume)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {11u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  CHECK_CLOSE(408, cuboid_geometry.GetMaxLatticeVolume(), g_loose_tol);

  cuboid_geometry.rGetCuboid(1).Initialize(x_pos, y_pos, delta_R,
      100, 100);
  CHECK_EQUAL(10000, cuboid_geometry.GetMaxLatticeVolume());
}

TEST(TestCuboidGeometry2D_GetMinDeltaR)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};
  auto nx {6u};
  auto ny {7u};

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny, 8);

  CHECK_CLOSE(delta_R, cuboid_geometry.GetMinDeltaR(), g_loose_tol);
  cuboid_geometry.rGetCuboid(1).Initialize(x_pos, y_pos, 0.5 * delta_R,
      100, 100);
  CHECK_CLOSE(0.5 * delta_R, cuboid_geometry.GetMinDeltaR(), g_loose_tol);
}

TEST(TestCuboidGeometry2D_GetMaxDeltaR)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};
  auto nx {6u};
  auto ny {7u};

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny, 8);

  CHECK_CLOSE(delta_R, cuboid_geometry.GetMaxDeltaR(), g_loose_tol);
  cuboid_geometry.rGetCuboid(1).Initialize(x_pos, y_pos, 2.0 * delta_R,
      100, 100);
  CHECK_CLOSE(2.0 * delta_R, cuboid_geometry.GetMaxDeltaR(), g_loose_tol);
}

TEST(TestCuboidGeometry2D_GetGlobalCuboidIndex)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};

  // Cuboids will be split into 2 x 4 (col x row) with lower left coords
  // (1.2, 3.4), (1.2, 5.4), (1.2, 7.4), (1.2, 9.4), (4.7, 3.4), (4.7, 5.4),
  // (4.7, 7.4), (4.7, 9.4)
  // Create test coordinates that are either inside or outside by an offset
  // of delta_R
  std::vector<Vector2D<double>> phys_R {{1.7, 3.9}, {1.7, 5.9}, {1.7, 7.9},
      {1.7, 9.9}, {5.2, 3.9}, {5.2, 5.9}, {5.2, 7.9}, {5.2, 9.9}, {0.7, 2.9},
      {0.7, 4.9}, {0.7, 6.9}, {0.7, 8.}, {4.2, 2.9}, {4.2, 4.9}, {4.2, 6.9},
      {4.2, 8.9}};
  std::vector<gsl::index> cuboid_index {0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1,
      -1, -1, 0, 1, 2};

  for (gsl::index i {0}; i < phys_R.size(); ++i) {
    CHECK_EQUAL(cuboid_index[i], cuboid_geometry.GetGlobalCuboidIndex(
        phys_R[i][0], phys_R[i][1]));
  }
}

TEST(TestCuboidGeometry2D_SetPeriodic)
{
  TestCuboidGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {5.6};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny};
  auto is_periodic {tester.GetIsPeriodic(cuboid_geometry)};
  CHECK_EQUAL(2u, is_periodic.size());
  CHECK(is_periodic[0] == false);
  CHECK(is_periodic[1] == false);

  cuboid_geometry.SetIsPeriodic(true, true);
  is_periodic = tester.GetIsPeriodic(cuboid_geometry);
  CHECK(is_periodic[0] == true);
  CHECK(is_periodic[1] == true);
}
}
}  // namespace iblbm
