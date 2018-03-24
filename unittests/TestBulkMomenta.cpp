#include <gsl/gsl>
#include <iostream>

#include "BgkDynamics.hpp"
#include "BulkMomenta.hpp"
#include "Cell.hpp"
#include "Descriptor.hpp"
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

TEST(TestBulkMomenta_ComputeRho)
{

  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  auto result = 1.0;
  for (gsl::index i = 0; i < descriptor::D2Q9Descriptor<double>::sQ; ++i) {
    cell[i] = i;
    result += static_cast<double>(i);
  }
  CHECK_CLOSE(result, bulk_momenta.ComputeRho(cell), g_zero_tol);
}
}
}  // namespace iblbm
