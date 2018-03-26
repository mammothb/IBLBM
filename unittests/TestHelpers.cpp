#include <numeric>

#include "BgkDynamics.hpp"
#include "BulkMomenta.hpp"
#include "Descriptor.hpp"
#include "Helpers.hpp"
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
static const auto g_u_sqr = std::inner_product(g_cell_velocity.begin(),
      g_cell_velocity.end(), g_cell_velocity.begin(), 0.0);

TEST(TestLbmHelpers_ComputeEquilibrium)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;

  for (auto q = 0u; q < B::sQ; ++q) {
    auto e_dot_u = 0.0;
    for (auto d = 0u; d < B::sD; ++d)
        e_dot_u += B::sE[q][d] * g_cell_velocity[d];
    auto exp_feq = g_cell_density * B::sWeight[q] * (1.0 + B::sInvCsSqr *
        e_dot_u + B::sInvCsSqr * B::sInvCsSqr / 2.0 * e_dot_u * e_dot_u -
        B::sInvCsSqr / 2.0 * g_u_sqr) - B::sWeight[q];
    auto feq = LbmHelper<double, descriptor::D2Q9Descriptor>::
        ComputeEquilibrium(q, g_cell_density, g_cell_velocity, g_u_sqr);

    CHECK_CLOSE(exp_feq, feq, g_loose_tol);
  }
}

TEST(TestLbmHelpers_BgkCollide)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  std::vector<double> exp_cell(B::sQ);
  for (auto q = 0u; q < B::sQ; ++q) {
    cell[q] = q + 1.0;
    exp_cell[q] = q + 1.0;
  }
  auto omega = 1.0 / g_relaxation_time;
  auto u_sqr = LbmHelper<double, descriptor::D2Q9Descriptor>::
      BgkCollide(cell, g_cell_density, g_cell_velocity, omega);

  CHECK_CLOSE(g_u_sqr, u_sqr, g_loose_tol);
  for (auto q = 0u; q < B::sQ; ++q) {
    exp_cell[q] *= 1.0 - omega;
    exp_cell[q] += omega * LbmHelper<double, descriptor::D2Q9Descriptor>::
        ComputeEquilibrium(q, g_cell_density, g_cell_velocity, g_u_sqr);
    CHECK_CLOSE(exp_cell[q], cell[q], g_loose_tol);
  }
}

TEST(TestLbmHelpers_ComputeRho)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);

  auto exp_rho = 1.0;
  for (auto q = 0u; q < B::sQ; ++q) {
    cell[q] = q + 1.0;
    exp_rho += q + 1.0;
  }
  auto rho = LbmHelper<double, descriptor::D2Q9Descriptor>::ComputeRho(cell);

  CHECK_CLOSE(exp_rho, rho, g_loose_tol);
}

TEST(TestLbmHelpers_ComputeRhoAndU)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  auto actual_rho = 0.0;
  auto exp_rho = 1.0;
  std::vector<double> actual_u(B::sD, 0.0);
  std::vector<double> exp_u(B::sD, 0.0);
  for (auto q = 0u; q < B::sQ; ++q) {
    cell[q] = q;
    exp_rho += static_cast<double>(q);
  }
  LbmHelper<double, descriptor::D2Q9Descriptor>::ComputeRhoAndU(cell,
      actual_rho, actual_u);
  // Calculated expected fluid velocity using the more efficient method
  exp_u[0] = ((cell[5] + cell[6] + cell[7]) - (cell[1] + cell[2] +
      cell[3])) / exp_rho;
  exp_u[1] = ((cell[7] + cell[8] + cell[1]) - (cell[3] + cell[4] +
      cell[5])) / exp_rho;

  CHECK_CLOSE(exp_rho, actual_rho, g_loose_tol);
  CHECK(testutil::CheckCloseVector(actual_u, exp_u, g_loose_tol));
}
}
}  // namespace iblbm
