#include <iostream>
#include <numeric>

#include "BulkMomenta.hpp"
#include "Cell.hpp"
#include "ForcedBgkDynamics.hpp"
#include "UnitTestCustomUtilities.hpp"
#include "UnitTest++/UnitTest++.h"

namespace iblbm
{
/**
 * Tester class to access private members
 */
class TestLatticeStatistics
{
 public:
  std::vector<double> GetInternalAverageValues(
      const LatticeStatistics<double>& rStats)
  {
    return rStats.mInternalAverageValues;
  }

  std::vector<double> GetInternalMaxValues(
      const LatticeStatistics<double>& rStats)
  {
    return rStats.mInternalMaxValues;
  }

  std::size_t GetInternalNumCells(const LatticeStatistics<double>& rStats)
  {
    return rStats.mInternalNumCells;
  }
};

SUITE(TestDynamics)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
// Initial values
static const auto g_relaxation_time = 0.8;
static const auto g_cell_density = 1.2;
static const std::vector<double> g_cell_velocity = {3.4, 5.6};

TEST(TestForcedBgkDynamics_Constructor)
{
  BulkMomenta<double, descriptor::ForcedD2Q9Descriptor> bulk_momenta;
  ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);

  CHECK_CLOSE(1.0 / g_relaxation_time, dynamics.GetOmega(), g_loose_tol);
}

//TEST(TestForcedBgkDynamics_Collide)
//{
//  typedef descriptor::D2Q9DescriptorBase<double> B;
//  LatticeStatistics<double> stats;
//  TestLatticeStatistics tester;
//  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
//  ForcedBgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
//      bulk_momenta);
//  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
//  for (gsl::index q = 0; q < B::sQ; ++q) cell[q] = q + 1;
//  dynamics.Collide(cell, stats);
//
//  // Expected result
//  auto omega = 1.0 / g_relaxation_time;
//  std::vector<double> exp_cell(B::sQ);
//  std::iota(exp_cell.begin(), exp_cell.end(), 1.0);
//  auto density = 1.0;
//  std::vector<double> velocity(B::sD, 0.0);
//  // Compute fluid velocity
//  for (gsl::index q = 0; q < B::sQ; ++q) {
//    density += exp_cell[q];
//    for (gsl::index d = 0; d < B::sD; ++d)
//        velocity[d] += exp_cell[q] * B::sE[q][d];
//  }
//  for (gsl::index d = 0; d < B::sD; ++d) velocity[d] /= density;
//  // Calculate convenience variable
//  auto u_sqr = std::inner_product(velocity.begin(), velocity.end(),
//      velocity.begin(), 0.0);
//  // Calculate equilibrium
//  for (gsl::index q = 0; q < B::sQ; ++q) {
//    exp_cell[q] *= 1.0 - omega;
//    auto e_dot_u = 0.0;
//    for (gsl::index d = 0; d < B::sD; ++d) e_dot_u += B::sE[q][d] * velocity[d];
////    // Original code
////    exp_cell[q] += omega * (density * B::sWeight[q] * (1.0 + B::sInvCsSqr *
////        e_dot_u + B::sInvCsSqr * B::sInvCsSqr * 0.5 * e_dot_u * e_dot_u -
////        B::sInvCsSqr * 0.5 * u_sqr) - B::sWeight[q]);
//    exp_cell[q] += omega * density * B::sWeight[q] * (1.0 + B::sInvCsSqr *
//        e_dot_u + B::sInvCsSqr * B::sInvCsSqr * 0.5 * e_dot_u * e_dot_u -
//        B::sInvCsSqr * 0.5 * u_sqr);
//  }
//
//  for (gsl::index q = 0; q < B::sQ; ++q)
//      CHECK_CLOSE(exp_cell[q], cell[q], g_loose_tol);
//
//  auto avg_values = tester.GetInternalAverageValues(stats);
//  CHECK_CLOSE(density, avg_values[stats.AVG_RHO], g_zero_tol);
//  CHECK_CLOSE(u_sqr, avg_values[stats.AVG_ENERGY], g_zero_tol);
//
//  auto max_values = tester.GetInternalMaxValues(stats);
//  CHECK_CLOSE(u_sqr, max_values[stats.MAX_U], g_zero_tol);
//
//  CHECK_EQUAL(1u, tester.GetInternalNumCells(stats));
//}
//
//TEST(TestForcedBgkDynamics_ComputeEquilibrium)
//{
//  typedef descriptor::D2Q9DescriptorBase<double> B;
//  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
//  ForcedBgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
//      bulk_momenta);
//
//  auto u_sqr = g_cell_velocity[0] * g_cell_velocity[0] + g_cell_velocity[1] *
//      g_cell_velocity[1];
//  for (gsl::index q = 0; q < B::sQ; ++q) {
//    auto e_dot_u = B::sE[q][0] * g_cell_velocity[0] + B::sE[q][1] *
//        g_cell_velocity[1];
////    // Original code
////    auto exp_feq = g_cell_density * B::sWeight[q] * (1.0 + B::sInvCsSqr *
////        e_dot_u + B::sInvCsSqr * B::sInvCsSqr / 2.0 * e_dot_u * e_dot_u -
////        B::sInvCsSqr / 2.0 * u_sqr) - B::sWeight[q];
//    auto exp_feq = g_cell_density * B::sWeight[q] * (1.0 + B::sInvCsSqr *
//        e_dot_u + B::sInvCsSqr * B::sInvCsSqr / 2.0 * e_dot_u * e_dot_u -
//        B::sInvCsSqr / 2.0 * u_sqr);
//    auto actual_feq = dynamics.ComputeEquilibrium(q, g_cell_density,
//        g_cell_velocity, u_sqr);
//
//    CHECK_CLOSE(exp_feq, actual_feq, g_loose_tol);
//  }
//}
//
//TEST(TestForcedBgkDynamics_InitializeEquilibrium)
//{
//  typedef descriptor::D2Q9DescriptorBase<double> B;
//  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
//  ForcedBgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
//      bulk_momenta);
//  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
//
//  dynamics.InitializeEquilibrium(cell, g_cell_density, g_cell_velocity);
//
//  auto u_sqr = g_cell_velocity[0] * g_cell_velocity[0] + g_cell_velocity[1] *
//      g_cell_velocity[1];
//  for (gsl::index q = 0; q < B::sQ; ++q) {
//    auto e_dot_u = B::sE[q][0] * g_cell_velocity[0] + B::sE[q][1] *
//        g_cell_velocity[1];
////    // Original code
////    auto exp_feq = g_cell_density * B::sWeight[q] * (1.0 + B::sInvCsSqr *
////        e_dot_u + B::sInvCsSqr * B::sInvCsSqr / 2.0 * e_dot_u * e_dot_u -
////        B::sInvCsSqr / 2.0 * u_sqr) - B::sWeight[q];
//    auto exp_feq = g_cell_density * B::sWeight[q] * (1.0 + B::sInvCsSqr *
//        e_dot_u + B::sInvCsSqr * B::sInvCsSqr / 2.0 * e_dot_u * e_dot_u -
//        B::sInvCsSqr / 2.0 * u_sqr);
//
//    CHECK_CLOSE(exp_feq, cell[q], g_loose_tol);
//  }
//}

TEST(TestForcedBgkDynamics_ComputeRho)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::ForcedD2Q9Descriptor> bulk_momenta;
  ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::ForcedD2Q9Descriptor> cell(&dynamics);
  auto exp_rho = 1.0;
  for (gsl::index q = 0; q < B::sQ; ++q) {
    cell[q] = q;
    exp_rho += static_cast<double>(q);
  }

  CHECK_CLOSE(exp_rho, dynamics.ComputeRho(cell), g_zero_tol);
}

TEST(TestForcedBgkDynamics_ComputeU)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  typedef descriptor::ForcedD2Q9Descriptor<double> D;
  BulkMomenta<double, descriptor::ForcedD2Q9Descriptor> bulk_momenta;
  ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::ForcedD2Q9Descriptor> cell(&dynamics);

  auto rho = 1.0;
  std::vector<double> actual_u(B::sD, 0.0);
  std::vector<double> exp_u(B::sD, 0.0);

  for (gsl::index q = 0; q < B::sQ; ++q) {
    cell[q] = q;
    rho += static_cast<double>(q);
  }
  auto offset = 0u;
  std::vector<double> force(D::ExternalField::sNumScalars);
  std::iota(force.begin(), force.end(), 1.0);

  cell.SetExternalField(offset, force);
  dynamics.ComputeU(cell, actual_u);

  exp_u[0] = ((cell[5] + cell[6] + cell[7]) - (cell[1] + cell[2] +
      cell[3])) / rho + force[0] / 2.0;
  exp_u[1] = ((cell[7] + cell[8] + cell[1]) - (cell[3] + cell[4] +
      cell[5])) / rho + force[1] / 2.0;

  CHECK(testutil::CheckCloseVector(actual_u, exp_u, g_loose_tol));
}

TEST(TestForcedBgkDynamics_ComputeRhoAndU)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  typedef descriptor::ForcedD2Q9Descriptor<double> D;
  BulkMomenta<double, descriptor::ForcedD2Q9Descriptor> bulk_momenta;
  ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor> dynamics(
      g_relaxation_time, bulk_momenta);
  Cell<double, descriptor::ForcedD2Q9Descriptor> cell(&dynamics);
  auto actual_rho = 0.0;
  auto exp_rho = 1.0;
  std::vector<double> actual_u(B::sD, 0.0);
  std::vector<double> exp_u(B::sD, 0.0);

  for (gsl::index q = 0; q < B::sQ; ++q) {
    cell[q] = q;
    exp_rho += static_cast<double>(q);
  }
  auto offset = 0u;
  std::vector<double> force(D::ExternalField::sNumScalars);
  std::iota(force.begin(), force.end(), 1.0);

  cell.SetExternalField(offset, force);
  dynamics.ComputeRhoAndU(cell, actual_rho, actual_u);

  exp_u[0] = ((cell[5] + cell[6] + cell[7]) - (cell[1] + cell[2] +
      cell[3])) / exp_rho + force[0] / 2.0;
  exp_u[1] = ((cell[7] + cell[8] + cell[1]) - (cell[3] + cell[4] +
      cell[5])) / exp_rho + force[1] / 2.0;

  CHECK_CLOSE(exp_rho, actual_rho, g_loose_tol);
  CHECK(testutil::CheckCloseVector(actual_u, exp_u, g_loose_tol));
}
}
}  // namespace iblbm
