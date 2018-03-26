#include <cassert>
#include <iostream>

#include "BgkDynamics.hpp"
#include "BulkMomenta.hpp"
#include "Cell.hpp"
#include "UnitTest++.h"

namespace iblbm
{
SUITE(TestCore)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
// Initial values
static const auto g_relaxation_time = 0.8;

TEST(TestCell_Constructor_Default)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  Cell<double, descriptor::D2Q9Descriptor> cell;
  for (auto q = 0u; q < B::sQ; ++q) CHECK_CLOSE(0.0, cell[q], g_zero_tol);

  CHECK(cell.pGetDynamics() == nullptr);
}

TEST(TestCell_Constructor_WithDynamics)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  for (auto q = 0u; q < B::sQ; ++q) CHECK_CLOSE(0.0, cell[q], g_zero_tol);

  CHECK(cell.pGetDynamics() == &dynamics);
}
}
}  // namespace
