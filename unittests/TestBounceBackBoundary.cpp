#include "BounceBackBoundary.hpp"
#include "CollisionNsf.hpp"
#include "IblbmUtilities.hpp"
#include "Lattice2D.hpp"
#include "UnitTest++.h"

namespace iblbm
{
SUITE(TestBoundaryCondition)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
static const auto g_num_dirs = 9u;
static const auto g_nx = 2u;
static const auto g_ny = 4u;
static const auto g_is_modify_stream = true;
// Initial values
static const auto g_initial_density = 1.2;
static const std::vector<double> g_initial_velocity = {3.4, 5.6};

TEST(TestBounceBackBoundary_Fullway_Constructor)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);
  BounceBackBoundary fullway_bounce_back(lattice, &collision);

  CHECK_EQUAL(true, fullway_bounce_back.IsBeforeStream());
  CHECK_EQUAL(false, fullway_bounce_back.IsDuringStream());
}

TEST(TestBounceBackBoundary_Fullway_AddNode_CollisionNsf)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);
  BounceBackBoundary fullway_bounce_back(lattice, &collision);

  for (auto x = 0u; x < g_nx; ++x) fullway_bounce_back.AddNode(x, /*y=*/1u);
  auto nodes = fullway_bounce_back.rGetNodes();
  auto is_skip = collision.rGetIsSkip();
  CHECK_EQUAL(g_nx, nodes.size());
  for (auto x = 0u; x < g_nx; ++x) {
    auto n = x + g_nx;
    CHECK_EQUAL(n, nodes[x].mIndex);
  }
  // Check that the right nodes have been added to be skipped
  for (auto y = 0u; y < g_ny; ++y) {
    for (auto x = 0u; x < g_nx; ++x) {
      auto n = x + y * g_nx;
      if (y == 1) {
        CHECK_EQUAL(true, is_skip[n]);
      }
      else {
        CHECK_EQUAL(false, is_skip[n]);
      }
    }
  }
}

TEST(TestBounceBackBoundary_Fullway_UpdateNodes_CollisionNsf)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);
  BounceBackBoundary fullway_bounce_back(lattice, &collision);

  std::vector<double> node = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<double> bounced_node = {0, 3, 4, 1, 2, 7, 8, 5, 6};
  std::vector<std::vector<double>> df(g_nx * g_ny, node);
  for (auto x = 0u; x < g_nx; ++x) fullway_bounce_back.AddNode(x, /*y=*/1u);
  fullway_bounce_back.UpdateNodes(df, !g_is_modify_stream);

  for (auto y = 0u; y < g_ny; ++y) {
    for (auto x = 0u; x < g_nx; ++x) {
      auto n = x + y * g_nx;
      if (y == 1) {
        CHECK(util::CheckCloseVector(df[n], bounced_node, g_zero_tol));
      }
      else {
        CHECK(util::CheckCloseVector(df[n], node, g_zero_tol));
      }
    }
  }
}

TEST(TestBounceBackBoundary_Halfway_Constructor)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  BounceBackBoundary fullway_bounce_back(lattice);

  CHECK_EQUAL(true, fullway_bounce_back.IsBeforeStream());
  CHECK_EQUAL(true, fullway_bounce_back.IsDuringStream());
}
}
}  // namespace iblbm
