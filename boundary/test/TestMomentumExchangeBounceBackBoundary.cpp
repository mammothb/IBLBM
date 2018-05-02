#include "UnitTest++/UnitTest++.h"
#include "BgkDynamics.hpp"
#include "BulkMomenta.hpp"
#include "Cell.hpp"
#include "Descriptor.hpp"
#include "LatticeStatistics.hpp"
#include "MomentumExchangeBounceBackBoundary.hpp"

namespace iblbm
{
SUITE(TestBoundary)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
// Initial values
static const auto g_relaxation_time = 0.8;
static const auto g_cell_density = 1.2;

TEST(TestMomentumExchangeBounceBackBoundary_Constructor_Default)
{
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  MomentumExchangeBounceBackBoundary<double, descriptor::D2Q9Descriptor>
      bounce_back;

  CHECK_CLOSE(0.0, bounce_back.ComputeRho(cell), g_zero_tol);
  CHECK_CLOSE(0.0, bounce_back.GetOmega(), g_zero_tol);
}

TEST(TestMomentumExchangeBounceBackBoundary_Constructor_UserDefined)
{
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  MomentumExchangeBounceBackBoundary<double, descriptor::D2Q9Descriptor>
      bounce_back(g_cell_density);

  CHECK_CLOSE(g_cell_density, bounce_back.ComputeRho(cell), g_zero_tol);
  CHECK_CLOSE(0.0, bounce_back.GetOmega(), g_zero_tol);
}

TEST(TestMomentumExchangeBounceBackBoundary_Collide)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  LatticeStatistics<double> stats;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  MomentumExchangeBounceBackBoundary<double, descriptor::D2Q9Descriptor>
      bounce_back;
  std::vector<double> exp_cell(B::sQ);

  for (gsl::index q = 0; q < B::sQ; ++q) {
    cell[q] = q + 1;
    exp_cell[B::sOpposite[q]] = q + 1;
  }
  bounce_back.Collide(cell, stats);

  for (gsl::index q = 0; q < B::sQ; ++q)
      CHECK_CLOSE(exp_cell[q], cell[q], g_zero_tol);
}

TEST(TestMomentumExchangeBounceBackBoundary_ComputeU)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  MomentumExchangeBounceBackBoundary<double, descriptor::D2Q9Descriptor>
      bounce_back;
  std::vector<double> u(B::sQ);

  bounce_back.ComputeU(cell, u);

  for (const auto& r_it : u) CHECK_CLOSE(0.0, r_it, g_zero_tol);
}

TEST(TestMomentumExchangeBounceBackBoundary_ComputeRhoAndU)
{
  typedef descriptor::D2Q9DescriptorBase<double> B;
  BulkMomenta<double, descriptor::D2Q9Descriptor> bulk_momenta;
  BgkDynamics<double, descriptor::D2Q9Descriptor> dynamics(g_relaxation_time,
      bulk_momenta);
  Cell<double, descriptor::D2Q9Descriptor> cell(&dynamics);
  MomentumExchangeBounceBackBoundary<double, descriptor::D2Q9Descriptor>
      bounce_back(g_cell_density);
  std::vector<double> u(B::sQ);

  double density {0};
  bounce_back.ComputeRhoAndU(cell, density, u);

  CHECK_CLOSE(g_cell_density, density, g_zero_tol);
  for (const auto& r_it : u) CHECK_CLOSE(0.0, r_it, g_zero_tol);
}
}
}  // namespace iblbm
