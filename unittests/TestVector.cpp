#include <iostream>
#include <typeinfo>
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

TEST(TestVector_CumulativeAddVector)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(3, 4);

  vec_1 += vec_2;

  CHECK_CLOSE(4.0, vec_1[0], g_zero_tol);
  CHECK_CLOSE(6.0, vec_1[1], g_zero_tol);
}

TEST(TestVector_CumulativeSubtractVector)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(3, 4);

  vec_1 -= vec_2;

  CHECK_CLOSE(-2.0, vec_1[0], g_zero_tol);
  CHECK_CLOSE(-2.0, vec_1[1], g_zero_tol);
}

TEST(TestVector_CumulativeAddScalar)
{
  Vector<double, 2> vec_1(1, 2);

  vec_1 += 3.4;

  CHECK_CLOSE(4.4, vec_1[0], g_zero_tol);
  CHECK_CLOSE(5.4, vec_1[1], g_zero_tol);
}

TEST(TestVector_CumulativeSubtractScalar)
{
  Vector<double, 2> vec_1(1, 2);

  vec_1 -= 3.4;

  CHECK_CLOSE(-2.4, vec_1[0], g_zero_tol);
  CHECK_CLOSE(-1.4, vec_1[1], g_zero_tol);
}

TEST(TestVector_CumulativeDivideScalar)
{
  Vector<double, 2> vec_1(1, 2);

  vec_1 /= 3.4;

  CHECK_CLOSE(1.0 / 3.4, vec_1[0], g_loose_tol);
  CHECK_CLOSE(2.0 / 3.4, vec_1[1], g_loose_tol);
}

TEST(TestVector_CheckEqual)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(1, 2);

  CHECK(vec_1 == vec_2);
}

TEST(TestVector_CheckNotEqual)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(3, 4);

  CHECK(vec_1 != vec_2);
}

TEST(TestVector_size)
{
  Vector<double, 2> vec_1(1, 2);

  CHECK_EQUAL(2u, vec_1.size());
}

TEST(TestVector_ConvertToStdVector)
{
  Vector<double, 2> vec_1(1, 2);
  auto vec_2 = vec_1.ConvertToStdVector();
  std::vector<double> exp_vec = {1, 2};

  CHECK(typeid(exp_vec) == typeid(vec_2));
}

TEST(TestVector_Ostream)
{
  // output from buffer gets written to this
  std::ostringstream ostring_stream;
  Vector<double, 2> vec_1(1, 2);
  ostring_stream << vec_1;

  auto expected_string = "(1, 2)";

  CHECK_EQUAL(expected_string, ostring_stream.str());
}

TEST(TestVector_ScalarAddVector)
{
  auto scalar = 3.4;
  Vector<double, 2> vec_1(1, 2);

  auto vec_2 = scalar + vec_1;

  CHECK_CLOSE(4.4, vec_2[0], g_loose_tol);
  CHECK_CLOSE(5.4, vec_2[1], g_loose_tol);
}

TEST(TestVector_VectorAddScalar)
{
  auto scalar = 3.4;
  Vector<double, 2> vec_1(1, 2);

  auto vec_2 = vec_1 + scalar;

  CHECK_CLOSE(4.4, vec_2[0], g_loose_tol);
  CHECK_CLOSE(5.4, vec_2[1], g_loose_tol);
}

TEST(TestVector_VectorAddVector)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(3, 4);

  auto vec_3 = vec_1 + vec_2;

  CHECK_CLOSE(4, vec_3[0], g_loose_tol);
  CHECK_CLOSE(6, vec_3[1], g_loose_tol);
}

TEST(TestVector_ScalarSubtractVector)
{
  auto scalar = 3.4;
  Vector<double, 2> vec_1(1, 2);

  auto vec_2 = scalar - vec_1;

  CHECK_CLOSE(-2.4, vec_2[0], g_loose_tol);
  CHECK_CLOSE(-1.4, vec_2[1], g_loose_tol);
}

TEST(TestVector_VectorSubtractScalar)
{
  auto scalar = 3.4;
  Vector<double, 2> vec_1(1, 2);

  auto vec_2 = vec_1 - scalar;

  CHECK_CLOSE(-2.4, vec_2[0], g_loose_tol);
  CHECK_CLOSE(-1.4, vec_2[1], g_loose_tol);
}

TEST(TestVector_VectorSubtractVector)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(3, 4);

  auto vec_3 = vec_1 - vec_2;

  CHECK_CLOSE(-2, vec_3[0], g_loose_tol);
  CHECK_CLOSE(-2, vec_3[1], g_loose_tol);
}

TEST(TestVector_ScalarMultiplyVector)
{
  auto scalar = 3.4;
  Vector<double, 2> vec_1(1, 2);

  auto vec_2 = scalar * vec_1;

  CHECK_CLOSE(3.4, vec_2[0], g_loose_tol);
  CHECK_CLOSE(6.8, vec_2[1], g_loose_tol);
}

TEST(TestVector_VectorMultiplyScalar)
{
  auto scalar = 3.4;
  Vector<double, 2> vec_1(1, 2);

  auto vec_2 = vec_1 * scalar;

  CHECK_CLOSE(3.4, vec_2[0], g_loose_tol);
  CHECK_CLOSE(6.8, vec_2[1], g_loose_tol);
}

TEST(TestVector_VectorMultiplyVector)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(3, 4);

  auto scalar_product = vec_1 * vec_2;

  CHECK_CLOSE(11.0, scalar_product, g_loose_tol);
}


TEST(TestVector_VectorDivideScalar)
{
  auto scalar = 3.4;
  Vector<double, 2> vec_1(1, 2);

  auto vec_2 = vec_1 / scalar;

  CHECK_CLOSE(1.0 / 3.4, vec_2[0], g_loose_tol);
  CHECK_CLOSE(2.0 / 3.4, vec_2[1], g_loose_tol);
}

TEST(TestVector_IsLessThan)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(3, 4);

  CHECK(vec_1 < vec_2);
}

TEST(TestVector_IsGreaterThan)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(3, 4);

  CHECK(vec_2 > vec_1);
}

TEST(TestVector_IsLessThanOrEqual)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(3, 4);
  Vector<double, 2> vec_3(3, 4);

  CHECK(vec_1 <= vec_2);
  CHECK(vec_2 <= vec_3);
}

TEST(TestVector_IsGreaterThanOrEqual)
{
  Vector<double, 2> vec_1(1, 2);
  Vector<double, 2> vec_2(3, 4);
  Vector<double, 2> vec_3(3, 4);

  CHECK(vec_2 >= vec_1);
  CHECK(vec_2 >= vec_3);
}

TEST(TestVector_TypeAlias)
{
  Vector2D<double> vec_1(1, 2);
  Vector<double, 2> vec_2(1, 2);

  CHECK(vec_1 == vec_2);
}
}
}  // namespace iblbm
