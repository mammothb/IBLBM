#include <iostream>
#include <vector>

#include "UnitTest++/UnitTest++.h"
#include "Vector.hpp"

namespace iblbm
{
SUITE(TestCore)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;

TEST(TestVector_Constructor_Defaul)
{
  Vector<double, 2> vec;

  CHECK_CLOSE(0.0, vec[0], g_zero_tol);
  CHECK_CLOSE(0.0, vec[1], g_zero_tol);
}

TEST(TestVector_Constructor_Scalar)
{
  auto scalar = 1.2;
  Vector<double, 2> vec(scalar);

  CHECK_CLOSE(scalar, vec[0], g_zero_tol);
  CHECK_CLOSE(scalar, vec[1], g_zero_tol);
}

TEST(TestVector_Constructor_StdVector)
{
  std::vector<double> exp_vec = {1, 2};
  Vector<double, 2> vec(exp_vec);

  CHECK_CLOSE(exp_vec[0], vec[0], g_zero_tol);
  CHECK_CLOSE(exp_vec[1], vec[1], g_zero_tol);
}

TEST(TestVector_Constructor_TwoValues)
{
  auto first_value = 1.0;
  auto second_value = 2.0;
  Vector<double, 2> vec(first_value, second_value);

  CHECK_CLOSE(first_value, vec[0], g_zero_tol);
  CHECK_CLOSE(second_value, vec[1], g_zero_tol);
}

TEST(TestVector_Constructor_CopyConstructor)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(vec_1);

  CHECK_CLOSE(vec_1[0], vec_2[0], g_zero_tol);
  CHECK_CLOSE(vec_1[1], vec_2[1], g_zero_tol);
}

TEST(TestVector_Constructor_CumulativeAdd)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(3, 4);

  vec_1 += vec_2;

  CHECK_CLOSE(4.0, vec_1[0], g_zero_tol);
  CHECK_CLOSE(6.0, vec_1[1], g_zero_tol);
}
}
}  // namespace iblbm
