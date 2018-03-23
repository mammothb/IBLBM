#include <algorithm>
#include <chrono>
#include <numeric>

#include "IblbmUtilities.hpp"
#include "UnitTestCustomUtilities.hpp"
#include "UnitTest++.h"

namespace iblbm
{
SUITE(TestUtilities)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
static const auto g_num_dims = 2u;
static const auto g_num_dirs = 9u;

TEST(TestIblbmUtilites_InnerProduct)
{
  std::vector<double> vec_1 = {1.2, 3.4};
  std::vector<double> vec_2 = {5.6, 7.8};
  auto result = vec_1[0] * vec_2[0] + vec_1[1] * vec_2[1];

  CHECK_CLOSE(result, util::InnerProduct(vec_1, vec_2), g_loose_tol);
}

TEST(TestIblbmUtilites_GetZerothMoment)
{
  std::vector<double> vec_1(10);
  std::iota(vec_1.begin(), vec_1.end(), 0.0);
  auto result = std::accumulate(vec_1.begin(), vec_1.end(), 0.0);

  CHECK_CLOSE(result, util::GetZerothMoment(vec_1), g_loose_tol);
}

TEST(TestIblbmUtilites_GetFirstMoment)
{
  std::vector<double> vec_1(g_num_dirs);
  std::iota(vec_1.begin(), vec_1.end(), 0.0);
  std::vector<std::vector<double>> vec_2;
  for (auto i = 0.0; i < g_num_dirs; ++i) vec_2.push_back({i, i + 1});

  std::vector<double> result = {0.0, 0.0};
  for (auto d = 0u; d < g_num_dims; ++d) {
    for (auto i = 0u; i < g_num_dirs; ++i)
        result[d] += vec_1[i] * vec_2[i][d];
  }

  CHECK(testutil::CheckCloseVector(util::GetFirstMoment(vec_1, vec_2),
      result, g_loose_tol));
}
}
}
