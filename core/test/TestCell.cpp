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
  CHECK(std::as_const(cell).pGetDynamics() == nullptr);
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
  CHECK(std::as_const(cell).pGetDynamics() == &dynamics);
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
  CHECK(std::as_const(cell).pGetDynamics() == &dynamics);
}

TEST(TestCell_SetExternalField)
{
  typedef descriptor::ForcedD2Q9Descriptor<double> D;

  BulkMomenta<double, descriptor::ForcedD2Q9Descriptor> bulk_momenta;
  ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::ForcedD2Q9Descriptor> cell(&dynamics);

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

TEST(TestCell_GetNumBlock)
{
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);

  CHECK_EQUAL(1, cell.GetNumBlock());
}

TEST(TestCell_GetNumBlock_WithForcedDynamics)
{
  BulkMomenta<double, descriptor::ForcedD2Q9Descriptor> bulk_momenta;
  ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::ForcedD2Q9Descriptor> cell(&dynamics);

  CHECK_EQUAL(2, cell.GetNumBlock());
}

TEST(TestCell_GetSerializableSize)
{
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);

  auto serializable_size {9 * sizeof(double)};
  CHECK_EQUAL(serializable_size, cell.GetSerializableSize());
}

TEST(TestCell_GetSerializableSize_WithForcedDynamics)
{
  typedef descriptor::ForcedD2Q9Descriptor<double> D;
  BulkMomenta<double, descriptor::ForcedD2Q9Descriptor> bulk_momenta;
  ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::ForcedD2Q9Descriptor> cell(&dynamics);

  auto serializable_size {9 * sizeof(double) +
      D::ExternalField::sNumScalars * sizeof(double)};
  CHECK_EQUAL(serializable_size, cell.GetSerializableSize());
}

TEST(TestCell_pGetBlock)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);

  for (gsl::index q {0}; q < B::sQ; ++q) cell[q] = static_cast<double>(q);
  gsl::index block_index {0};
  bool* p_data {nullptr};
  std::size_t block_size {0};
  p_data = cell.pGetBlock(block_index, block_size);
  for (gsl::index q {0}; q < B::sQ; ++q) {
    CHECK_CLOSE(static_cast<double>(q), reinterpret_cast<double*>(p_data)[q],
        g_zero_tol);
  }
}

TEST(TestCell_pGetBlock_WithForcedDynamics)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  typedef descriptor::ForcedD2Q9Descriptor<double> D;
  BulkMomenta<double, descriptor::ForcedD2Q9Descriptor> bulk_momenta;
  ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::ForcedD2Q9Descriptor> cell(&dynamics);

  for (gsl::index q {0}; q < B::sQ; ++q) cell[q] = static_cast<double>(q);
  for (gsl::index i {0}; i < D::ExternalField::sNumScalars; ++i) {
    cell.pGetExternal(D::ExternalField::sForceOffset)[i] =
        static_cast<double>(i) + 1.0;
  }
  gsl::index block_index {0};
  bool* p_data {nullptr};
  std::size_t block_size {0};
  p_data = cell.pGetBlock(block_index, block_size);
  for (gsl::index q {0}; q < B::sQ; ++q) {
    CHECK_CLOSE(static_cast<double>(q), reinterpret_cast<double*>(p_data)[q],
        g_zero_tol);
  }
  CHECK_EQUAL(B::sQ * sizeof(double), block_size);
  ++block_index;
  p_data = cell.pGetBlock(block_index, block_size);
  for (gsl::index i {0}; i < D::ExternalField::sNumScalars; ++i) {
    CHECK_CLOSE(static_cast<double>(i) + 1.0,
        reinterpret_cast<double*>(p_data)[i], g_zero_tol);
  }
  CHECK_EQUAL(D::ExternalField::sNumScalars * sizeof(double), block_size);
}
}
}  // namespace iblbm
