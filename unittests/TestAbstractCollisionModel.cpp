#include <vector>

#include "AbstractCollisionModel.hpp"
#include "IblbmUtilities.hpp"
#include "Lattice2D.hpp"
#include "UnitTest++.h"

namespace iblbm
{
SUITE(TestCollisionModel)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
static const auto g_nx = 2u;
static const auto g_ny = 4u;
// Initial values
static const auto g_initial_density = 1.2;
static const std::vector<double> g_initial_velocity = {3.4, 5.6};

TEST(TestAbstractCollisionModel_Constructor)
{
  Lattice2D lattice(g_nx, g_ny);
  AbstractCollisionModel collision(lattice);

  const auto r_edf = collision.rGetEquilibriumDistribution();
  CHECK_EQUAL(g_nx * g_ny, r_edf.size());
  for (auto node : r_edf) {
    for (auto direction : node) CHECK_CLOSE(0.0, direction, g_zero_tol);
  }
}

TEST(TestAbstractCollisionModel_InitializeDensity)
{
  Lattice2D lattice(g_nx, g_ny);
  AbstractCollisionModel collision(lattice);

  collision.InitializeDensity(g_initial_density);

  const auto r_density = collision.rGetDensity();
  CHECK_EQUAL(g_nx * g_ny, r_density.size());
  for (auto node : r_density) {
    CHECK_CLOSE(g_initial_density, node, g_zero_tol);
  }
}

TEST(TestAbstractCollisionModel_ComputeEquilibriumDistribution)
{
  Lattice2D lattice(g_nx, g_ny);
  AbstractCollisionModel collision(lattice);

  lattice.InitializeVelocity(g_initial_velocity);

  collision.InitializeDensity(g_initial_density);
  collision.ComputeEquilibriumDistribution();

  const auto r_density = collision.rGetDensity();
  const auto r_edf = collision.rGetEquilibriumDistribution();
  CHECK_EQUAL(g_nx * g_ny, r_edf.size());

  // Variables for testing
  std::vector<double> weight = {16.0 / 36.0, 4.0 / 36.0, 4.0 / 36.0,
      4.0 / 36.0, 4.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0,
      1.0 / 36.0};
  std::vector<std::vector<double>> discrete_velocity = {{0.0, 0.0},
      {1.0, 0.0}, {0.0, 1.0}, {-1.0, 0.0}, {0.0, -1.0}, {1.0, 1.0},
      {-1.0, 1.0}, {-1.0, -1.0}, {1.0, -1.0}};
  auto cs_sqr = 1.0 * 1.0 / 3.0;
  for (auto n = 0u; n < g_nx * g_ny; ++n) {
    auto u_sqr = util::InnerProduct(g_initial_velocity, g_initial_velocity) /
        2.0 / cs_sqr;
    for (auto i = 0u; i < weight.size(); ++i) {
      auto c_dot_u = util::InnerProduct(discrete_velocity[i],
          g_initial_velocity) / cs_sqr;
      auto edf = r_density[n] * weight[i] * (1.0 + c_dot_u *
          (1.0 + c_dot_u) - u_sqr);
      CHECK_CLOSE(edf, r_edf[n][i], g_loose_tol);
    }
  }
}
}
}  // namespace iblbm
