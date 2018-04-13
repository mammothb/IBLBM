#include <iostream>

#include "UnitTest++/UnitTest++.h"
#include "LatticeStatistics.hpp"

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

SUITE(TestCore)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;

TEST(TestLatticeStatistics_Constructor)
{
  LatticeStatistics<double> stats;
  TestLatticeStatistics tester;

  auto avg_values = tester.GetInternalAverageValues(stats);
  for (auto it : avg_values) CHECK_CLOSE(0.0, it, g_zero_tol);

  auto max_values = tester.GetInternalMaxValues(stats);
  for (auto it : max_values) CHECK_CLOSE(0.0, it, g_zero_tol);

  CHECK_EQUAL(0u, tester.GetInternalNumCells(stats));
}

TEST(TestLatticeStatistics_IncrementStats)
{
  LatticeStatistics<double> stats;
  TestLatticeStatistics tester;

  auto rho = 1.2;
  auto u_sqr = 3.4;
  auto avg_rho = rho;
  auto avg_energy = u_sqr;

  stats.IncrementStats(rho, u_sqr);

  auto avg_values = tester.GetInternalAverageValues(stats);
  CHECK_CLOSE(avg_rho, avg_values[stats.AVG_RHO], g_zero_tol);
  CHECK_CLOSE(avg_energy, avg_values[stats.AVG_ENERGY], g_zero_tol);

  auto max_values = tester.GetInternalMaxValues(stats);
  CHECK_CLOSE(u_sqr, max_values[stats.MAX_U], g_zero_tol);

  CHECK_EQUAL(1u, tester.GetInternalNumCells(stats));

  rho = 5.6;
  u_sqr = 7.8;
  avg_rho += rho;
  avg_energy += u_sqr;

  stats.IncrementStats(rho, u_sqr);

  avg_values = tester.GetInternalAverageValues(stats);
  CHECK_CLOSE(avg_rho, avg_values[stats.AVG_RHO], g_zero_tol);
  CHECK_CLOSE(avg_energy, avg_values[stats.AVG_ENERGY], g_zero_tol);

  max_values = tester.GetInternalMaxValues(stats);
  CHECK_CLOSE(u_sqr, max_values[stats.MAX_U], g_zero_tol);

  CHECK_EQUAL(2u, tester.GetInternalNumCells(stats));
}
}
}  // namespace iblbm
