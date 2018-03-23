#include <gsl/gsl>
#include <iostream>

#include "Descriptor.hpp"
#include "IblbmUtilities.hpp"
#include "UnitTestCustomUtilities.hpp"
#include "UnitTest++.h"

namespace iblbm
{
namespace descriptor
{
SUITE(TestDynamics)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;

TEST(TestDescriptor_NoExternalField)
{
  CHECK_EQUAL(0u, NoExternalField::sNumScalars);
  CHECK_EQUAL(0u, NoExternalField::sNumSpecies);
  CHECK_EQUAL(0, NoExternalField::sForceBeginsAt);
  CHECK_EQUAL(0u, NoExternalField::sSizeOfForce);
  CHECK_EQUAL(0, NoExternalField::sVelocityBeginsAt);
  CHECK_EQUAL(0u, NoExternalField::sSizeOfVelocity);
}

TEST(TestDescriptor_NoExternalFieldBase)
{
  CHECK_EQUAL(0u, NoExternalFieldBase::ExternalField::sNumScalars);
  CHECK_EQUAL(0u, NoExternalFieldBase::ExternalField::sNumSpecies);
  CHECK_EQUAL(0, NoExternalFieldBase::ExternalField::sForceBeginsAt);
  CHECK_EQUAL(0u, NoExternalFieldBase::ExternalField::sSizeOfForce);
  CHECK_EQUAL(0, NoExternalFieldBase::ExternalField::sVelocityBeginsAt);
  CHECK_EQUAL(0u, NoExternalFieldBase::ExternalField::sSizeOfVelocity);
}

TEST(TestDescriptor_Force2dDescriptor)
{
  CHECK_EQUAL(2u, Force2dDescriptor::sNumScalars);
  CHECK_EQUAL(1u, Force2dDescriptor::sNumSpecies);
  CHECK_EQUAL(0, Force2dDescriptor::sForceBeginsAt);
  CHECK_EQUAL(2u, Force2dDescriptor::sSizeOfForce);
}

TEST(TestDescriptor_Force2dDescriptorBase)
{
  CHECK_EQUAL(2u, Force2dDescriptorBase::ExternalField::sNumScalars);
  CHECK_EQUAL(1u, Force2dDescriptorBase::ExternalField::sNumSpecies);
  CHECK_EQUAL(0, Force2dDescriptorBase::ExternalField::sForceBeginsAt);
  CHECK_EQUAL(2u, Force2dDescriptorBase::ExternalField::sSizeOfForce);
}

TEST(TestDescriptor_D2Q9DescriptorBase)
{
  const auto actual_e = D2Q9DescriptorBase<double>::sE;
  std::vector<std::vector<double>> expected_e = {
      {0, 0},
      {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}
  };
  std::vector<std::size_t> expected_opposite = {
      0, 5, 6, 7, 8, 1, 2, 3, 4
  };
  std::vector<double> expected_weight = {
      4.0 / 9.0,
      1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0, 1.0 / 9.0,
      1.0 / 36.0, 1.0 / 9.0
  };
  CHECK_EQUAL(2u, D2Q9DescriptorBase<double>::sD);
  CHECK_EQUAL(9u, D2Q9DescriptorBase<double>::sQ);
  CHECK_EQUAL(1u, D2Q9DescriptorBase<double>::sVicinity);
  for (gsl::index i = 0; i < D2Q9DescriptorBase<double>::sQ; ++i) {
    CHECK(testutil::CheckCloseVector(actual_e[i], expected_e[i],
        g_zero_tol));
  }
  CHECK(testutil::CheckEqualVector(D2Q9DescriptorBase<double>::sOpposite,
      expected_opposite));
  CHECK(testutil::CheckCloseVector(D2Q9DescriptorBase<double>::sWeight,
      expected_weight, g_zero_tol));
  CHECK_CLOSE(3.0, D2Q9DescriptorBase<double>::sInvCsSqr, g_zero_tol);

  // Check that lattice directions follows the requirements
  const auto q = D2Q9DescriptorBase<double>::sQ;
  CHECK_CLOSE(0.0, actual_e[0][0], g_zero_tol);
  CHECK_CLOSE(0.0, actual_e[0][1], g_zero_tol);
  for (gsl::index i = 1; i <= (q - 1) / 2; ++i) {
    CHECK(std::abs(actual_e[i][0] + actual_e[i + (q - 1) / 2][0]) <
        g_zero_tol &&
        std::abs(actual_e[i][1] + actual_e[i + (q - 1) / 2][1]) <
        g_zero_tol);
    CHECK(actual_e[i][0] < 0.0 || (actual_e[i][0] < g_zero_tol &&
        actual_e[i][1] < 0.0));
  }
}

TEST(TestDescriptor_D2Q9Descriptor)
{
  const auto actual_e = D2Q9Descriptor<double>::BaseDescriptor::sE;
  std::vector<std::vector<double>> expected_e = {
      {0, 0},
      {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}
  };
  std::vector<std::size_t> expected_opposite = {
      0, 5, 6, 7, 8, 1, 2, 3, 4
  };
  std::vector<double> expected_weight = {
      4.0 / 9.0,
      1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0, 1.0 / 9.0,
      1.0 / 36.0, 1.0 / 9.0
  };
  CHECK_EQUAL(2u, D2Q9Descriptor<double>::BaseDescriptor::sD);
  CHECK_EQUAL(9u, D2Q9Descriptor<double>::BaseDescriptor::sQ);
  CHECK_EQUAL(1u, D2Q9Descriptor<double>::BaseDescriptor::sVicinity);
  for (gsl::index i = 0; i < D2Q9Descriptor<double>::BaseDescriptor::sQ;
      ++i) {
    CHECK(testutil::CheckCloseVector(actual_e[i], expected_e[i],
        g_zero_tol));
  }
  CHECK(testutil::CheckEqualVector(
      D2Q9Descriptor<double>::BaseDescriptor::sOpposite, expected_opposite));
  CHECK(testutil::CheckCloseVector(
      D2Q9Descriptor<double>::BaseDescriptor::sWeight, expected_weight,
      g_zero_tol));
  CHECK_CLOSE(3.0, D2Q9Descriptor<double>::BaseDescriptor::sInvCsSqr,
      g_zero_tol);

  CHECK_EQUAL(0u, D2Q9Descriptor<double>::ExternalField::sNumScalars);
  CHECK_EQUAL(0u, D2Q9Descriptor<double>::ExternalField::sNumSpecies);
  CHECK_EQUAL(0, D2Q9Descriptor<double>::ExternalField::sForceBeginsAt);
  CHECK_EQUAL(0u, D2Q9Descriptor<double>::ExternalField::sSizeOfForce);
  CHECK_EQUAL(0, D2Q9Descriptor<double>::ExternalField::sVelocityBeginsAt);
  CHECK_EQUAL(0u, D2Q9Descriptor<double>::ExternalField::sSizeOfVelocity);

  // Check that lattice directions follows the requirements
  const auto q = D2Q9Descriptor<double>::BaseDescriptor::sQ;
  CHECK_CLOSE(0.0, actual_e[0][0], g_zero_tol);
  CHECK_CLOSE(0.0, actual_e[0][1], g_zero_tol);
  for (gsl::index i = 1; i <= (q - 1) / 2; ++i) {
    CHECK(std::abs(actual_e[i][0] + actual_e[i + (q - 1) / 2][0]) <
        g_zero_tol &&
        std::abs(actual_e[i][1] + actual_e[i + (q - 1) / 2][1]) <
        g_zero_tol);
    CHECK(actual_e[i][0] < 0.0 || (actual_e[i][0] < g_zero_tol &&
        actual_e[i][1] < 0.0));
  }
}
}
}  // namespace descriptor
}  // namespace iblbm
