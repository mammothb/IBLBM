#include <iostream>
#include <vector>

#include "CollisionNsf.hpp"
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
static const std::vector<double> g_weight = {16.0 / 36.0, 4.0 / 36.0,
    4.0 / 36.0, 4.0 / 36.0, 4.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0,
    1.0 / 36.0};
static const std::vector<std::vector<double>> g_discrete_velocity = {
    {0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {-1.0, 0.0}, {0.0, -1.0}, {1.0, 1.0},
    {-1.0, 1.0}, {-1.0, -1.0}, {1.0, -1.0}};
// Initial values
static const auto g_tau = 0.8;
static const auto g_initial_density = 1.2;
static const std::vector<double> g_initial_velocity = {3.4, 5.6};
static const std::vector<double> g_initial_force = {7.8, 9.0};

TEST(TestCollisionNsf_Constructor)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);

  const auto r_edf = collision.rGetEquilibriumDistribution();
  CHECK_EQUAL(g_nx * g_ny, r_edf.size());

  // Variables for testing
  auto cs_sqr = 1.0 * 1.0 / 3.0;
  auto u_sqr = util::InnerProduct(g_initial_velocity, g_initial_velocity) /
      2.0 / cs_sqr;
  for (auto n = 0u; n < g_nx * g_ny; ++n) {
    for (auto i = 0u; i < g_weight.size(); ++i) {
      auto c_dot_u = util::InnerProduct(g_discrete_velocity[i],
          g_initial_velocity) / cs_sqr;
      auto edf = g_initial_density * g_weight[i] * (1.0 + c_dot_u *
          (1.0 + c_dot_u) - u_sqr);
      CHECK_CLOSE(edf, r_edf[n][i], g_loose_tol);
    }
  }
}

TEST(TestCollisionNsf_SetDensity)
{
  auto density = g_initial_density * 2.0;
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);

  collision.SetDensity(density);

  const auto r_edf = collision.rGetEquilibriumDistribution();
  const auto r_density = collision.rGetDensity();
  CHECK_EQUAL(g_nx * g_ny, r_density.size());
  for (auto node : r_density) {
    CHECK_CLOSE(density, node, g_zero_tol);
  }
  // Variables for testing
  auto cs_sqr = 1.0 * 1.0 / 3.0;
  auto u_sqr = util::InnerProduct(g_initial_velocity, g_initial_velocity) /
      2.0 / cs_sqr;
  for (auto n = 0u; n < g_nx * g_ny; ++n) {
    for (auto i = 0u; i < g_weight.size(); ++i) {
      auto c_dot_u = util::InnerProduct(g_discrete_velocity[i],
          g_initial_velocity) / cs_sqr;
      auto edf = density * g_weight[i] * (1.0 + c_dot_u * (1.0 + c_dot_u) -
          u_sqr);
      CHECK_CLOSE(edf, r_edf[n][i], g_loose_tol);
    }
  }
}

TEST(TestCollisionNsf_ComputeEquilibriumDistribution)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);

  // Change the velocity so we will get a different value for edf
  std::vector<double> velocity = {0.1, 0.2};
  lattice.SetVelocity(velocity);
  collision.ComputeEquilibriumDistribution();

  const auto r_edf = collision.rGetEquilibriumDistribution();

  // Variables for testing
  auto cs_sqr = 1.0 * 1.0 / 3.0;
  for (auto n = 0u; n < g_nx * g_ny; ++n) {
    auto u_sqr = util::InnerProduct(velocity, velocity) / 2.0 / cs_sqr;
    for (auto i = 0u; i < g_weight.size(); ++i) {
      auto c_dot_u = util::InnerProduct(g_discrete_velocity[i], velocity) /
          cs_sqr;
      auto edf = g_initial_density * g_weight[i] * (1.0 + c_dot_u *
          (1.0 + c_dot_u) - u_sqr);
      CHECK_CLOSE(edf, r_edf[n][i], g_loose_tol);
    }
  }
}

TEST(TestCollisionNsf_Collide)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);

  collision.SetRelaxationTime(g_tau);
  collision.SetForce(g_initial_force);

  const auto nc = lattice.GetNumberOfDirections();
  std::vector<std::vector<double>> df(g_nx * g_ny,
      std::vector<double>(nc, 0.0));
  for (auto n = 0u; n < g_nx * g_ny; ++n) {
    for (auto i = 0u; i < nc; ++i) {
      df[n][i] = static_cast<double>(n * 10 + i) / 10.0;
    }
  }
  auto expected_df = df;

  collision.Collide(df);

  const auto r_edf = collision.rGetEquilibriumDistribution();

  // Variables for testing
  auto cs_sqr = 1.0 * 1.0 / 3.0;
  for (auto n = 0u; n < g_nx * g_ny; ++n) {
    for (auto i = 0u; i < g_weight.size(); ++i) {
      const auto c_dot_u = util::InnerProduct(g_discrete_velocity[i],
          g_initial_velocity) / cs_sqr;
      std::vector<double> src_dot_product = {
          (g_discrete_velocity[i][0] - g_initial_velocity[0] + c_dot_u *
              g_discrete_velocity[i][0]) / cs_sqr,
          (g_discrete_velocity[i][1] - g_initial_velocity[1] + c_dot_u *
              g_discrete_velocity[i][1]) / cs_sqr};
      const auto src_i = (1.0 - 0.5 / g_tau) * g_weight[i] *
          util::InnerProduct(src_dot_product, g_initial_force);
      expected_df[n][i] += (r_edf[n][i] - expected_df[n][i]) / g_tau + src_i;
      CHECK_CLOSE(expected_df[n][i], df[n][i], g_loose_tol);
    }
  }
}
}
}  // namespace iblbm
