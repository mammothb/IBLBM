#include <array>
#include <chrono>
#include <numeric>
#include <vector>

#include "UnitTest++/UnitTest++.h"
#include "BgkDynamics.hpp"
#include "BulkMomenta.hpp"
#include "CoreUtilities.hpp"
#include "Descriptor.hpp"
#include "ForcedBgkDynamics.hpp"
#include "Helpers.hpp"
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
static const auto g_cell_density = 1.2;
static const std::vector<double> g_cell_velocity = {3.4, 5.6};
static const auto g_u_sqr = std::inner_product(g_cell_velocity.begin(),
      g_cell_velocity.end(), g_cell_velocity.begin(), 0.0);

//TEST(bench)
//{
//  std::array<double, 2> vec_1 = {1, 2};
//  using namespace std::chrono;
//  auto t1 = high_resolution_clock::now();
//  for (auto i = 0; i < 10000; ++i)
//      for (auto it : vec_1) auto tmp = it;
//  auto t2 = high_resolution_clock::now();
//  auto duration = duration_cast<microseconds>(t2 - t1).count();
//  std::cout << duration << std::endl;
//
//  std::array<double, 2> vec_2 = {1, 2};
//  t1 = high_resolution_clock::now();
//  for (auto i = 0; i < 1000000; ++i)
//      for (auto& rIt : vec_2) rIt += 2.3;
//  t2 = high_resolution_clock::now();
//  duration = duration_cast<microseconds>(t2 - t1).count();
//  std::cout << duration << std::endl;
//
//  std::array<double, 2> vec_3 = {1, 2};
//  t1 = high_resolution_clock::now();
//  for (auto i = 0; i < 1000000; ++i)
//      for (gsl::index j = 0; j < 2; ++j) vec_3[j] += 2.3;
//  t2 = high_resolution_clock::now();
//  duration = duration_cast<microseconds>(t2 - t1).count();
//  std::cout << duration << std::endl;
//
//  std::vector<double> vec_4(10000, 1.2);
//  t1 = high_resolution_clock::now();
//  for (auto i = 0; i < 10000; ++i)
//      for (gsl::index j = 0; j < 10000; ++j) auto tmp = vec_4[j];
//  t2 = high_resolution_clock::now();
//  duration = duration_cast<microseconds>(t2 - t1).count();
//  std::cout << duration << std::endl;
//}

TEST(TestLbmHelpers_ComputeEquilibrium)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;

  for (gsl::index q = 0; q < B::sQ; ++q) {
    auto e_dot_u = util::InnerProduct(B::sE[q], g_cell_velocity);
    // Original code
//    auto exp_feq = g_cell_density * B::sWeight[q] * (1.0 + B::sInvCsSqr *
//        e_dot_u + B::sInvCsSqr * B::sInvCsSqr / 2.0 * e_dot_u * e_dot_u -
//        B::sInvCsSqr / 2.0 * g_u_sqr) - B::sWeight[q];
    auto exp_feq = g_cell_density * B::sWeight[q] * (1.0 + B::sInvCsSqr *
        e_dot_u + B::sInvCsSqr * B::sInvCsSqr / 2.0 * e_dot_u * e_dot_u -
        B::sInvCsSqr / 2.0 * g_u_sqr);
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
  for (gsl::index q = 0; q < B::sQ; ++q) {
    cell[q] = q + 1.0;
    exp_cell[q] = q + 1.0;
  }
  auto omega = 1.0 / g_relaxation_time;
  auto u_sqr = LbmHelper<double, descriptor::D2Q9Descriptor>::
      BgkCollide(cell, g_cell_density, g_cell_velocity, omega);

  CHECK_CLOSE(g_u_sqr, u_sqr, g_loose_tol);
  for (gsl::index q = 0; q < B::sQ; ++q) {
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
  for (gsl::index q = 0; q < B::sQ; ++q) {
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
  for (gsl::index q = 0; q < B::sQ; ++q) {
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

TEST(TestLbmHelpers_AddExternalForce)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  typedef descriptor::ForcedD2Q9Descriptor<double> D;
  BulkMomenta<double, descriptor::ForcedD2Q9Descriptor> bulk_momenta;
  ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::ForcedD2Q9Descriptor> cell(&dynamics);
  std::vector<double> exp_cell(B::sQ, 0.0);

  auto omega = 1.0 / g_relaxation_time;
  auto rho = 1.0;
  std::vector<double> actual_u(B::sD, 0.0);
  std::vector<double> exp_u(B::sD, 0.0);
  for (gsl::index q = 0; q < B::sQ; ++q) {
    cell[q] = q;
    exp_cell[q] = q;
    rho += static_cast<double>(q);
  }
  auto offset = 0u;
  std::vector<double> force(D::ExternalField::sNumScalars);
  std::iota(force.begin(), force.end(), 1.0);

  cell.SetExternalField(offset, force);
  LbmHelper<double, descriptor::ForcedD2Q9Descriptor>::AddExternalForce(cell,
      g_cell_velocity, omega, rho);

  // Calculate expected DF after adding force
  for (gsl::index q = 0; q < B::sQ; ++q) {
    auto e_dot_u = std::inner_product(B::sE[q].begin(), B::sE[q].end(),
        g_cell_velocity.begin(), 0.0);
    e_dot_u *= B::sInvCsSqr * B::sInvCsSqr;
    auto force_term = 0.0;
    for (gsl::index d = 0; d < B::sD; ++d) {
      force_term += ((B::sE[q][d] - g_cell_velocity[d]) * B::sInvCsSqr +
          e_dot_u * B::sE[q][d]) * force[d];
    }
    force_term *= B::sWeight[q];
    force_term *= 1.0 - omega / 2.0;
    force_term *= rho;
    exp_cell[q] += force_term;

    CHECK_CLOSE(exp_cell[q], cell[q], g_loose_tol);
  }
}
}
}  // namespace iblbm
