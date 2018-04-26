#include <iostream>

#include "UnitTest++/UnitTest++.h"
#include "BlockGeometry2D.hpp"
#include "BlockGeometryStatistics2D.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
SUITE(TestGeometry)
{
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestBlockGeometryStatistics2D_Constructor)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
  BlockGeometryStatistics2D<double> statistics {&block_geometry};

  std::vector<gsl::index> min_lattice_R {0, 0};
  std::vector<gsl::index> max_lattice_R {nx - 1, ny - 1};
  std::vector<double> min_phys_R {x_pos, y_pos};
  std::vector<double> max_phys_R {x_pos + static_cast<double>(nx - 1) *
      delta_R, y_pos + static_cast<double>(ny - 1) * delta_R};
  std::vector<double> lattice_extent {static_cast<double>(nx - 1),
      static_cast<double>(ny - 1)};
  std::vector<double> phys_extent {static_cast<double>(nx - 1) * delta_R,
      static_cast<double>(ny - 1) * delta_R};
  std::vector<double> phys_radius {0.5 * delta_R *
      static_cast<double>(nx - 1), 0.5 * delta_R *
      static_cast<double>(ny - 1)};
  std::vector<double> center_phys_R {x_pos + 0.5 * delta_R *
      static_cast<double>(nx - 1), y_pos + 0.5 * delta_R *
      static_cast<double>(ny - 1)};

  CHECK(statistics.rGetStatus());
  CHECK(std::as_const(statistics).rGetStatus());
  CHECK_EQUAL(nx * ny, statistics.GetMaterialToNum()[0]);
  CHECK_EQUAL(1, statistics.GetNumMaterial());
  CHECK_EQUAL(nx * ny, statistics.GetNumVoxel(0));
  CHECK_EQUAL(0, statistics.GetNumVoxel());
  CHECK(testutil::CheckEqualVector(min_lattice_R,
      statistics.GetMinLatticeR(0)));
  CHECK(testutil::CheckEqualVector(max_lattice_R,
      statistics.GetMaxLatticeR(0)));
  CHECK(testutil::CheckCloseVector(min_phys_R, statistics.GetMinPhysR(0),
      g_loose_tol));
  CHECK(testutil::CheckCloseVector(max_phys_R, statistics.GetMaxPhysR(0),
      g_loose_tol));
  CHECK(testutil::CheckCloseVector(lattice_extent,
      statistics.GetLatticeExtent(0), g_loose_tol));
  CHECK(testutil::CheckCloseVector(phys_extent, statistics.GetPhysExtent(0),
      g_loose_tol));
  CHECK(testutil::CheckCloseVector(phys_radius, statistics.GetPhysRadius(0),
      g_loose_tol));
  CHECK(testutil::CheckCloseVector(center_phys_R,
      statistics.GetCenterPhysR(0), g_loose_tol));
}

TEST(TestBlockGeometryStatistics2D_Getter_MultipleMaterial)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
  BlockGeometryStatistics2D<double> statistics {&block_geometry};

  std::vector<unsigned> num_material(3, 0);
  auto counter {0u};
  std::vector<std::vector<gsl::index>> min_lattice_R {{0, 0}, {nx / 2, 0},
      {nx / 2, ny / 2}};
  std::vector<std::vector<gsl::index>> max_lattice_R {{nx / 2 - 1, ny - 1},
      {nx - 1, ny / 2 - 1}, {nx - 1, ny - 1}};
  std::vector<std::vector<double>> min_phys_R {{x_pos, y_pos},
      {x_pos + static_cast<double>(nx / 2) * delta_R, y_pos},
      {x_pos + static_cast<double>(nx / 2) * delta_R, y_pos +
          static_cast<double>(ny / 2) * delta_R}};
  std::vector<std::vector<double>> max_phys_R {
      {x_pos + static_cast<double>(nx / 2 - 1) * delta_R, y_pos +
          static_cast<double>(ny - 1) * delta_R},
      {x_pos + static_cast<double>(nx - 1) * delta_R, y_pos +
          static_cast<double>(ny / 2 - 1) * delta_R},
      {x_pos + static_cast<double>(nx - 1) * delta_R, y_pos +
          static_cast<double>(ny - 1) * delta_R}};
  std::vector<std::vector<double>> lattice_extent {
      {static_cast<double>(nx / 2 - 1), static_cast<double>(ny - 1)},
      {static_cast<double>(nx / 2), static_cast<double>(ny / 2 - 1)},
      {static_cast<double>(nx / 2), static_cast<double>(ny / 2 - 1)}};
  std::vector<std::vector<double>> phys_extent {
      {lattice_extent[0][0] * delta_R, lattice_extent[0][1] * delta_R},
      {lattice_extent[1][0] * delta_R, lattice_extent[1][1] * delta_R},
      {lattice_extent[2][0] * delta_R, lattice_extent[2][1] * delta_R}};
  std::vector<std::vector<double>> phys_radius {
      {0.5 * phys_extent[0][0], 0.5 * phys_extent[0][1]},
      {0.5 * phys_extent[1][0], 0.5 * phys_extent[1][1]},
      {0.5 * phys_extent[2][0], 0.5 * phys_extent[2][1]}};
  std::vector<std::vector<double>> center_phys_R {
      {min_phys_R[0][0] + phys_radius[0][0], min_phys_R[0][1] +
          phys_radius[0][1]},
      {min_phys_R[1][0] + phys_radius[1][0], min_phys_R[1][1] +
          phys_radius[1][1]},
      {min_phys_R[2][0] + phys_radius[2][0], min_phys_R[2][1] +
          phys_radius[2][1]}};

  // +-------+  + -> y
  // |   0   |  | +
  // +---+---+  v
  // | 1 | 2 |  x
  // +---+---+
  for (gsl::index x {0}; x < nx / 2; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetData(x, y) = 0;
      ++num_material[0];
    }
  }
  for (gsl::index x {nx / 2}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny / 2; ++y) {
      block_geometry.rGetData(x, y) = 1;
      block_geometry.rGetData(x, y + ny / 2) = 2;
      ++num_material[1];
      ++num_material[2];
      counter += 2;
    }
  }

  auto material_to_num {statistics.GetMaterialToNum()};
  for (gsl::index i {0}; i < num_material.size(); ++i)
      CHECK_EQUAL(num_material[i], material_to_num[i]);
  CHECK_EQUAL(counter, statistics.GetNumVoxel());
  for (gsl::index i {0}; i < min_lattice_R.size(); ++i) {
    CHECK(testutil::CheckEqualVector(min_lattice_R[i],
        statistics.GetMinLatticeR(i)));
    CHECK(testutil::CheckEqualVector(max_lattice_R[i],
        statistics.GetMaxLatticeR(i)));
    CHECK(testutil::CheckCloseVector(min_phys_R[i],
        statistics.GetMinPhysR(i), g_loose_tol));
    CHECK(testutil::CheckCloseVector(max_phys_R[i],
        statistics.GetMaxPhysR(i), g_loose_tol));
    CHECK(testutil::CheckCloseVector(lattice_extent[i],
        statistics.GetLatticeExtent(i), g_loose_tol));
    CHECK(testutil::CheckCloseVector(phys_extent[i],
        statistics.GetPhysExtent(i), g_loose_tol));
    CHECK(testutil::CheckCloseVector(phys_radius[i],
        statistics.GetPhysRadius(i), g_loose_tol));
    CHECK(testutil::CheckCloseVector(center_phys_R[i],
        statistics.GetCenterPhysR(i), g_loose_tol));
  }
}

TEST(TestBlockGeometryStatistics2D_GetType)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  gsl::index x {2};
  gsl::index y {3};

  // Do nothing and fluid node
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};
    std::vector<int> zero_vec(3, 0);

    CHECK(testutil::CheckEqualVector(zero_vec, statistics.GetType(x, y)));
    block_geometry.rGetData(x, y) = 1;
    CHECK(testutil::CheckEqualVector(zero_vec, statistics.GetType(x, y)));
  }

  // boundary along y-axis
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y) = 2;
    block_geometry.rGetData(x, y - 1) = 2;
    block_geometry.rGetData(x, y + 1) = 2;

    block_geometry.rGetData(x + 1, y) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{0, -1, 0},
        statistics.GetType(x, y)));

    block_geometry.rGetData(x + 1, y) = 0;
    block_geometry.rGetData(x - 1, y) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{0, 1, 0},
        statistics.GetType(x, y)));
  }

  // boundary along x-axis
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y) = 2;
    block_geometry.rGetData(x - 1, y) = 2;
    block_geometry.rGetData(x + 1, y) = 2;

    block_geometry.rGetData(x, y + 1) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{0, 0, -1},
        statistics.GetType(x, y)));

    block_geometry.rGetData(x, y + 1) = 0;
    block_geometry.rGetData(x, y - 1) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{0, 0, 1},
        statistics.GetType(x, y)));
  }

  // external corner left
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y) = 2;
    block_geometry.rGetData(x + 1, y) = 2;

    block_geometry.rGetData(x, y + 1) = 2;
    block_geometry.rGetData(x + 1, y + 1) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{1, -1, -1},
        statistics.GetType(x, y)));

    block_geometry.rGetData(x, y + 1) = 0;
    block_geometry.rGetData(x + 1, y + 1) = 0;
    block_geometry.rGetData(x, y - 1) = 2;
    block_geometry.rGetData(x + 1, y - 1) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{1, -1, 1},
        statistics.GetType(x, y)));
  }

  // external corner right
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y) = 2;
    block_geometry.rGetData(x - 1, y) = 2;

    block_geometry.rGetData(x, y + 1) = 2;
    block_geometry.rGetData(x - 1, y + 1) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{1, 1, -1},
        statistics.GetType(x, y)));

    block_geometry.rGetData(x, y + 1) = 0;
    block_geometry.rGetData(x + 1, y + 1) = 0;
    block_geometry.rGetData(x, y - 1) = 2;
    block_geometry.rGetData(x - 1, y - 1) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{1, 1, 1},
        statistics.GetType(x, y)));
  }

  // internal corner right
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y) = 2;
    block_geometry.rGetData(x - 1, y) = 2;

    block_geometry.rGetData(x, y - 1) = 2;
    block_geometry.rGetData(x - 1, y - 1) = 0;
    CHECK(testutil::CheckEqualVector(std::vector<int>{2, -1, -1},
        statistics.GetType(x, y)));

    block_geometry.rGetData(x, y - 1) = 0;
    block_geometry.rGetData(x - 1, y - 1) = 0;
    block_geometry.rGetData(x, y + 1) = 2;
    block_geometry.rGetData(x - 1, y + 1) = 0;
    CHECK(testutil::CheckEqualVector(std::vector<int>{2, -1, 1},
        statistics.GetType(x, y)));
  }

  // internal corner left
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y) = 2;
    block_geometry.rGetData(x + 1, y) = 2;

    block_geometry.rGetData(x, y - 1) = 2;
    block_geometry.rGetData(x + 1, y - 1) = 0;
    CHECK(testutil::CheckEqualVector(std::vector<int>{2, 1, -1},
        statistics.GetType(x, y)));

    block_geometry.rGetData(x, y - 1) = 0;
    block_geometry.rGetData(x + 1, y - 1) = 0;
    block_geometry.rGetData(x, y + 1) = 2;
    block_geometry.rGetData(x + 1, y + 1) = 0;
    CHECK(testutil::CheckEqualVector(std::vector<int>{2, 1, 1},
        statistics.GetType(x, y)));
  }
}

TEST(TestBlockGeometryStatistics2D_ComputeNormal)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  gsl::index x {2};
  gsl::index y {3};

  // Do nothing
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};
    std::vector<int> zero_vec(2, 0);

    CHECK(testutil::CheckEqualVector(zero_vec,
        statistics.ComputeNormal(x, y)));
  }

  // Left
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x - 1, y) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{-1, 0},
        statistics.ComputeNormal(x, y)));
  }

  // Right
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x + 1, y) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{1, 0},
        statistics.ComputeNormal(x, y)));
  }

  // Lower
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y - 1) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{0, -1},
        statistics.ComputeNormal(x, y)));
  }

  // Upper
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y + 1) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{0, 1},
        statistics.ComputeNormal(x, y)));
  }

  // Lower left
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y - 1) = 1;
    block_geometry.rGetData(x - 1, y) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{-1, -1},
        statistics.ComputeNormal(x, y)));
  }

  // Upper left
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y + 1) = 1;
    block_geometry.rGetData(x - 1, y) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{-1, 1},
        statistics.ComputeNormal(x, y)));
  }

  // Lower right
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y - 1) = 1;
    block_geometry.rGetData(x + 1, y) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{1, -1},
        statistics.ComputeNormal(x, y)));
  }

  // Upper right
  {
    BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
    BlockGeometryStatistics2D<double> statistics {&block_geometry};

    block_geometry.rGetData(x, y + 1) = 1;
    block_geometry.rGetData(x + 1, y) = 1;
    CHECK(testutil::CheckEqualVector(std::vector<int>{1, 1},
        statistics.ComputeNormal(x, y)));
  }
}

TEST(TestBlockGeometryStatistics2D_ComputeNormal_Material)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
  BlockGeometryStatistics2D<double> statistics {&block_geometry};

  // +-------+  + -> y
  // |   0   |  | +
  // +---+---+  v
  // | 1 | 2 |  x
  // +---+---+
  for (gsl::index x {0}; x < nx / 2; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetData(x, y) = 0;
    }
  }
  for (gsl::index x {nx / 2}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny / 2; ++y) {
      block_geometry.rGetData(x, y) = 1;
      block_geometry.rGetData(x, y + ny / 2) = 2;
    }
  }
  CHECK(testutil::CheckCloseVector(std::vector<double>{1, 0},
      statistics.ComputeNormal(0), g_loose_tol));
  CHECK(testutil::CheckCloseVector(std::vector<double>{0, -1},
      statistics.ComputeNormal(2), g_loose_tol));
}

TEST(TestBlockGeometryStatistics2D_ComputeDiscreteNormal)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
  BlockGeometryStatistics2D<double> statistics {&block_geometry};

  // +-------+  + -> y
  // |   0   |  | +
  // +---+---+  v
  // | 1 | 2 |  x
  // +---+---+
  for (gsl::index x {0}; x < nx / 2; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetData(x, y) = 0;
    }
  }
  for (gsl::index x {nx / 2}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny / 2; ++y) {
      block_geometry.rGetData(x, y) = 1;
      block_geometry.rGetData(x, y + ny / 2) = 2;
    }
  }
  CHECK(testutil::CheckCloseVector(std::vector<int>{1, 0},
      statistics.ComputeDiscreteNormal(0), g_loose_tol));
  CHECK(testutil::CheckCloseVector(std::vector<int>{0, -1},
      statistics.ComputeDiscreteNormal(2), g_loose_tol));
}
}
}  // namespace iblbm
