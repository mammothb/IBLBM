#include "UnitTest++/UnitTest++.h"
#include "CuboidGeometry2D.hpp"
#include "CuboidNeighborhood2D.hpp"
#include "HeuristicLoadBalancer.hpp"
#include "IndicatorFunctor2D.hpp"
#include "MpiManager.hpp"
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

  std::vector<Cell2D<double>> GetExCells(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mExCells;
  }

  std::vector<gsl::index> GetExNbrCuboids(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mExNbrCuboids;
  }

  std::vector<std::size_t> GetExNbrNumCells(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mExNbrNumCells;
  }

  std::vector<std::size_t> GetInNbrNumCells(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mInNbrNumCells;
  }

  bool** pGetInData(CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mpInData;
  }

  double** pGetInDataCoords(CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mpInDataCoords;
  }

  std::size_t* pGetTmpInNbrNumCells(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mpTmpInNbrNumCells;
  }

  bool GetHasInitializedInNeighbor(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mHasInitializedInNeighbor;
  }

  bool GetHasInitializedExNeighbor(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mHasInitializedExNeighbor;
  }
};

SUITE(TestParallel)
{
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestCell2D_Constructor_Empty)
{
  Cell2D<double> cell;
  CHECK_EQUAL(-1, cell.mGlobalCuboidIndex);
  CHECK_EQUAL(2, cell.mLatticeR.size());
  CHECK(testutil::CheckEqualVector(Vector2D<gsl::index>{0, 0},
      cell.mLatticeR));
  CHECK_EQUAL(2, cell.mPhysR.size());
  CHECK(testutil::CheckCloseVector(Vector2D<double>{0, 0}, cell.mPhysR,
      g_zero_tol));
}

TEST(TestCell2D_Constructor)
{
  gsl::index global_cuboid_index {2};
  gsl::index x_index {3};
  gsl::index y_index {4};
  Vector2D<double> phys_R {5.6, 7.8};
  Cell2D<double> cell {global_cuboid_index, x_index, y_index, phys_R};

  CHECK_EQUAL(global_cuboid_index, cell.mGlobalCuboidIndex);
  CHECK_EQUAL(2, cell.mLatticeR.size());
  CHECK(testutil::CheckEqualVector(Vector2D<gsl::index>{x_index, y_index},
      cell.mLatticeR));
  CHECK_EQUAL(2, cell.mPhysR.size());
  CHECK(testutil::CheckCloseVector(phys_R, cell.mPhysR, g_zero_tol));
}

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

  SuperGeometry2D<double> super_geometry(cuboid_geometry, load_balancer);
  CuboidNeighborhood2D neighborhood(super_geometry, 3);

  CHECK_EQUAL(3, tester.GetGlobalCuboidIndex(neighborhood));
  CHECK_EQUAL(nc, tester.GetNumCuboid(neighborhood));
  CHECK_CLOSE(delta_R, tester.GetDeltaR(neighborhood), g_zero_tol);
  CHECK_EQUAL(1, tester.GetNumData(neighborhood));
  CHECK_EQUAL(sizeof(std::size_t), tester.GetSizeofDataType(neighborhood));
  CHECK(neighborhood.pGetInData() == nullptr);
  CHECK(neighborhood.pGetExData() == nullptr);
  CHECK(tester.GetHasInitializedInNeighbor(neighborhood) == false);
  CHECK(tester.GetHasInitializedExNeighbor(neighborhood) == false);
}

TEST(TestCuboidNeighborhood2D_CopyConstructor)
{
  TestCuboidNeighborhood2D tester;

  auto delta_R {0.5};
  Vector2D<double> origin {1.2, 3.4};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid(extent, origin);
  CuboidGeometry2D<double> cuboid_geometry(indicator_cuboid, delta_R, nc);
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer};
  CuboidNeighborhood2D neighborhood {super_geometry, 3};
  CuboidNeighborhood2D neighborhood_2 {neighborhood};

  CHECK_EQUAL(3, tester.GetGlobalCuboidIndex(neighborhood_2));
  CHECK_EQUAL(nc, tester.GetNumCuboid(neighborhood_2));
  CHECK_CLOSE(delta_R, tester.GetDeltaR(neighborhood_2), g_zero_tol);
  CHECK_EQUAL(1, tester.GetNumData(neighborhood_2));
  CHECK_EQUAL(sizeof(std::size_t), tester.GetSizeofDataType(neighborhood_2));
  CHECK(neighborhood_2.pGetInData() == nullptr);
  CHECK(neighborhood_2.pGetExData() == nullptr);
  CHECK(tester.GetHasInitializedInNeighbor(neighborhood_2) == false);
  CHECK(tester.GetHasInitializedExNeighbor(neighborhood_2) == false);
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

  SuperGeometry2D<double> super_geometry(cuboid_geometry, load_balancer);

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

TEST(TestCuboidNeighborhood2D_AddExCell_Cell)
{
  TestCuboidNeighborhood2D tester;

  auto delta_R {0.5};
  Vector2D<double> origin {1.2, 3.4};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid(extent, origin);
  CuboidGeometry2D<double> cuboid_geometry(indicator_cuboid, delta_R, nc);
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry(cuboid_geometry, load_balancer);

  gsl::index global_cuboid_index {3};
  CuboidNeighborhood2D<double> neighborhood(super_geometry,
      global_cuboid_index);

  std::vector<Cell2D<double>> exp_cell;
  for (gsl::index i {0}; i < 3; ++i) {
    Vector2D<double> phys_R {1.2 + i, 3.4 + i};
    exp_cell.push_back(Cell2D<double>{global_cuboid_index, 2 + i, 4 + i,
        phys_R});
    neighborhood.AddExCell(exp_cell.back());
  }
  CHECK_EQUAL(exp_cell.size(), tester.GetExCells(neighborhood).size());
  for (gsl::index i {0}; i < exp_cell.size(); ++i) {
    CHECK_EQUAL(exp_cell[i].mGlobalCuboidIndex,
        tester.GetExCells(neighborhood)[i].mGlobalCuboidIndex);
    CHECK(testutil::CheckEqualVector(
        tester.GetExCells(neighborhood)[i].mLatticeR,
        exp_cell[i].mLatticeR));
    CHECK(testutil::CheckCloseVector(
        tester.GetExCells(neighborhood)[i].mPhysR,
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

  SuperGeometry2D<double> super_geometry(cuboid_geometry, load_balancer);

  // No repeat and within bounds
  {
    gsl::index global_cuboid_index {2};
    CuboidNeighborhood2D<double> neighborhood(super_geometry,
        global_cuboid_index);

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
    CuboidNeighborhood2D<double> neighborhood(super_geometry,
        global_cuboid_index);

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
    CuboidNeighborhood2D<double> neighborhood(super_geometry,
        global_cuboid_index);

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
    CuboidNeighborhood2D<double> neighborhood(super_geometry,
        global_cuboid_index);

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

  SuperGeometry2D<double> super_geometry(cuboid_geometry, load_balancer);

  // No repeat and within bounds
  {
    gsl::index global_cuboid_index {2};
    CuboidNeighborhood2D<double> neighborhood(super_geometry,
        global_cuboid_index);

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
    CuboidNeighborhood2D<double> neighborhood(super_geometry,
        global_cuboid_index);

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
  TestCuboidNeighborhood2D tester;

  auto delta_R {0.5};
  Vector2D<double> origin {1.2, 3.4};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry(cuboid_geometry, load_balancer);

  // Trying to mimic the call order within SuperGeometry2D, other ways have
  // resulted in deadlock
  std::vector<CuboidNeighborhood2D<double>> neighborhoods;
  std::vector<std::vector<Cell2D<double>>> exp_cell;
  std::vector<std::vector<gsl::index>> exp_nbr_cuboid;
  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    CuboidNeighborhood2D<double> neighborhood {super_geometry, global_idx};
    neighborhoods.push_back(neighborhood);
    exp_cell.push_back(std::vector<Cell2D<double>>());
    exp_nbr_cuboid.push_back(std::vector<gsl::index>());

    if (global_idx < 4) {
      Cell2D<double> cell {global_idx, 0, 0,
          cuboid_geometry.GetPhysR(global_idx, 0, 0)};
      if (cuboid_geometry.HasCuboid(cell.mPhysR, cell.mGlobalCuboidIndex)) {
        neighborhoods[local_idx].AddInCell(cell);
        exp_cell[local_idx].push_back(cell);
        exp_nbr_cuboid[local_idx].push_back(global_idx);
      }
    }
    // Skip global_idx == 4
    if (global_idx > 4) {
      for (gsl::index i {0}; i < 2; ++i) {
        Cell2D<double> cell {global_idx, i, i,
            cuboid_geometry.GetPhysR(global_idx, i, i)};
        if (cuboid_geometry.HasCuboid(cell.mPhysR, cell.mGlobalCuboidIndex)) {
          neighborhoods[local_idx].AddInCell(cell);
          exp_cell[local_idx].push_back(cell);
          exp_nbr_cuboid[local_idx].push_back(global_idx);
        }
      }
    }
  }
  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    neighborhoods[local_idx].InitializeInNeighbor();
    for (gsl::index i {0}; i < neighborhoods[local_idx].GetInCellsSize();
        ++i) {
      auto global_idx {neighborhoods[local_idx].rGetInCell(i)
          .mGlobalCuboidIndex};
      if (MpiManager::Instance().GetRank() ==
          load_balancer.GetRank(global_idx)) {
        Cell2D<double> tmp_cell;
        tmp_cell.mPhysR = neighborhoods[local_idx].rGetInCell(i).mPhysR;
        cuboid_geometry.GetLatticeR(tmp_cell.mPhysR,
            tmp_cell.mGlobalCuboidIndex, tmp_cell.mLatticeR);
        tmp_cell.mGlobalCuboidIndex = load_balancer.GetGlobalIndex(local_idx);
        neighborhoods[load_balancer.GetLocalIndex(global_idx)].AddExCell(
            tmp_cell);
      }
    }
  }
  for (auto& r_it : neighborhoods) r_it.InitializeExNeighbor();
  for (auto& r_it : neighborhoods) r_it.FinishComm();

  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    CHECK_EQUAL(exp_cell[local_idx].size(),
        neighborhoods[local_idx].GetInCellsSize());
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    if (global_idx < 4) {
      CHECK_EQUAL(1, tester.pGetTmpInNbrNumCells(
          neighborhoods[local_idx])[global_idx]);
    }
    else if (global_idx > 4) {
      CHECK_EQUAL(2, tester.pGetTmpInNbrNumCells(
          neighborhoods[local_idx])[global_idx]);
    }
    else {
      CHECK_EQUAL(0, tester.pGetTmpInNbrNumCells(
          neighborhoods[local_idx])[global_idx]);
    }
    // In parallel, the InCells sent to each neighborhood are split
    for (gsl::index i {0};
        i < tester.GetInNbrNumCells(neighborhoods[local_idx]).size(); ++i) {
      CHECK_EQUAL(exp_nbr_cuboid[local_idx][i],
          neighborhoods[local_idx].rGetInNeighborCuboid(i));
      if (global_idx < 4) {
        CHECK_EQUAL(1, tester.GetInNbrNumCells(neighborhoods[local_idx])[i]);
      }
      else if (global_idx > 4) {
        CHECK_EQUAL(2, tester.GetInNbrNumCells(neighborhoods[local_idx])[i]);
      }
      else {
        CHECK_EQUAL(0, tester.GetInNbrNumCells(neighborhoods[local_idx])[i]);
      }
    }
#ifdef IBLBM_PARALLEL_MPI
    if (global_idx == 4) {
      CHECK(tester.pGetInData(neighborhoods[local_idx])[global_idx] ==
          nullptr);
      CHECK(tester.pGetInDataCoords(neighborhoods[local_idx])[global_idx] ==
          nullptr);
    }
    else {
      CHECK(tester.pGetInData(neighborhoods[local_idx])[global_idx] !=
          nullptr);
      CHECK(tester.pGetInDataCoords(neighborhoods[local_idx])[global_idx] !=
          nullptr);
    }
#endif  // IBLBM_PARALLEL_MPI
  }
}

TEST(TestCuboidNeighborhood2D_InitializeExNeighbor)
{
  TestCuboidNeighborhood2D tester;

  auto delta_R {0.5};
  Vector2D<double> origin {1.2, 3.4};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry(cuboid_geometry, load_balancer);

  // Trying to mimic the call order within SuperGeometry2D, other ways have
  // resulted in deadlock
  std::vector<CuboidNeighborhood2D<double>> neighborhoods;
  std::vector<std::vector<Cell2D<double>>> exp_cell;
  std::vector<std::vector<gsl::index>> exp_nbr_cuboid;
  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    CuboidNeighborhood2D<double> neighborhood {super_geometry, global_idx};
    neighborhoods.push_back(neighborhood);
    exp_cell.push_back(std::vector<Cell2D<double>>());
    exp_nbr_cuboid.push_back(std::vector<gsl::index>());

    if (global_idx < 4) {
      Cell2D<double> cell {global_idx, 0, 0,
          cuboid_geometry.GetPhysR(global_idx, 0, 0)};
      if (cuboid_geometry.HasCuboid(cell.mPhysR, cell.mGlobalCuboidIndex)) {
        neighborhoods[local_idx].AddInCell(cell);
        exp_cell[local_idx].push_back(cell);
        exp_nbr_cuboid[local_idx].push_back(global_idx);
      }
    }
    // Skip global_idx == 4
    if (global_idx > 4) {
      for (gsl::index i {0}; i < 2; ++i) {
        Cell2D<double> cell {global_idx, i, i,
            cuboid_geometry.GetPhysR(global_idx, i, i)};
        if (cuboid_geometry.HasCuboid(cell.mPhysR, cell.mGlobalCuboidIndex)) {
          neighborhoods[local_idx].AddInCell(cell);
          exp_cell[local_idx].push_back(cell);
          exp_nbr_cuboid[local_idx].push_back(global_idx);
        }
      }
    }
  }
  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    neighborhoods[local_idx].InitializeInNeighbor();
    for (gsl::index i {0}; i < neighborhoods[local_idx].GetInCellsSize();
        ++i) {
      auto global_idx {neighborhoods[local_idx].rGetInCell(i)
          .mGlobalCuboidIndex};
      if (MpiManager::Instance().GetRank() ==
          load_balancer.GetRank(global_idx)) {
        Cell2D<double> tmp_cell;
        tmp_cell.mPhysR = neighborhoods[local_idx].rGetInCell(i).mPhysR;
        cuboid_geometry.GetLatticeR(tmp_cell.mPhysR,
            tmp_cell.mGlobalCuboidIndex, tmp_cell.mLatticeR);
        tmp_cell.mGlobalCuboidIndex = load_balancer.GetGlobalIndex(local_idx);
        neighborhoods[load_balancer.GetLocalIndex(global_idx)].AddExCell(
            tmp_cell);
      }
    }
  }
  for (auto& r_it : neighborhoods) r_it.InitializeExNeighbor();
  for (auto& r_it : neighborhoods) r_it.FinishComm();

  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    CHECK_EQUAL(exp_cell[local_idx].size(), tester.GetExCells(
        neighborhoods[local_idx]).size());
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    for (gsl::index i {0};
        i < tester.GetExNbrCuboids(neighborhoods[local_idx]).size(); ++i) {
      CHECK_EQUAL(exp_nbr_cuboid[local_idx][i],
          tester.GetExNbrCuboids(neighborhoods[local_idx])[i]);
      if (global_idx < 4) {
        CHECK_EQUAL(1, tester.GetExNbrNumCells(neighborhoods[local_idx])[i]);
      }
      else if (global_idx > 4) {
        CHECK_EQUAL(2, tester.GetExNbrNumCells(neighborhoods[local_idx])[i]);
      }
      else {
        CHECK_EQUAL(0, tester.GetExNbrNumCells(neighborhoods[local_idx])[i]);
      }
    }
    CHECK(tester.GetHasInitializedExNeighbor(neighborhoods[local_idx]));
  }
}
}
}  // namespace iblbm
