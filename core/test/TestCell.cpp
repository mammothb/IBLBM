#include <cassert>
#include <iostream>
#include <numeric>

#include "UnitTest++/UnitTest++.h"
#include "BgkDynamics.hpp"
#include "BulkMomenta.hpp"
#include "Cell.hpp"
#include "ForcedBgkDynamics.hpp"

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

  for (gsl::index q = 0; q < B::sQ; ++q) CHECK_CLOSE(0.0, cell[q], g_zero_tol);

  CHECK(cell.pGetDynamics() == nullptr);
}

TEST(TestCell_Constructor_WithDynamics)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);

  for (gsl::index q = 0; q < B::sQ; ++q)
      CHECK_CLOSE(0.0, cell[q], g_zero_tol);

  CHECK(cell.pGetDynamics() == &dynamics);
}

TEST(TestCell_Constructor_WithForcedDynamics)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  typedef descriptor::ForcedD2Q9Descriptor<double> D;

  BulkMomenta<double, descriptor::ForcedD2Q9Descriptor> bulk_momenta;
  ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::ForcedD2Q9Descriptor> cell(&dynamics);
  const auto const_cell = cell;

  for (gsl::index q = 0; q < B::sQ; ++q) CHECK_CLOSE(0.0, cell[q], g_zero_tol);
  for (auto i = 0u; i < D::ExternalField::sNumScalars; ++i) {
    const auto external = const_cell.pGetExternal(
        D::ExternalField::sForceOffset)[i];
    CHECK_CLOSE(0.0, external, g_zero_tol);
    CHECK_CLOSE(0.0, cell.pGetExternal(D::ExternalField::sForceOffset)[i],
        g_zero_tol);
  }

  CHECK(cell.pGetDynamics() == &dynamics);
}

TEST(TestCell_SetExternalField)
{
  typedef descriptor::ForcedD2Q9Descriptor<double> D;

  BulkMomenta<double, descriptor::ForcedD2Q9Descriptor> bulk_momenta;
  ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::ForcedD2Q9Descriptor> cell(&dynamics);

  gsl::index offset {0};
  for (gsl::index i {0}; i < D::ExternalField::sNumScalars; ++i) {
    cell.pGetExternal(D::ExternalField::sForceOffset)[i] =
        static_cast<double>(i) + 1.0;
  }
  std::vector<double> exp_force(D::ExternalField::sNumScalars);
  std::iota(exp_force.begin(), exp_force.end(), 1.0);

  for (auto i = 0u; i < D::ExternalField::sNumScalars; ++i) {
    CHECK_CLOSE(exp_force[i], cell.pGetExternal(
        D::ExternalField::sForceOffset)[i], g_zero_tol);
  }

  CHECK(cell.pGetDynamics() == &dynamics);
}
}
}  // namespace iblbm
