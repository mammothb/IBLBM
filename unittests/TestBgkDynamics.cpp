#include <iostream>
#include <numeric>

#include "BgkDynamics.hpp"
#include "BulkMomenta.hpp"
#include "Cell.hpp"
#include "UnitTestCustomUtilities.hpp"
#include "UnitTest++.h"

namespace iblbm
{
SUITE(TestDynamics)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
// Initial values
static const auto g_relaxation_time = 0.8;
static const auto g_cell_density = 1.2;
static const std::vector<double> g_cell_velocity = {3.4, 5.6};

TEST(TestBgkDynamics_Constructor)
{
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);

  CHECK_CLOSE(1.0 / g_relaxation_time, dynamics.GetOmega(), g_loose_tol);
}

TEST(TestBgkDynamics_ComputeRho)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  auto exp_rho = 1.0;
  for (auto q = 0u; q < B::sQ; ++q) {
    cell[q] = q;
    exp_rho += static_cast<double>(q);
  }

  CHECK_CLOSE(exp_rho, dynamics.ComputeRho(cell), g_zero_tol);
}

TEST(TestBgkDynamics_ComputeEquilibrium)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);

  auto u_sqr = g_cell_velocity[0] * g_cell_velocity[0] + g_cell_velocity[1] *
      g_cell_velocity[1];
  for (auto q = 0u; q < B::sQ; ++q) {
    auto e_dot_u = B::sE[q][0] * g_cell_velocity[0] + B::sE[q][1] *
        g_cell_velocity[1];
    auto exp_feq = g_cell_density * B::sWeight[q] * (1.0 + B::sInvCsSqr *
        e_dot_u + B::sInvCsSqr * B::sInvCsSqr / 2.0 * e_dot_u * e_dot_u -
        B::sInvCsSqr / 2.0 * u_sqr) - B::sWeight[q];
    auto actual_feq = dynamics.ComputeEquilibrium(q, g_cell_density,
        g_cell_velocity, u_sqr);

    CHECK_CLOSE(exp_feq, actual_feq, g_loose_tol);
  }
}

TEST(TestBgkDynamics_Collide)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  for (auto q = 0u; q < B::sQ; ++q) cell[q] = q + 1;
  dynamics.Collide(cell);

  // Expected result
  auto omega = 1.0 / g_relaxation_time;
  std::vector<double> exp_cell(B::sQ);
  std::iota(exp_cell.begin(), exp_cell.end(), 1.0);
  auto density = 1.0;
  std::vector<double> velocity(B::sD, 0.0);
  // Compute fluid velocity
  for (auto q = 0u; q < B::sQ; ++q) {
    density += exp_cell[q];
    for (auto d = 0u; d < B::sD; ++d)
        velocity[d] += exp_cell[q] * B::sE[q][d];
  }
  for (auto d = 0u; d < B::sD; ++d) velocity[d] /= density;
  // Calculate convenience variable
  auto u_sqr = std::inner_product(velocity.begin(), velocity.end(),
      velocity.begin(), 0.0);
  // Calculate equilibrium
  for (auto q = 0u; q < B::sQ; ++q) {
    exp_cell[q] *= 1.0 - omega;
    auto e_dot_u = 0.0;
    for (auto d = 0u; d < B::sD; ++d) e_dot_u += B::sE[q][d] * velocity[d];
    exp_cell[q] += omega * (density * B::sWeight[q] * (1.0 + B::sInvCsSqr *
        e_dot_u + B::sInvCsSqr * B::sInvCsSqr * 0.5 * e_dot_u * e_dot_u -
        B::sInvCsSqr * 0.5 * u_sqr) - B::sWeight[q]);
  }

  for (auto q = 0u; q < B::sQ; ++q)
      CHECK_CLOSE(exp_cell[q], cell[q], g_loose_tol);
}
}
}  // namespace iblbm
