#include <iostream>
#include <limits>

#include "UnitTest++/UnitTest++.h"
#include "Cuboid2D.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
SUITE(TestGeometry)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;

TEST(TestCuboid2D_Constructor_Default)
{
  auto x_pos = 1.2;
  auto y_pos = 3.4;
  auto delta_r = 5.6;
  auto nx = 7u;
  auto ny = 8u;

  Cuboid2D cuboid(x_pos, y_pos, delta_r, nx, ny);

  auto exp_phys_perimeter = 2.0 * delta_r * nx + 2.0 * delta_r * ny;
  auto exp_lattice_perimenter = 2 * nx + 2 * ny - 4;
  auto exp_phys_volume = delta_r * nx * delta_r * ny;
  auto exp_lattice_volume = nx * ny;

  CHECK_CLOSE(x_pos, cuboid.GetGlobalXPosition(), g_zero_tol);
  CHECK_CLOSE(y_pos, cuboid.GetGlobalYPosition(), g_zero_tol);
  CHECK_CLOSE(x_pos, cuboid.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, cuboid.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, cuboid.GetNx());
  CHECK_EQUAL(ny, cuboid.GetNy());
  CHECK_EQUAL(nx, cuboid.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, cuboid.GetLatticeExtent()[1]);
  CHECK_EQUAL(std::numeric_limits<std::size_t>::max(), cuboid.GetWeight());
  CHECK_CLOSE(exp_phys_perimeter, cuboid.GetPhysPerimeter(), g_loose_tol);
  CHECK_EQUAL(exp_lattice_perimenter, cuboid.GetLatticePerimeter());
  CHECK_CLOSE(exp_phys_volume, cuboid.GetPhysVolume(), g_loose_tol);
  CHECK_EQUAL(exp_lattice_volume, cuboid.GetLatticeVolume());
}
}
}  // namespace iblbm
