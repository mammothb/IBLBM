#include <iostream>

#include "BgkDynamics.hpp"
#include "BulkMomenta.hpp"
#include "Cell.hpp"
#include "Descriptor.hpp"
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

TEST(TestBulkMomenta_D2Q9_ComputeRho)
{
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  auto result = 1.0;
  for (auto q = 0u; q < descriptor::D2Q9Descriptor<double>::sQ; ++q) {
    cell[q] = q;
    result += static_cast<double>(q);
  }
  CHECK_CLOSE(result, bulk_momenta.ComputeRho(cell), g_zero_tol);
}

TEST(TestBulkMomenta_D2Q9_ComputeU)
{
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  auto rho = 1.0;
  std::vector<double> actual_u(descriptor::D2Q9Descriptor<double>::sD, 0.0);
  std::vector<double> expected_u(descriptor::D2Q9Descriptor<double>::sD,
      0.0);
  for (auto q = 0u; q < descriptor::D2Q9Descriptor<double>::sQ; ++q) {
    cell[q] = q;
    rho += static_cast<double>(q);
  }
  expected_u[0] = ((cell[5] + cell[6] + cell[7]) - (cell[1] + cell[2] +
      cell[3])) / rho;
  expected_u[1] = ((cell[7] + cell[8] + cell[1]) - (cell[3] + cell[4] +
      cell[5])) / rho;
  bulk_momenta.ComputeU(cell, actual_u);
  CHECK(testutil::CheckCloseVector(actual_u, expected_u, g_loose_tol));
}
}
}  // namespace iblbm
