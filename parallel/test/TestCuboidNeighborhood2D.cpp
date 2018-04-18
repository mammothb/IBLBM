#include "UnitTest++/UnitTest++.h"
#include "CuboidGeometry2D.hpp"
#include "CuboidNeighborhood2D.hpp"
#include "HeuristicLoadBalancer.hpp"
#include "IndicatorFunctor2D.hpp"
#include "SuperGeometry2D.hpp"
#include "UnitTestCustomUtilities.hpp"

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
  CHECK(neighborhood.pGetInData() == nullptr);
  CHECK(neighborhood.pGetOutData() == nullptr);
  CHECK(tester.GetHasInitializedInNeighbor(neighborhood) == false);
  CHECK(tester.GetHasInitializedOutNeighbor(neighborhood) == false);
}

TEST(TestCuboidNeighborhood2D_AddInCell_Cell)
{
  auto delta_R {0.5};
  Vector2D<double> origin {1.2, 3.4};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid(extent, origin);
  CuboidGeometry2D<double> cuboid_geometry(indicator_cuboid, delta_R, nc);
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D super_geometry(cuboid_geometry, load_balancer);

  gsl::index global_cuboid_index {3};
  CuboidNeighborhood2D neighborhood(super_geometry, global_cuboid_index);

  std::vector<Cell2D<double>> exp_cell;
  for (gsl::index i {0}; i < 3; ++i) {
    Vector2D<double> phys_R {1.2 + i, 3.4 + i};
    exp_cell.push_back(Cell2D<double>{global_cuboid_index, 2 + i, 4 + i,
        phys_R});
    neighborhood.AddInCell(exp_cell.back());
  }
  CHECK_EQUAL(exp_cell.size(), neighborhood.GetInCellsSize());
  for (gsl::index i {0}; i < exp_cell.size(); ++i) {
    CHECK_EQUAL(exp_cell[i].mGlobalCuboidIndex,
        neighborhood.rGetInCell(i).mGlobalCuboidIndex);
    CHECK(testutil::CheckEqualVector(neighborhood.rGetInCell(i).mLatticeR,
        exp_cell[i].mLatticeR));
    CHECK(testutil::CheckCloseVector(neighborhood.rGetInCell(i).mPhysR,
        exp_cell[i].mPhysR, g_zero_tol));
  }
}

TEST(TestCuboidNeighborhood2D_AddInCell_Index)
{
  auto delta_R {0.5};
  Vector2D<double> origin {1.2, 3.4};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid(extent, origin);
  CuboidGeometry2D<double> cuboid_geometry(indicator_cuboid, delta_R, nc);
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D super_geometry(cuboid_geometry, load_balancer);

  // No repeat and within bounds
  {
    gsl::index global_cuboid_index {2};
    CuboidNeighborhood2D neighborhood(super_geometry, global_cuboid_index);

    std::vector<Cell2D<double>> exp_cell;
    for (gsl::index i {0}; i < 6; ++i) {
      auto x {i % 6};
      auto y {i % 4};

      auto phys_R {cuboid_geometry.GetPhysR(global_cuboid_index, x, y)};
      if (cuboid_geometry.HasCuboid(phys_R, global_cuboid_index)) {
        exp_cell.push_back(Cell2D<double>{global_cuboid_index, x, y,
            phys_R});
      }
      neighborhood.AddInCell(x, y);
    }
    CHECK_EQUAL(exp_cell.size(), neighborhood.GetInCellsSize());
    for (gsl::index i {0}; i < exp_cell.size(); ++i) {
      CHECK_EQUAL(exp_cell[i].mGlobalCuboidIndex,
          neighborhood.rGetInCell(i).mGlobalCuboidIndex);
      CHECK(testutil::CheckEqualVector(neighborhood.rGetInCell(i).mLatticeR,
          exp_cell[i].mLatticeR));
      CHECK(testutil::CheckCloseVector(neighborhood.rGetInCell(i).mPhysR,
          exp_cell[i].mPhysR, g_zero_tol));
    }
  }

  // With repeat and within bounds
  {
    gsl::index global_cuboid_index {2};
    CuboidNeighborhood2D neighborhood(super_geometry, global_cuboid_index);

    std::vector<Cell2D<double>> exp_cell;
    for (gsl::index i {0}; i < 6; ++i) {
      auto x {i % 6};
      auto y {i % 4};

      auto phys_R {cuboid_geometry.GetPhysR(global_cuboid_index, x, y)};
      if (cuboid_geometry.HasCuboid(phys_R, global_cuboid_index)) {
        exp_cell.push_back(Cell2D<double>{global_cuboid_index, x, y,
            phys_R});
      }
      neighborhood.AddInCell(x, y);
    }
    // Try to add the exact same cells again
    for (gsl::index i {0}; i < 6; ++i) {
      auto x {i % 6};
      auto y {i % 4};
      neighborhood.AddInCell(x, y);
    }
    CHECK_EQUAL(exp_cell.size(), neighborhood.GetInCellsSize());
    for (gsl::index i {0}; i < exp_cell.size(); ++i) {
      CHECK_EQUAL(exp_cell[i].mGlobalCuboidIndex,
          neighborhood.rGetInCell(i).mGlobalCuboidIndex);
      CHECK(testutil::CheckEqualVector(neighborhood.rGetInCell(i).mLatticeR,
          exp_cell[i].mLatticeR));
      CHECK(testutil::CheckCloseVector(neighborhood.rGetInCell(i).mPhysR,
          exp_cell[i].mPhysR, g_zero_tol));
    }
  }

  // No repeat and with out of bounds
  {
    gsl::index global_cuboid_index {2};
    CuboidNeighborhood2D neighborhood(super_geometry, global_cuboid_index);

    std::vector<Cell2D<double>> exp_cell;
    for (gsl::index i {0}; i < 20; ++i) {
      auto x {i};
      auto y {i};

      auto tmp_index {global_cuboid_index};
      auto phys_R {cuboid_geometry.GetPhysR(tmp_index, x, y)};
      if (cuboid_geometry.HasCuboid(phys_R, tmp_index)) {
        exp_cell.push_back(Cell2D<double>{tmp_index, x, y, phys_R});
      }
      neighborhood.AddInCell(x, y);
    }
    CHECK_EQUAL(exp_cell.size(), neighborhood.GetInCellsSize());
    for (gsl::index i {0}; i < exp_cell.size(); ++i) {
      CHECK_EQUAL(exp_cell[i].mGlobalCuboidIndex,
          neighborhood.rGetInCell(i).mGlobalCuboidIndex);
      CHECK(testutil::CheckEqualVector(neighborhood.rGetInCell(i).mLatticeR,
          exp_cell[i].mLatticeR));
      CHECK(testutil::CheckCloseVector(neighborhood.rGetInCell(i).mPhysR,
          exp_cell[i].mPhysR, g_zero_tol));
    }
  }
  // No repeats and with out of bounds
  {
    gsl::index global_cuboid_index {2};
    CuboidNeighborhood2D neighborhood(super_geometry, global_cuboid_index);

    std::vector<Cell2D<double>> exp_cell;
    for (gsl::index i {0}; i < 20; ++i) {
      auto x {i};
      auto y {i};

      auto tmp_index {global_cuboid_index};
      auto phys_R {cuboid_geometry.GetPhysR(tmp_index, x, y)};
      if (cuboid_geometry.HasCuboid(phys_R, tmp_index)) {
        exp_cell.push_back(Cell2D<double>{tmp_index, x, y, phys_R});
      }
      neighborhood.AddInCell(x, y);
    }
    // Add again
    for (gsl::index i {0}; i < 20; ++i) {
      auto x {i};
      auto y {i};
      neighborhood.AddInCell(x, y);
    }
    CHECK_EQUAL(exp_cell.size(), neighborhood.GetInCellsSize());
    for (gsl::index i {0}; i < exp_cell.size(); ++i) {
      CHECK_EQUAL(exp_cell[i].mGlobalCuboidIndex,
          neighborhood.rGetInCell(i).mGlobalCuboidIndex);
      CHECK(testutil::CheckEqualVector(neighborhood.rGetInCell(i).mLatticeR,
          exp_cell[i].mLatticeR));
      CHECK(testutil::CheckCloseVector(neighborhood.rGetInCell(i).mPhysR,
          exp_cell[i].mPhysR, g_zero_tol));
    }
  }
}

TEST(TestCuboidNeighborhood2D_AddInCell_Overlap)
{
  auto delta_R {0.5};
  Vector2D<double> origin {1.2, 3.4};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid(extent, origin);
  CuboidGeometry2D<double> cuboid_geometry(indicator_cuboid, delta_R, nc);
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D super_geometry(cuboid_geometry, load_balancer);

  // No repeat and within bounds
  {
    gsl::index global_cuboid_index {2};
    CuboidNeighborhood2D neighborhood(super_geometry, global_cuboid_index);

    std::vector<Cell2D<double>> exp_cell;
    auto nx {cuboid_geometry.rGetCuboid(global_cuboid_index).GetNx()};
    auto ny {cuboid_geometry.rGetCuboid(global_cuboid_index).GetNy()};
    auto overlap {2u};

    for (gsl::index x {0}; x < nx + 2 * overlap; ++x) {
      for (gsl::index y {0}; y < ny + 2 * overlap; ++y) {
        if (x < overlap || x > nx + overlap - 1 ||
            y < overlap || y > ny + overlap - 1) {
          auto tmp_index {global_cuboid_index};
          auto tmp_x {x - overlap};
          auto tmp_y {y - overlap};
          auto phys_R {cuboid_geometry.GetPhysR(tmp_index, tmp_x, tmp_y)};
          if (cuboid_geometry.HasCuboid(phys_R, tmp_index)) {
            exp_cell.push_back(Cell2D<double>{tmp_index, tmp_x, tmp_y,
                phys_R});
          }
        }
      }
    }
    neighborhood.AddInCells(overlap);

    CHECK_EQUAL(exp_cell.size(), neighborhood.GetInCellsSize());
    for (gsl::index i {0}; i < exp_cell.size(); ++i) {
      CHECK_EQUAL(exp_cell[i].mGlobalCuboidIndex,
          neighborhood.rGetInCell(i).mGlobalCuboidIndex);
      CHECK(testutil::CheckEqualVector(neighborhood.rGetInCell(i).mLatticeR,
          exp_cell[i].mLatticeR));
      CHECK(testutil::CheckCloseVector(neighborhood.rGetInCell(i).mPhysR,
          exp_cell[i].mPhysR, g_zero_tol));
    }
  }

  // With repeat and within bounds
  {
    gsl::index global_cuboid_index {2};
    CuboidNeighborhood2D neighborhood(super_geometry, global_cuboid_index);

    std::vector<Cell2D<double>> exp_cell;
    auto nx {cuboid_geometry.rGetCuboid(global_cuboid_index).GetNx()};
    auto ny {cuboid_geometry.rGetCuboid(global_cuboid_index).GetNy()};
    auto overlap {2u};

    for (gsl::index x {0}; x < nx + 2 * overlap; ++x) {
      for (gsl::index y {0}; y < ny + 2 * overlap; ++y) {
        if (x < overlap || x > nx + overlap - 1 ||
            y < overlap || y > ny + overlap - 1) {
          auto tmp_index {global_cuboid_index};
          auto tmp_x {x - overlap};
          auto tmp_y {y - overlap};
          auto phys_R {cuboid_geometry.GetPhysR(tmp_index, tmp_x, tmp_y)};
          if (cuboid_geometry.HasCuboid(phys_R, tmp_index)) {
            exp_cell.push_back(Cell2D<double>{tmp_index, tmp_x, tmp_y,
                phys_R});
          }
        }
      }
    }
    neighborhood.AddInCells(overlap);
    neighborhood.AddInCells(overlap);

    CHECK_EQUAL(exp_cell.size(), neighborhood.GetInCellsSize());
    for (gsl::index i {0}; i < exp_cell.size(); ++i) {
      CHECK_EQUAL(exp_cell[i].mGlobalCuboidIndex,
          neighborhood.rGetInCell(i).mGlobalCuboidIndex);
      CHECK(testutil::CheckEqualVector(neighborhood.rGetInCell(i).mLatticeR,
          exp_cell[i].mLatticeR));
      CHECK(testutil::CheckCloseVector(neighborhood.rGetInCell(i).mPhysR,
          exp_cell[i].mPhysR, g_zero_tol));
    }
  }
}

TEST(TestCuboidNeighborhood2D_InitializeInNeighbor)
{
  ;
}
}
}  // namespace iblbm
