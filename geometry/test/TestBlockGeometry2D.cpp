#include <array>
#include <iostream>
#include <list>

#include "UnitTest++/UnitTest++.h"
#include "BlockGeometry2D.hpp"
#include "Descriptor.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
class TestBlockGeometry2D
{
 public:
  std::list<bool*> GetStatisticsList(
      const BlockGeometry2D<double>& rBlockGeometry)
  {
    return rBlockGeometry.mNeedStatisticsUpdate;
  }

  void ResetStatistics(BlockGeometry2D<double>& rBlockGeometry)
  {
    rBlockGeometry.ResetStatistics();
  }
};

SUITE(TestGeometry)
{
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestBlockGeometry2D_Constructor_Index_Default)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};
  auto statistics {block_geometry.rGetStatistics()};
  const auto const_stats {std::as_const(block_geometry).rGetStatistics()};

  CHECK_EQUAL(-1, block_geometry.rGetGlobalCuboidIndex());
  CHECK_EQUAL(nx, block_geometry.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, block_geometry.GetLatticeExtent()[1]);
  CHECK_CLOSE(x_pos, block_geometry.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, block_geometry.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, block_geometry.GetNx());
  CHECK_EQUAL(ny, block_geometry.GetNy());
  CHECK_CLOSE(delta_R, block_geometry.GetDeltaR(), g_zero_tol);

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
  gsl::index x_index {0};
  gsl::index y_index {0};
  CHECK(statistics.Find(/*material=*/0, /*xOffset=*/1, /*yOffset=*/2,
      x_index, y_index));
  // Since material default to 0 when out of bounds
  CHECK_EQUAL(0, x_index);
  CHECK_EQUAL(0, y_index);
  CHECK(statistics.Find(/*material=*/0, /*xOffset=*/2, /*yOffset=*/3,
      x_index, y_index));
  CHECK_EQUAL(0, x_index);
  CHECK_EQUAL(0, y_index);
  CHECK(statistics.Find(/*material=*/0, /*xOffset=*/2, /*yOffset=*/3,
      x_index, y_index));
  CHECK_EQUAL(0, x_index);
  CHECK_EQUAL(0, y_index);
  CHECK(statistics.Find(/*material=*/1, /*xOffset=*/1, /*yOffset=*/1,
      x_index, y_index));
  CHECK_EQUAL(4, x_index);
  CHECK_EQUAL(1, y_index);
  CHECK(!statistics.Find(/*material=*/1, /*xOffset=*/2, /*yOffset=*/2,
      x_index, y_index));
  CHECK_EQUAL(nx, x_index);
  CHECK_EQUAL(ny, y_index);
  CHECK(statistics.Find(/*material=*/2, /*xOffset=*/1, /*yOffset=*/1,
      x_index, y_index));
  CHECK_EQUAL(4, x_index);
  CHECK_EQUAL(5, y_index);
  CHECK(!statistics.Find(/*material=*/2, /*xOffset=*/2, /*yOffset=*/2,
      x_index, y_index));
  CHECK_EQUAL(nx, x_index);
  CHECK_EQUAL(ny, y_index);

  CHECK(const_stats.rGetStatus());
}

TEST(TestBlockGeometry2D_Constructor_Index_UserDefined)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};
  auto cuboid_index {9};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny,
      cuboid_index};

  CHECK_EQUAL(cuboid_index, block_geometry.rGetGlobalCuboidIndex());
  CHECK_EQUAL(nx, block_geometry.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, block_geometry.GetLatticeExtent()[1]);
  CHECK_CLOSE(x_pos, block_geometry.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, block_geometry.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, block_geometry.GetNx());
  CHECK_EQUAL(ny, block_geometry.GetNy());
  CHECK_CLOSE(delta_R, block_geometry.GetDeltaR(), g_zero_tol);
}

TEST(TestBlockGeometry2D_Constructor_Cuboid_Default)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockGeometry2D<double> block_geometry {cuboid};

  CHECK_EQUAL(-1, block_geometry.rGetGlobalCuboidIndex());
  CHECK_EQUAL(nx, block_geometry.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, block_geometry.GetLatticeExtent()[1]);
  CHECK_CLOSE(x_pos, block_geometry.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, block_geometry.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, block_geometry.GetNx());
  CHECK_EQUAL(ny, block_geometry.GetNy());
  CHECK_CLOSE(delta_R, block_geometry.GetDeltaR(), g_zero_tol);
}

TEST(TestBlockGeometry2D_Constructor_Cuboid_UserDefined)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};
  auto cuboid_index {9};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockGeometry2D<double> block_geometry {cuboid, cuboid_index};

  CHECK_EQUAL(cuboid_index, block_geometry.rGetGlobalCuboidIndex());
  CHECK_EQUAL(nx, block_geometry.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, block_geometry.GetLatticeExtent()[1]);
  CHECK_CLOSE(x_pos, block_geometry.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, block_geometry.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, block_geometry.GetNx());
  CHECK_EQUAL(ny, block_geometry.GetNy());
  CHECK_CLOSE(delta_R, block_geometry.GetDeltaR(), g_zero_tol);
}

TEST(TestBlockGeometry2D_GetPhysR)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  gsl::index x_index {2};
  gsl::index y_index {3};
  double phys_R[2];
  block_geometry.GetPhysR(x_index, y_index, phys_R);
  CHECK_CLOSE(1.4, phys_R[0], g_loose_tol);
  CHECK_CLOSE(3.7, phys_R[1], g_loose_tol);
}

TEST(TestBlockGeometry2D_GetPhysR_Array)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  gsl::index lattice_R[2] {2, 3};
  double phys_R[2];
  dynamic_cast<AbstractBlockGeometryInterface2D<double>*>(
      &block_geometry)->GetPhysR(lattice_R, phys_R);
  CHECK_CLOSE(1.4, phys_R[0], g_loose_tol);
  CHECK_CLOSE(3.7, phys_R[1], g_loose_tol);
}

TEST(TestBlockGeometry2D_GetMaterial)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  CHECK_EQUAL(0, block_geometry.GetMaterial(-1, 0));
  CHECK_EQUAL(0, block_geometry.GetMaterial(nx, 0));
  CHECK_EQUAL(0, block_geometry.GetMaterial(0, -1));
  CHECK_EQUAL(0, block_geometry.GetMaterial(0, ny));
  // make sure material number are defaulted to zero
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      CHECK_EQUAL(0, block_geometry.GetMaterial(x, y));
    }
  }
  // set material numbers
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetData(x, y) = x + y;
    }
  }
  // Check we have set the material numbers
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      CHECK_EQUAL(x + y, block_geometry.GetMaterial(x, y));
    }
  }
}

TEST(TestBlockGeometry2D_rGetMaterial)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  CHECK_EQUAL(0, block_geometry.GetMaterial(-1, 0));
  CHECK_EQUAL(0, block_geometry.GetMaterial(nx, 0));
  CHECK_EQUAL(0, block_geometry.GetMaterial(0, -1));
  CHECK_EQUAL(0, block_geometry.GetMaterial(0, ny));
  // make sure material number are defaulted to zero
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      std::vector<gsl::index> lattice_R {x, y};
      CHECK_EQUAL(0, block_geometry.rGetMaterial(x, y));
      CHECK_EQUAL(0, dynamic_cast<AbstractBlockGeometryInterface2D<double>&>(
          block_geometry).rGetMaterial(lattice_R));
      CHECK_EQUAL(0, std::as_const(dynamic_cast<
          AbstractBlockGeometryInterface2D<double>&>(block_geometry))
              .rGetMaterial(lattice_R));
    }
  }
  // set material numbers
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetData(x, y) = x + y;
    }
  }
  // Check we have set the material numbers
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      std::vector<gsl::index> lattice_R {x, y};
      CHECK_EQUAL(x + y, block_geometry.rGetMaterial(x, y));
      CHECK_EQUAL(x + y,
          dynamic_cast<AbstractBlockGeometryInterface2D<double>&>(
              block_geometry).rGetMaterial(lattice_R));
      CHECK_EQUAL(x + y, std::as_const(dynamic_cast<
          AbstractBlockGeometryInterface2D<double>&>(block_geometry))
              .rGetMaterial(lattice_R));
    }
  }
}

TEST(TestBlockGeometry2D_rGetMaterial_Set)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  CHECK_EQUAL(0, block_geometry.GetMaterial(-1, 0));
  CHECK_EQUAL(0, block_geometry.GetMaterial(nx, 0));
  CHECK_EQUAL(0, block_geometry.GetMaterial(0, -1));
  CHECK_EQUAL(0, block_geometry.GetMaterial(0, ny));
  // make sure material number are defaulted to zero
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      CHECK_EQUAL(0, block_geometry.rGetMaterial(x, y));
    }
  }
  // set material numbers
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetMaterial(x, y) = x + y;
    }
  }
  // Check we have set the material numbers
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      CHECK_EQUAL(x + y, block_geometry.rGetMaterial(x, y));
    }
  }
}

TEST(TestBlockGeometry2D_AddToStatisticsList)
{
  TestBlockGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  bool need_update_1 {true};
  bool need_update_2 {false};
  bool need_update_3 {true};
  block_geometry.AddToStatisticsList(&need_update_1);
  block_geometry.AddToStatisticsList(&need_update_2);
  block_geometry.AddToStatisticsList(&need_update_3);

  auto need_update_list {tester.GetStatisticsList(block_geometry)};
  CHECK_EQUAL(4u, need_update_list.size());
  auto it {need_update_list.begin()};
  // Added in constructor
  CHECK(**it);
  ++it;
  // need_update_ 1 to 3
  CHECK(**it);
  ++it;
  CHECK(!(**it));
  ++it;
  CHECK(**it);
}

TEST(TestBlockGeometry2D_RemoveFromStatisticsList)
{
  TestBlockGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  bool need_update_1 {true};
  bool need_update_2 {false};
  bool need_update_3 {true};
  block_geometry.AddToStatisticsList(&need_update_1);
  block_geometry.AddToStatisticsList(&need_update_2);
  block_geometry.AddToStatisticsList(&need_update_3);
  block_geometry.RemoveFromStatisticsList(&need_update_1);

  auto need_update_list {tester.GetStatisticsList(block_geometry)};
  CHECK_EQUAL(3u, need_update_list.size());
  auto it {need_update_list.begin()};
  // Added in constructor
  CHECK(**it);
  ++it;
  // need_update_ 2 to 3
  CHECK(!(**it));
  ++it;
  CHECK(**it);
}

TEST(TestBlockGeometry2D_ResetStatistics)
{
  TestBlockGeometry2D tester;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  bool need_update_1 {true};
  bool need_update_2 {false};
  bool need_update_3 {true};
  block_geometry.AddToStatisticsList(&need_update_1);
  block_geometry.AddToStatisticsList(&need_update_2);
  block_geometry.AddToStatisticsList(&need_update_3);
  tester.ResetStatistics(block_geometry);

  auto need_update_list {tester.GetStatisticsList(block_geometry)};
  CHECK_EQUAL(4u, need_update_list.size());
  auto it {need_update_list.begin()};
  // Added in constructor
  CHECK(**it);
  ++it;
  // need_update_ 2 to 3
  CHECK(**it);
  ++it;
  CHECK(**it);
}

TEST(TestBlockGeometry2D_Clean)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  // +-------+  + -> y
  // |   0   |  | +
  // +---+---+  v
  // | 1 | 2 |  x
  // +---+---+
  for (gsl::index x {0}; x < nx / 2; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetMaterial(x, y) = 0;
    }
  }
  for (gsl::index x {nx / 2}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny / 2; ++y) {
      block_geometry.rGetMaterial(x, y) = 1;
      block_geometry.rGetMaterial(x, y + ny / 2) = 2;
    }
  }
  CHECK_EQUAL(12, block_geometry.Clean(false));
  for (gsl::index x {0}; x < nx / 2; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      CHECK_EQUAL(0, block_geometry.rGetMaterial(x, y));
    }
  }
  for (gsl::index x {nx / 2}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny / 2; ++y) {
      CHECK_EQUAL(1, block_geometry.rGetMaterial(x, y));
      if (y > 0) {
        CHECK_EQUAL(0, block_geometry.rGetMaterial(x, y + ny / 2));
      }
      else {
        CHECK_EQUAL(2, block_geometry.rGetMaterial(x, y + ny / 2));
      }
    }
  }
}

TEST(TestBlockGeometry2D_OuterClean)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  // +-------+  + -> y
  // |   0   |  | +
  // +---+---+  v
  // | 1 | 2 |  x
  // +---+---+
  for (gsl::index x {0}; x < nx / 2; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetMaterial(x, y) = 0;
    }
  }
  for (gsl::index x {nx / 2}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny / 2; ++y) {
      block_geometry.rGetMaterial(x, y) = 1;
      block_geometry.rGetMaterial(x, y + ny / 2) = 2;
    }
  }
  CHECK_EQUAL(16, block_geometry.OuterClean(false));
  for (gsl::index x {0}; x < nx / 2; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      CHECK_EQUAL(0, block_geometry.rGetMaterial(x, y));
    }
  }
  for (gsl::index x {nx / 2}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny / 2; ++y) {
      CHECK_EQUAL(0, block_geometry.rGetMaterial(x, y));
      CHECK_EQUAL(2, block_geometry.rGetMaterial(x, y + ny / 2));
    }
  }
}

TEST(TestBlockGeometry2D_InnerClean)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetMaterial(x, y) = 1;
    }
  }
  for (gsl::index y {1}; y < ny - 1; ++y) {
    block_geometry.rGetMaterial(nx / 2, y) = 2;
  }
  for (gsl::index x {1}; x < nx - 1; ++x) {
    block_geometry.rGetMaterial(x, ny / 2) = 2;
  }
  block_geometry.InnerClean(false);
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      if ((x == 3 && y == 4) || (x == 3 && y == 5) || (x == 4 && y == 4)) {
        CHECK_EQUAL(2, block_geometry.rGetMaterial(x, y));
      }
      else {
        CHECK_EQUAL(1, block_geometry.rGetMaterial(x, y));
      }
    }
  }
}

TEST(TestBlockGeometry2D_InnerClean_FromMaterial)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetMaterial(x, y) = 1;
    }
  }
  for (gsl::index y {1}; y < ny - 1; ++y) {
    block_geometry.rGetMaterial(nx / 2, y) = 3;
  }
  for (gsl::index x {1}; x < nx - 1; ++x) {
    block_geometry.rGetMaterial(x, ny / 2) = 2;
  }
  block_geometry.InnerClean(3, false);
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      if (x > 0 && x < nx - 1 && y == 4) {
        CHECK_EQUAL(2, block_geometry.rGetMaterial(x, y));
      }
      else if (x == 3 && y == 5) {
        CHECK_EQUAL(3, block_geometry.rGetMaterial(x, y));
      }
      else {
        CHECK_EQUAL(1, block_geometry.rGetMaterial(x, y));
      }
    }
  }
}

TEST(TestBlockGeometry2D_FindStreamDirections)
{
  typedef descriptor::D2Q9Descriptor<double> D;
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetMaterial(x, y) = 1;
    }
  }
  for (gsl::index y {1}; y < ny - 1; ++y) {
    block_geometry.rGetMaterial(nx / 2, y) = 3;
  }
  for (gsl::index x {1}; x < nx - 1; ++x) {
    block_geometry.rGetMaterial(x, ny / 2) = 2;
  }
  std::list bulk_material {1};
  std::array<bool, D::sQ> directions;
  block_geometry.FindStreamDirections<double, descriptor::D2Q9Descriptor>(
      /*xIndex=*/2, /*yIndex=*/3, /*material=*/1, bulk_material, directions);
  for (gsl::index q {0}; q < D::sQ; ++q) {
    if (q == 2 || q == 3 || q == 4) {
      CHECK(directions[q]);
    }
    else {
      CHECK(!directions[q]);
    }
  }
  block_geometry.FindStreamDirections<double, descriptor::D2Q9Descriptor>(
      /*xIndex=*/2, /*yIndex=*/5, /*material=*/1, bulk_material, directions);
  for (gsl::index q {0}; q < D::sQ; ++q) {
    if (q == 1 || q == 2 || q == 8) {
      CHECK(directions[q]);
    }
    else {
      CHECK(!directions[q]);
    }
  }
  block_geometry.FindStreamDirections<double, descriptor::D2Q9Descriptor>(
      /*xIndex=*/4, /*yIndex=*/3, /*material=*/1, bulk_material, directions);
  for (gsl::index q {0}; q < D::sQ; ++q) {
    if (q == 4 || q == 5 || q == 6) {
      CHECK(directions[q]);
    }
    else {
      CHECK(!directions[q]);
    }
  }
  block_geometry.FindStreamDirections<double, descriptor::D2Q9Descriptor>(
      /*xIndex=*/4, /*yIndex=*/5, /*material=*/1, bulk_material, directions);
  for (gsl::index q {0}; q < D::sQ; ++q) {
    if (q == 6 || q == 7 || q == 8) {
      CHECK(directions[q]);
    }
    else {
      CHECK(!directions[q]);
    }
  }
  // Set 2 as fluid material
  std::list bulk_material_2 {1, 2};
  block_geometry.FindStreamDirections<double, descriptor::D2Q9Descriptor>(
      /*xIndex=*/2, /*yIndex=*/3, /*material=*/1, bulk_material_2,
      directions);
  for (gsl::index q {0}; q < D::sQ; ++q) {
    if (q == 1 || q == 2 || q == 3 || q == 4 || q == 7 || q == 8) {
      CHECK(directions[q]);
    }
    else {
      CHECK(!directions[q]);
    }
  }
}

TEST(TestBlockGeometry2D_CheckForErrors)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  BlockGeometry2D<double> block_geometry {x_pos, y_pos, delta_R, nx, ny};

  // +-------+  + -> y
  // |   0   |  | +
  // +---+---+  v
  // | 1 | 2 |  x
  // +---+---+
  for (gsl::index x {0}; x < nx / 2; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      block_geometry.rGetMaterial(x, y) = 0;
    }
  }
  for (gsl::index x {nx / 2}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny / 2; ++y) {
      block_geometry.rGetMaterial(x, y) = 1;
      block_geometry.rGetMaterial(x, y + ny / 2) = 2;
    }
  }
  CHECK(block_geometry.CheckForErrors(false));
  block_geometry.OuterClean(false);
  CHECK(!block_geometry.CheckForErrors(false));
  for (gsl::index x {0}; x < nx / 2; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      CHECK_EQUAL(0, block_geometry.rGetMaterial(x, y));
    }
  }
  for (gsl::index x {nx / 2}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny / 2; ++y) {
      CHECK_EQUAL(0, block_geometry.rGetMaterial(x, y));
      CHECK_EQUAL(2, block_geometry.rGetMaterial(x, y + ny / 2));
    }
  }
}
}
}  // namespace iblbm
