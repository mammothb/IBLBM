#include "UnitTest++/UnitTest++.h"
#include "Communicator2D.hpp"
#include "CuboidGeometry2D.hpp"
#include "HeuristicLoadBalancer.hpp"
#include "IndicatorFunctor2D.hpp"
#include "SuperGeometry2D.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
class TestCommunicator2D
{
 public:
  std::size_t GetNumCuboid(Communicator2D<double>& rCommunicator)
  {
    return rCommunicator.mNumCuboid;
  }

  bool GetIsInitialized(Communicator2D<double>& rCommunicator)
  {
    return rCommunicator.mIsInitialized;
  }

  std::vector<CuboidNeighborhood2D<double>> GetNeighborhood(
      Communicator2D<double>& rCommunicator)
  {
    return rCommunicator.mNeighborhood;
  }

  std::vector<CuboidNeighborhood2D<double>>& rGetNeighborhood(
      Communicator2D<double>& rCommunicator)
  {
    return rCommunicator.mNeighborhood;
  }
};

class TestCuboidNeighborhood2D
{
 public:
  gsl::index GetGlobalCuboidIndex(CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mGlobalCuboidIndex;
  }

  std::vector<Cell2D<double>> GetInCells(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mInCells;
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

  std::size_t* pGetTmpInNbrNumCells(
      CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mpTmpInNbrNumCells;
  }

  double** pGetInDataCoords(CuboidNeighborhood2D<double>& rNeighborhood)
  {
    return rNeighborhood.mpInDataCoords;
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

TEST(TestCommunicator2D_Constructor)
{
  TestCommunicator2D tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer};
  Communicator2D<double> communicator {super_geometry};

  CHECK_EQUAL(0, tester.GetNumCuboid(communicator));
  CHECK(!tester.GetIsInitialized(communicator));
}

TEST(TestCommunicator2D_InitializeNeighborhood)
{
  TestCommunicator2D tester;
  TestCuboidNeighborhood2D nbr_tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer};
  Communicator2D<double> communicator {super_geometry};

  communicator.InitializeNeighborhood();

  CHECK_EQUAL(nc, tester.GetNumCuboid(communicator));
  CHECK_EQUAL(load_balancer.GetSize(),
      tester.GetNeighborhood(communicator).size());
  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    CHECK_EQUAL(load_balancer.GetGlobalIndex(local_idx),
        nbr_tester.GetGlobalCuboidIndex(
            tester.GetNeighborhood(communicator)[local_idx]));
  }
}

TEST(TestCommunicator2D_AddCell_Index)
{
  TestCommunicator2D tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer};
  Communicator2D<double> communicator {super_geometry};

  communicator.InitializeNeighborhood();

  for (gsl::index i {0}; i < 2; ++i) {
    gsl::index x_idx {i};
    gsl::index y_idx {i + 1};
    for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
        ++local_idx) {
      communicator.AddCell(local_idx, x_idx, y_idx);

      auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
      Cell2D<double> cell {global_idx, x_idx, y_idx,
          cuboid_geometry.GetPhysR(global_idx, x_idx, y_idx)};
      if (cuboid_geometry.HasCuboid(cell.mPhysR, cell.mGlobalCuboidIndex)) {
        CHECK_EQUAL(global_idx,
            tester.GetNeighborhood(communicator)[local_idx].rGetInCell(i)
                .mGlobalCuboidIndex);
      }
    }
  }
}

TEST(TestCommunicator2D_AddCell_Overlap)
{
  TestCommunicator2D tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer};
  Communicator2D<double> communicator {super_geometry};

  communicator.InitializeNeighborhood();

  std::size_t overlap {2};
  communicator.AddCells(overlap);

  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    std::vector<Cell2D<double>> exp_cell;
    auto nx {cuboid_geometry.rGetCuboid(global_idx).GetNx()};
    auto ny {cuboid_geometry.rGetCuboid(global_idx).GetNy()};
    for (gsl::index x {0}; x < nx + 2 * overlap; ++x) {
      for (gsl::index y {0}; y < ny + 2 * overlap; ++y) {
        if (x < overlap || x > nx + overlap - 1 ||
            y < overlap || y > ny + overlap - 1) {
          auto tmp_index {global_idx};
          auto tmp_x {x - static_cast<gsl::index>(overlap)};
          auto tmp_y {y - static_cast<gsl::index>(overlap)};
          auto phys_R {cuboid_geometry.GetPhysR(tmp_index, tmp_x, tmp_y)};
          if (cuboid_geometry.HasCuboid(phys_R, tmp_index)) {
            exp_cell.push_back(Cell2D<double>{tmp_index, tmp_x, tmp_y,
                phys_R});
          }
        }
      }
    }
    CHECK_EQUAL(exp_cell.size(),
        tester.GetNeighborhood(communicator)[local_idx].GetInCellsSize());
    for (gsl::index i {0}; i < exp_cell.size(); ++i) {
      CHECK_EQUAL(exp_cell[i].mGlobalCuboidIndex,
          tester.GetNeighborhood(communicator)[local_idx].rGetInCell(i)
              .mGlobalCuboidIndex);
      CHECK(testutil::CheckEqualVector(
          tester.GetNeighborhood(communicator)[local_idx].rGetInCell(i)
              .mLatticeR, exp_cell[i].mLatticeR));
      CHECK(testutil::CheckCloseVector(
          tester.GetNeighborhood(communicator)[local_idx].rGetInCell(i)
              .mPhysR, exp_cell[i].mPhysR, g_zero_tol));
    }
  }
}

TEST(TestCommunicator2D_Initialize)
{
  TestCommunicator2D tester;
  TestCuboidNeighborhood2D nbr_tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer};
  Communicator2D<double> communicator {super_geometry};

  communicator.InitializeNeighborhood();

  std::vector<std::vector<Cell2D<double>>> exp_cell;
  std::vector<std::vector<gsl::index>> exp_nbr_cuboid;
  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    exp_cell.push_back(std::vector<Cell2D<double>>());
    exp_nbr_cuboid.push_back(std::vector<gsl::index>());
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    if (global_idx < 4) {
      communicator.AddCell(local_idx, 0, 0);
      Cell2D<double> cell {global_idx, 0, 0,
          cuboid_geometry.GetPhysR(global_idx, gsl::index{0}, gsl::index{0})};
      if (cuboid_geometry.HasCuboid(cell.mPhysR, cell.mGlobalCuboidIndex)) {
        exp_cell[local_idx].push_back(cell);
        exp_nbr_cuboid[local_idx].push_back(global_idx);
      }
    }
    // Skip global_idx == 4
    if (global_idx > 4) {
      for (gsl::index i {0}; i < 2; ++i) {
        communicator.AddCell(local_idx, i, i);
        Cell2D<double> cell {global_idx, i, i,
            cuboid_geometry.GetPhysR(global_idx, i, i)};
        if (cuboid_geometry.HasCuboid(cell.mPhysR, cell.mGlobalCuboidIndex)) {
          exp_cell[local_idx].push_back(cell);
          exp_nbr_cuboid[local_idx].push_back(global_idx);
        }
      }
    }
  }
  communicator.Initialize();

  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    // InCells
    CHECK_EQUAL(exp_cell[local_idx].size(),
        tester.GetNeighborhood(communicator)[local_idx].GetInCellsSize());
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    if (global_idx < 4) {
      CHECK_EQUAL(1, nbr_tester.pGetTmpInNbrNumCells(tester.rGetNeighborhood(
          communicator)[local_idx])[global_idx]);
      CHECK_EQUAL(1, tester.GetNeighborhood(communicator)[local_idx]
          .GetInNeighborCuboidsSize());
    }
    else if (global_idx > 4) {
      CHECK_EQUAL(2, nbr_tester.pGetTmpInNbrNumCells(tester.rGetNeighborhood(
          communicator)[local_idx])[global_idx]);
      // Neighbor cuboid number only gets pushed back once
      CHECK_EQUAL(1, tester.GetNeighborhood(communicator)[local_idx]
          .GetInNeighborCuboidsSize());
    }
    else {
      CHECK_EQUAL(0, nbr_tester.pGetTmpInNbrNumCells(tester.rGetNeighborhood(
          communicator)[local_idx])[global_idx]);
      CHECK_EQUAL(0, tester.GetNeighborhood(communicator)[local_idx]
          .GetInNeighborCuboidsSize());
    }
    for (gsl::index i {0};
        i < nbr_tester.GetInNbrNumCells(tester.GetNeighborhood(
            communicator)[local_idx]).size(); ++i) {
      CHECK_EQUAL(exp_nbr_cuboid[local_idx][i],
          tester.GetNeighborhood(communicator)[local_idx]
              .rGetInNeighborCuboid(i));
      if (global_idx < 4) {
        CHECK_EQUAL(1, nbr_tester.GetInNbrNumCells(
            tester.GetNeighborhood(communicator)[local_idx])[i]);
      }
      else if (global_idx > 4) {
        CHECK_EQUAL(2, nbr_tester.GetInNbrNumCells(
            tester.GetNeighborhood(communicator)[local_idx])[i]);
      }
      else {
        CHECK_EQUAL(0, nbr_tester.GetInNbrNumCells(
            tester.GetNeighborhood(communicator)[local_idx])[i]);
      }
    }
    CHECK(nbr_tester.GetHasInitializedInNeighbor(
        tester.rGetNeighborhood(communicator)[local_idx]));
    // ExCells
    CHECK_EQUAL(exp_cell[local_idx].size(), nbr_tester.GetExCells(
        tester.GetNeighborhood(communicator)[local_idx]).size());
    for (gsl::index i {0};
        i < nbr_tester.GetExNbrCuboids(
            tester.rGetNeighborhood(communicator)[local_idx]).size(); ++i) {
      CHECK_EQUAL(exp_nbr_cuboid[local_idx][i],
          nbr_tester.GetExNbrCuboids(
              tester.rGetNeighborhood(communicator)[local_idx])[i]);
      if (global_idx < 4) {
        CHECK_EQUAL(1, nbr_tester.GetExNbrNumCells(
            tester.rGetNeighborhood(communicator)[local_idx])[i]);
      }
      else if (global_idx > 4) {
        CHECK_EQUAL(2, nbr_tester.GetExNbrNumCells(
            tester.rGetNeighborhood(communicator)[local_idx])[i]);
      }
      else {
        CHECK_EQUAL(0, nbr_tester.GetExNbrNumCells(
            tester.rGetNeighborhood(communicator)[local_idx])[i]);
      }
    }
    CHECK(nbr_tester.GetHasInitializedExNeighbor(
        tester.rGetNeighborhood(communicator)[local_idx]));
  }
}

TEST(TestCommunicator2D_Reset)
{
  TestCommunicator2D tester;
  TestCuboidNeighborhood2D nbr_tester;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};
  auto nc {8u};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {indicator_cuboid, delta_R, nc};
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  SuperGeometry2D<double> super_geometry {cuboid_geometry, load_balancer};
  Communicator2D<double> communicator {super_geometry};

  communicator.InitializeNeighborhood();

  std::vector<std::vector<Cell2D<double>>> exp_cell;
  std::vector<std::vector<gsl::index>> exp_nbr_cuboid;
  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    exp_cell.push_back(std::vector<Cell2D<double>>());
    exp_nbr_cuboid.push_back(std::vector<gsl::index>());
    auto global_idx {load_balancer.GetGlobalIndex(local_idx)};
    if (global_idx < 4) {
      communicator.AddCell(local_idx, 0, 0);
      Cell2D<double> cell {global_idx, 0, 0,
          cuboid_geometry.GetPhysR(global_idx, gsl::index{0}, gsl::index{0})};
      if (cuboid_geometry.HasCuboid(cell.mPhysR, cell.mGlobalCuboidIndex)) {
        exp_cell[local_idx].push_back(cell);
        exp_nbr_cuboid[local_idx].push_back(global_idx);
      }
    }
    // Skip global_idx == 4
    if (global_idx > 4) {
      for (gsl::index i {0}; i < 2; ++i) {
        communicator.AddCell(local_idx, i, i);
        Cell2D<double> cell {global_idx, i, i,
            cuboid_geometry.GetPhysR(global_idx, i, i)};
        if (cuboid_geometry.HasCuboid(cell.mPhysR, cell.mGlobalCuboidIndex)) {
          exp_cell[local_idx].push_back(cell);
          exp_nbr_cuboid[local_idx].push_back(global_idx);
        }
      }
    }
  }
  communicator.Initialize();
  CHECK(tester.GetIsInitialized(communicator));

  communicator.Reset();
  CHECK(!tester.GetIsInitialized(communicator));
  for (gsl::index local_idx {0}; local_idx < load_balancer.GetSize();
      ++local_idx) {
    CHECK(!nbr_tester.GetHasInitializedInNeighbor(
          tester.rGetNeighborhood(communicator)[local_idx]));
    CHECK(!nbr_tester.GetHasInitializedExNeighbor(
          tester.rGetNeighborhood(communicator)[local_idx]));
  }
}
}
}  // namespace iblbm
