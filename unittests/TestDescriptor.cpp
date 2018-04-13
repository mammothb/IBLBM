#include <iostream>

#include "UnitTest++/UnitTest++.h"
#include "Descriptor.hpp"
#include "UnitTestCustomUtilities.hpp"

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
  CHECK_EQUAL(0u, NoExternalField::sForceOffset);
  CHECK_EQUAL(0u, NoExternalField::sSizeOfForce);
  CHECK_EQUAL(0u, NoExternalField::sVelocityOffset);
  CHECK_EQUAL(0u, NoExternalField::sSizeOfVelocity);
}

TEST(TestDescriptor_NoExternalFieldBase)
{
  CHECK_EQUAL(0u, NoExternalFieldBase::ExternalField::sNumScalars);
  CHECK_EQUAL(0u, NoExternalFieldBase::ExternalField::sNumSpecies);
  CHECK_EQUAL(0u, NoExternalFieldBase::ExternalField::sForceOffset);
  CHECK_EQUAL(0u, NoExternalFieldBase::ExternalField::sSizeOfForce);
  CHECK_EQUAL(0u, NoExternalFieldBase::ExternalField::sVelocityOffset);
  CHECK_EQUAL(0u, NoExternalFieldBase::ExternalField::sSizeOfVelocity);
}

TEST(TestDescriptor_Force2dDescriptor)
{
  CHECK_EQUAL(2u, Force2dDescriptor::sNumScalars);
  CHECK_EQUAL(1u, Force2dDescriptor::sNumSpecies);
  CHECK_EQUAL(0u, Force2dDescriptor::sForceOffset);
  CHECK_EQUAL(2u, Force2dDescriptor::sSizeOfForce);
}

TEST(TestDescriptor_Force2dDescriptorBase)
{
  CHECK_EQUAL(2u, Force2dDescriptorBase::ExternalField::sNumScalars);
  CHECK_EQUAL(1u, Force2dDescriptorBase::ExternalField::sNumSpecies);
  CHECK_EQUAL(0u, Force2dDescriptorBase::ExternalField::sForceOffset);
  CHECK_EQUAL(2u, Force2dDescriptorBase::ExternalField::sSizeOfForce);
}

TEST(TestDescriptor_ExternalFieldArray)
{
  ExternalFieldArray<double, Force2dDescriptor> field_vec;
  std::vector<double> exp_data = {1, 2};
  for (gsl::index i {0}; i < exp_data.size(); ++i)
      field_vec[0][i] = exp_data[i];
  for (auto i = 0u; i < exp_data.size(); ++i)
      CHECK_CLOSE(exp_data[i], field_vec[0][i], g_zero_tol);
}

TEST(TestDescriptor_D2Q9DescriptorBase)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  const auto actual_e = B::sE;
  std::vector<std::vector<double>> exp_e = {
      {0, 0},
      {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}
  };
  std::vector<gsl::index> exp_opposite = {
      0, 5, 6, 7, 8, 1, 2, 3, 4
  };
  std::vector<double> exp_weight = {
      4.0 / 9.0,
      1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0, 1.0 / 9.0,
      1.0 / 36.0, 1.0 / 9.0
  };
  CHECK_EQUAL(2u, B::sD);
  CHECK_EQUAL(9u, B::sQ);
  CHECK_EQUAL(1u, B::sVicinity);
  for (gsl::index q = 0; q < B::sQ; ++q)
      CHECK(testutil::CheckCloseVector(actual_e[q], exp_e[q], g_zero_tol));
  CHECK(testutil::CheckEqualVector(B::sOpposite, exp_opposite));
  CHECK(testutil::CheckCloseVector(B::sWeight, exp_weight, g_zero_tol));
  CHECK_CLOSE(3.0, B::sInvCsSqr, g_zero_tol);

  // Check that lattice directions follows the requirements
  const auto nq = B::sQ;
  CHECK_CLOSE(0.0, actual_e[0][0], g_zero_tol);
  CHECK_CLOSE(0.0, actual_e[0][1], g_zero_tol);
  for (auto q = 1u; q <= (nq - 1) / 2; ++q) {
    CHECK(std::abs(actual_e[q][0] + actual_e[q + (nq - 1) / 2][0]) <
        g_zero_tol &&
        std::abs(actual_e[q][1] + actual_e[q + (nq - 1) / 2][1]) <
        g_zero_tol);
    CHECK(actual_e[q][0] < 0.0 || (actual_e[q][0] < g_zero_tol &&
        actual_e[q][1] < 0.0));
  }
}

TEST(TestDescriptor_D2Q9Descriptor)
{
  typedef descriptor::D2Q9Descriptor<double>::BaseDescriptor B;
  typedef descriptor::D2Q9Descriptor<double> D;
  const auto actual_e = B::sE;
  std::vector<std::vector<double>> exp_e = {
      {0, 0},
      {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}
  };
  std::vector<gsl::index> exp_opposite = {
      0, 5, 6, 7, 8, 1, 2, 3, 4
  };
  std::vector<double> exp_weight = {
      4.0 / 9.0,
      1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0, 1.0 / 9.0,
      1.0 / 36.0, 1.0 / 9.0
  };
  CHECK_EQUAL(2u, B::sD);
  CHECK_EQUAL(9u, B::sQ);
  CHECK_EQUAL(1u, B::sVicinity);
  for (gsl::index q = 0; q < B::sQ; ++q)
      CHECK(testutil::CheckCloseVector(actual_e[q], exp_e[q], g_zero_tol));
  CHECK(testutil::CheckEqualVector(B::sOpposite, exp_opposite));
  CHECK(testutil::CheckCloseVector(B::sWeight, exp_weight, g_zero_tol));
  CHECK_CLOSE(3.0, B::sInvCsSqr, g_zero_tol);

  CHECK_EQUAL(0u, D::ExternalField::sNumScalars);
  CHECK_EQUAL(0u, D::ExternalField::sNumSpecies);
  CHECK_EQUAL(0u, D::ExternalField::sForceOffset);
  CHECK_EQUAL(0u, D::ExternalField::sSizeOfForce);
  CHECK_EQUAL(0u, D::ExternalField::sVelocityOffset);
  CHECK_EQUAL(0u, D::ExternalField::sSizeOfVelocity);

  // Check that lattice directions follows the requirements
  const auto nq = B::sQ;
  CHECK_CLOSE(0.0, actual_e[0][0], g_zero_tol);
  CHECK_CLOSE(0.0, actual_e[0][1], g_zero_tol);
  for (auto q = 1u; q <= (nq - 1) / 2; ++q) {
    CHECK(std::abs(actual_e[q][0] + actual_e[q + (nq - 1) / 2][0]) <
        g_zero_tol &&
        std::abs(actual_e[q][1] + actual_e[q + (nq - 1) / 2][1]) <
        g_zero_tol);
    CHECK(actual_e[q][0] < 0.0 || (actual_e[q][0] < g_zero_tol &&
        actual_e[q][1] < 0.0));
  }
}

TEST(TestDescriptor_ForcedD2Q9Descriptor)
{
  typedef descriptor::ForcedD2Q9Descriptor<double>::BaseDescriptor B;
  typedef descriptor::ForcedD2Q9Descriptor<double> D;
  const auto actual_e = B::sE;
  std::vector<std::vector<double>> exp_e = {
      {0, 0},
      {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}
  };
  std::vector<gsl::index> exp_opposite = {
      0, 5, 6, 7, 8, 1, 2, 3, 4
  };
  std::vector<double> exp_weight = {
      4.0 / 9.0,
      1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0, 1.0 / 9.0, 1.0 / 36.0, 1.0 / 9.0,
      1.0 / 36.0, 1.0 / 9.0
  };
  CHECK_EQUAL(2u, B::sD);
  CHECK_EQUAL(9u, B::sQ);
  CHECK_EQUAL(1u, B::sVicinity);
  for (gsl::index q = 0; q < B::sQ; ++q)
      CHECK(testutil::CheckCloseVector(actual_e[q], exp_e[q], g_zero_tol));
  CHECK(testutil::CheckEqualVector(B::sOpposite, exp_opposite));
  CHECK(testutil::CheckCloseVector(B::sWeight, exp_weight, g_zero_tol));
  CHECK_CLOSE(3.0, B::sInvCsSqr, g_zero_tol);

  CHECK_EQUAL(2u, D::ExternalField::sNumScalars);
  CHECK_EQUAL(1u, D::ExternalField::sNumSpecies);
  CHECK_EQUAL(0u, D::ExternalField::sForceOffset);
  CHECK_EQUAL(2u, D::ExternalField::sSizeOfForce);

  // Check that lattice directions follows the requirements
  const auto nq = B::sQ;
  CHECK_CLOSE(0.0, actual_e[0][0], g_zero_tol);
  CHECK_CLOSE(0.0, actual_e[0][1], g_zero_tol);
  for (auto q = 1u; q <= (nq - 1) / 2; ++q) {
    CHECK(std::abs(actual_e[q][0] + actual_e[q + (nq - 1) / 2][0]) <
        g_zero_tol &&
        std::abs(actual_e[q][1] + actual_e[q + (nq - 1) / 2][1]) <
        g_zero_tol);
    CHECK(actual_e[q][0] < 0.0 || (actual_e[q][0] < g_zero_tol &&
        actual_e[q][1] < 0.0));
  }
}
}
}  // namespace descriptor
}  // namespace iblbm
