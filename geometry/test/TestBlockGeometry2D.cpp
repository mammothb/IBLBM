#include <iostream>

#include "UnitTest++/UnitTest++.h"
#include "BlockGeometry2D.hpp"

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

  CHECK_EQUAL(-1, block_geometry.rGetGlobalCuboidIndex());
  CHECK_EQUAL(nx, block_geometry.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, block_geometry.GetLatticeExtent()[1]);
  CHECK_CLOSE(x_pos, block_geometry.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, block_geometry.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, block_geometry.GetNx());
  CHECK_EQUAL(ny, block_geometry.GetNy());
  CHECK_CLOSE(delta_R, block_geometry.GetDeltaR(), g_zero_tol);
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
}
}  // namespace iblbm
