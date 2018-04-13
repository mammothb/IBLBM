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

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny);
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

  IndicatorCuboid2D<double> indicator_cuboid(extent, origin);
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

  IndicatorCuboid2D<double> indicator_cuboid(extent, origin);
  CuboidGeometry2D<double> cuboid_geometry(indicator_cuboid, delta_R, nc);
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

TEST(TestCuboidGeometry2D_GetMinPhysR)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};
  auto nx {6u};
  auto ny {7u};

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny, 8);

  auto exp_min_x = x_pos;
  auto exp_min_y = y_pos;

  CHECK_CLOSE(exp_min_x, cuboid_geometry.GetMinPhysR()[0], g_loose_tol);
  CHECK_CLOSE(exp_min_y, cuboid_geometry.GetMinPhysR()[1], g_loose_tol);
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

TEST(TestCuboidGeometry2D_GetMinDeltaR)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};
  auto nx {6u};
  auto ny {7u};

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny, 8);

  CHECK_CLOSE(delta_R, cuboid_geometry.GetMinDeltaR(), g_loose_tol);
}

TEST(TestCuboidGeometry2D_GetPhysR_Index)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {5.6};
  auto nx {7u};
  auto ny {8u};

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny);

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

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny);
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

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny);
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

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny);
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

TEST(TestCuboidGeometry2D_GetGlobalCuboidIndex)
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

  CuboidGeometry2D<double> cuboid_geometry(x_pos, y_pos, delta_R, nx, ny);
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
