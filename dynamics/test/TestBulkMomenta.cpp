#include <iostream>

#include "UnitTest++/UnitTest++.h"
#include "BgkDynamics.hpp"
#include "BulkMomenta.hpp"
#include "Cell.hpp"
#include "Descriptor.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
SUITE(TestDynamics)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
// Initial values
static const auto g_relaxation_time = 0.8;

TEST(TestBulkMomenta_D2Q9_ComputeRho)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  auto exp_rho = 1.0;
  for (gsl::index q = 0; q < B::sQ; ++q) {
    cell[q] = q;
    exp_rho += static_cast<double>(q);
  }
  CHECK_CLOSE(exp_rho, bulk_momenta.ComputeRho(cell), g_zero_tol);
}

TEST(TestBulkMomenta_D2Q9_ComputeU)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  auto rho = 1.0;
  std::vector<double> actual_u(B::sD, 0.0);
  std::vector<double> exp_u(B::sD, 0.0);
  for (gsl::index q = 0; q < B::sQ; ++q) {
    cell[q] = q;
    rho += static_cast<double>(q);
  }
  exp_u[0] = ((cell[5] + cell[6] + cell[7]) - (cell[1] + cell[2] +
      cell[3])) / rho;
  exp_u[1] = ((cell[7] + cell[8] + cell[1]) - (cell[3] + cell[4] +
      cell[5])) / rho;
  bulk_momenta.ComputeU(cell, actual_u);
  CHECK(testutil::CheckCloseVector(actual_u, exp_u, g_loose_tol));
}

TEST(TestBulkMomenta_D2Q9_ComputeRhoAndU)
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
  for (gsl::index q = 0; q < B::sQ; ++q) {
    cell[q] = q;
    exp_rho += static_cast<double>(q);
  }

  bulk_momenta.ComputeRhoAndU(cell, actual_rho, actual_u);

  exp_u[0] = ((cell[5] + cell[6] + cell[7]) - (cell[1] + cell[2] +
      cell[3])) / exp_rho;
  exp_u[1] = ((cell[7] + cell[8] + cell[1]) - (cell[3] + cell[4] +
      cell[5])) / exp_rho;

  CHECK_CLOSE(exp_rho, actual_rho, g_loose_tol);
  CHECK(testutil::CheckCloseVector(actual_u, exp_u, g_loose_tol));
}
}
}  // namespace iblbm
