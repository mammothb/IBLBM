#include <iostream>
#include <vector>

#include "CollisionNsf.hpp"
#include "Lattice2D.hpp"
#include "LatticeBoltzmannMethod.hpp"
#include "Stream2D.hpp"
#include "UnitTest++.h"

namespace iblbm
{
SUITE(TestLatticeBoltzmannMethod)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
static const auto g_num_dirs = 9u;
static const auto g_nx = 6u;
static const auto g_ny = 8u;
// Initial values
static const auto g_initial_density = 1.2;
static const std::vector<double> g_initial_velocity = {3.4, 5.6};

TEST(TestLatticeBoltzmannMethod_Constructor_CollisionNsf_UniformDensity_ZeroVelocity)
{
  std::vector<double> zero_initial_velocity = {0.0, 0.0};
  Lattice2D lattice(g_ny, g_nx, zero_initial_velocity);
  Stream2D stream(lattice);
  CollisionNsf collision(lattice, g_initial_density);
  LatticeBoltzmannMethod lbm(lattice, collision, stream);
  std::vector<double> expected = {0.533333, 0.133333, 0.133333, 0.133333,
      0.133333, 0.033333, 0.033333, 0.033333, 0.033333};
  auto df = lbm.rGetDistribution();
  for (auto i = 0u; i < g_num_dirs; ++i) {
    for (auto node : df) CHECK_CLOSE(expected[i], node[i], g_loose_tol);
  } // i
}

TEST(TestLatticeBoltzmannMethod_Constructor_CollisionNsf_UniformDensity_NonZeroVelocity)
{
  Lattice2D lattice(g_ny, g_nx, g_initial_velocity);
  Stream2D stream(lattice);
  CollisionNsf collision(lattice, g_initial_density);
  LatticeBoltzmannMethod lbm(lattice, collision, stream);
  std::vector<double> expected = {-33.80266667, -0.154666667, 12.60533333,
      -2.874666667, 8.125333333, 10.93733333, -1.166666667, 9.137333333,
      -1.606666667};
  auto edf = collision.rGetEquilibriumDistribution();
  auto df = lbm.rGetDistribution();
  for (auto i = 0u; i < g_num_dirs; ++i) {
    for (auto node : edf) CHECK_CLOSE(expected[i], node[i], g_loose_tol);
  } // i
}
}
}  // namespace iblbm
