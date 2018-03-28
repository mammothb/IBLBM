#include <iostream>

#include "Lattice2D.hpp"
#include "UnitTestCustomUtilities.hpp"
#include "UnitTest++/UnitTest++.h"

namespace iblbm
{
SUITE(TestLatticeModel)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
static const auto g_nx = 6u;
static const auto g_ny = 8u;
// Initial values
static const std::vector<double> g_initial_velocity = {3.4, 5.6};

TEST(TestLattice2D_Constructor)
{
  auto num_dim = 2u;
  auto num_dir = 9u;
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  auto velocity = lattice.rGetVelocity();

  CHECK_EQUAL(num_dim, lattice.GetNumberOfDimensions());
  CHECK_EQUAL(num_dir, lattice.GetNumberOfDirections());
  CHECK_EQUAL(g_nx, lattice.GetNx());
  CHECK_EQUAL(g_ny, lattice.GetNy());
  CHECK_EQUAL(g_nx * g_ny, lattice.GetNumberOfNodes());
  CHECK_CLOSE(1.0, lattice.GetPropagationSpeed(), 1e-6);
  for (auto node : velocity) {
    CHECK(testutil::CheckCloseVector(node, g_initial_velocity, g_zero_tol));
  }
}

TEST(TestLattice2D_DetermineOrientation)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  for (auto y = 0u; y < g_ny; ++y) {
    for (auto x = 0u; x < g_nx; ++x) {
      auto n = x + g_nx * y;
      if (y == 0) {
        if (x == 0) {
          CHECK_EQUAL(Edge::LOWER_LEFT, lattice.DetermineOrientation(n));
        }
        else if (x == g_nx - 1) {
          CHECK_EQUAL(Edge::LOWER_RIGHT, lattice.DetermineOrientation(n));
        }
        else {
          CHECK_EQUAL(Edge::LOWER, lattice.DetermineOrientation(n));
        }
      }
      else if (y == g_ny - 1) {
        if (x == 0) {
          CHECK_EQUAL(Edge::UPPER_LEFT, lattice.DetermineOrientation(n));
        }
        else if (x == g_nx - 1) {
          CHECK_EQUAL(Edge::UPPER_RIGHT, lattice.DetermineOrientation(n));
        }
        else {
          CHECK_EQUAL(Edge::UPPER, lattice.DetermineOrientation(n));
        }
      }
      else {
        if (x == 0) {
          CHECK_EQUAL(Edge::LEFT, lattice.DetermineOrientation(n));
        }
        else if (x == g_nx - 1) {
          CHECK_EQUAL(Edge::RIGHT, lattice.DetermineOrientation(n));
        }
        else {
          CHECK_EQUAL(Edge::NOT_EDGE, lattice.DetermineOrientation(n));
        }
      }
    }
  }
}
}
}  // namespace iblbm
