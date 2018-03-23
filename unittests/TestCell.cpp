#include <iostream>

#include "AbstractDynamics.hpp"
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

TEST(TestCell_Constructor_Default)
{
  Cell<double, descriptor::D2Q9Descriptor> cell;
  for (gsl::index i = 0; i < descriptor::D2Q9Descriptor<double>::sQ; ++i)
      CHECK_CLOSE(0.0, cell[i], g_zero_tol);
  CHECK(cell.pGetDynamics() == nullptr);
}

TEST(TestCell_Constructor_WithDynamics)
{
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  AbstractDynamics<double, descriptor::D2Q9Descriptor> dynamics(bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  for (gsl::index i = 0; i < descriptor::D2Q9Descriptor<double>::sQ; ++i)
      CHECK_CLOSE(0.0, cell[i], g_zero_tol);
  CHECK(cell.pGetDynamics() == &dynamics);
}
}
}  // namespace
