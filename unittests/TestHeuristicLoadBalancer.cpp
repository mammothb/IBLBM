#include <limits>
#include <map>

#include "gsl/gsl"
#include "UnitTest++/UnitTest++.h"
#include "CuboidGeometry2D.hpp"
#include "HeuristicLoadBalancer.hpp"
#include "IndicatorFunctor2D.hpp"

namespace iblbm
{
class TestHeuristicLoadBalancer
{
 public:
  CuboidGeometry2D<double>* pGetCuboidGeometry2D(
      HeuristicLoadBalancer<double>& rLoadBalancer)
  {
    return rLoadBalancer.mpCuboidGeometry2D;
  }

  double GetRatioFullEmpty(HeuristicLoadBalancer<double>& rLoadBalancer)
  {
    return rLoadBalancer.mRatioFullEmpty;
  }
};

SUITE(TestParallel)
{
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestHeuristicLoadBalancer_Constructor_Default)
{
  TestHeuristicLoadBalancer tester;
  HeuristicLoadBalancer<double> load_balancer;
  CHECK(tester.pGetCuboidGeometry2D(load_balancer) == nullptr);
  CHECK_CLOSE(1.0, tester.GetRatioFullEmpty(load_balancer), g_zero_tol);
}

TEST(TestHeuristicLoadBalancer_Constructor_2D_Default)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};
  auto nc {7u};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};

  auto nx {static_cast<std::size_t>(extent[0] / delta_R + 1.5)};
  auto ny {static_cast<std::size_t>(extent[1] / delta_R + 1.5)};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny,
      nc};
  cuboid_geometry.Shrink(indicator_cuboid);
  HeuristicLoadBalancer<double> load_balancer {cuboid_geometry};

  std::vector<std::size_t> exp_weight {28, 28, 24, 24, 35, 28, 28};
  for (gsl::index i {0}; i < exp_weight.size(); ++i)
      CHECK_EQUAL(exp_weight[i], cuboid_geometry.rGetCuboid(i).GetWeight());
  if (MpiManager::Instance().IsOk()) {
    auto rank {MpiManager::Instance().GetRank()};
    std::vector<std::size_t> exp_size {1, 2, 2, 2};
    std::vector<std::vector<bool>> exp_is_local {
        {false, false, false, false, true, false, false},
        {true, false, false, false, false, false, true},
        {false, true, true, false, false, false, false},
        {false, false, false, true, false, true, false}};
    std::vector<std::map<gsl::index, gsl::index>> exp_local_index {
        {{4, 0}},
        {{0, 0}, {6, 1}},
        {{1, 0}, {2, 1}},
        {{3, 0}, {5, 1}}};
    std::map<gsl::index, std::size_t> exp_rank{{0, 1}, {1, 2}, {2, 2},
        {3, 3}, {4, 0}, {5, 3}, {6, 1}};
    CHECK_EQUAL(exp_size[rank], load_balancer.GetSize());
    for (gsl::index i {0}; i < nc; ++i) {
      CHECK_EQUAL(exp_is_local[rank][i], load_balancer.IsLocal(i));
      // Since the sizes of mLocalIndex and mGlobalIndex are restricted by
      // the number of assigned block, we only check if they are assigned to
      // the thread
      if (exp_is_local[rank][i]) {
        CHECK_EQUAL(exp_local_index[rank][i],
            load_balancer.GetLocalIndex(i));
        CHECK_EQUAL(i, load_balancer.GetGlobalIndex(
            exp_local_index[rank][i]));
      }
      CHECK_EQUAL(exp_rank[i], load_balancer.GetRank(i));
    }
  }
  else {
    CHECK_EQUAL(nc, load_balancer.GetSize());
    for (gsl::index i {0}; i < nc; ++i) {
      CHECK(load_balancer.IsLocal(i));
      CHECK_EQUAL(i, load_balancer.GetLocalIndex(i));
      CHECK_EQUAL(i, load_balancer.GetGlobalIndex(i));
      CHECK_EQUAL(0, load_balancer.GetRank(i));
    }
  }
}

TEST(TestHeuristicLoadBalancer_Initialize_2D_Default)
{
  HeuristicLoadBalancer<double> load_balancer;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};
  auto nc {7u};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};

  auto nx {static_cast<std::size_t>(extent[0] / delta_R + 1.5)};
  auto ny {static_cast<std::size_t>(extent[1] / delta_R + 1.5)};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny,
      nc};
  cuboid_geometry.Shrink(indicator_cuboid);

  load_balancer.Initialize(cuboid_geometry);

  std::vector<std::size_t> exp_weight {28, 28, 24, 24, 35, 28, 28};
  for (gsl::index i {0}; i < exp_weight.size(); ++i)
      CHECK_EQUAL(exp_weight[i], cuboid_geometry.rGetCuboid(i).GetWeight());
  if (MpiManager::Instance().IsOk()) {
    auto rank {MpiManager::Instance().GetRank()};
    std::vector<std::size_t> exp_size {1, 2, 2, 2};
    std::vector<std::vector<bool>> exp_is_local {
        {false, false, false, false, true, false, false},
        {true, false, false, false, false, false, true},
        {false, true, true, false, false, false, false},
        {false, false, false, true, false, true, false}};
    std::vector<std::map<gsl::index, gsl::index>> exp_local_index {
        {{4, 0}},
        {{0, 0}, {6, 1}},
        {{1, 0}, {2, 1}},
        {{3, 0}, {5, 1}}};
    std::map<gsl::index, std::size_t> exp_rank{{0, 1}, {1, 2}, {2, 2},
        {3, 3}, {4, 0}, {5, 3}, {6, 1}};
    CHECK_EQUAL(exp_size[rank], load_balancer.GetSize());
    for (gsl::index i {0}; i < nc; ++i) {
      CHECK_EQUAL(exp_is_local[rank][i], load_balancer.IsLocal(i));
      // Since the sizes of mLocalIndex and mGlobalIndex are restricted by
      // the number of assigned block, we only check if they are assigned to
      // the thread
      if (exp_is_local[rank][i]) {
        CHECK_EQUAL(exp_local_index[rank][i],
            load_balancer.GetLocalIndex(i));
        CHECK_EQUAL(i, load_balancer.GetGlobalIndex(
            exp_local_index[rank][i]));
      }
      CHECK_EQUAL(exp_rank[i], load_balancer.GetRank(i));
    }
  }
  else {
    CHECK_EQUAL(nc, load_balancer.GetSize());
    for (gsl::index i {0}; i < nc; ++i) {
      CHECK(load_balancer.IsLocal(i));
      CHECK_EQUAL(i, load_balancer.GetLocalIndex(i));
      CHECK_EQUAL(i, load_balancer.GetGlobalIndex(i));
      CHECK_EQUAL(0, load_balancer.GetRank(i));
    }
  }
}

TEST(TestHeuristicLoadBalancer_Swap)
{
  HeuristicLoadBalancer<double> load_balancer;

  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.5};
  auto nc {7u};

  Vector2D<double> origin {x_pos, y_pos};
  Vector2D<double> extent {6, 7};

  auto nx {static_cast<std::size_t>(extent[0] / delta_R + 1.5)};
  auto ny {static_cast<std::size_t>(extent[1] / delta_R + 1.5)};

  IndicatorCuboid2D<double> indicator_cuboid {extent, origin};
  CuboidGeometry2D<double> cuboid_geometry {x_pos, y_pos, delta_R, nx, ny,
      nc};
  cuboid_geometry.Shrink(indicator_cuboid);

  HeuristicLoadBalancer<double> load_balancer_2 {cuboid_geometry};

  load_balancer.Swap(load_balancer_2);

  std::vector<std::size_t> exp_weight {28, 28, 24, 24, 35, 28, 28};
  for (gsl::index i {0}; i < exp_weight.size(); ++i)
      CHECK_EQUAL(exp_weight[i], cuboid_geometry.rGetCuboid(i).GetWeight());
  if (MpiManager::Instance().IsOk()) {
    auto rank {MpiManager::Instance().GetRank()};
    std::vector<std::size_t> exp_size {1, 2, 2, 2};
    std::vector<std::vector<bool>> exp_is_local {
        {false, false, false, false, true, false, false},
        {true, false, false, false, false, false, true},
        {false, true, true, false, false, false, false},
        {false, false, false, true, false, true, false}};
    std::vector<std::map<gsl::index, gsl::index>> exp_local_index {
        {{4, 0}},
        {{0, 0}, {6, 1}},
        {{1, 0}, {2, 1}},
        {{3, 0}, {5, 1}}};
    std::map<gsl::index, std::size_t> exp_rank{{0, 1}, {1, 2}, {2, 2},
        {3, 3}, {4, 0}, {5, 3}, {6, 1}};
    CHECK_EQUAL(exp_size[rank], load_balancer.GetSize());
    for (gsl::index i {0}; i < nc; ++i) {
      CHECK_EQUAL(exp_is_local[rank][i], load_balancer.IsLocal(i));
      // Since the sizes of mLocalIndex and mGlobalIndex are restricted by
      // the number of assigned block, we only check if they are assigned to
      // the thread
      if (exp_is_local[rank][i]) {
        CHECK_EQUAL(exp_local_index[rank][i],
            load_balancer.GetLocalIndex(i));
        CHECK_EQUAL(i, load_balancer.GetGlobalIndex(
            exp_local_index[rank][i]));
      }
      CHECK_EQUAL(exp_rank[i], load_balancer.GetRank(i));
    }
  }
  else {
    CHECK_EQUAL(nc, load_balancer.GetSize());
    for (gsl::index i {0}; i < nc; ++i) {
      CHECK(load_balancer.IsLocal(i));
      CHECK_EQUAL(i, load_balancer.GetLocalIndex(i));
      CHECK_EQUAL(i, load_balancer.GetGlobalIndex(i));
      CHECK_EQUAL(0, load_balancer.GetRank(i));
    }
  }
}
}
}  // namespace iblbm
