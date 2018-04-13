#ifndef SRC_CORE_VECTOR_HPP_
#define SRC_CORE_VECTOR_HPP_

#include <array>
#include <cstring>
#include <cstddef>
#include <iostream>
#include <vector>

#include "gsl/gsl"
#include "Exception.hpp"
#include "CoreUtilities.hpp"

namespace iblbm
{
/**
 * Implementation of a static size Vector class. Default value of elements in
 * Vector it T{0}
 */
template<typename T, std::size_t N>
class Vector
{
 public:
  /** Construct with all entries as the scalar */
  explicit Vector(const T& rScalar = T{})
  {
    for (gsl::index d = 0; d < N; ++d) mData[d] = rScalar;
  }

  /** Constructor from std::vector<T> */
  explicit Vector(const std::vector<T>& rStdVector)
  {
    Expects(rStdVector.size() == N);
    std::memcpy(mData.data(), rStdVector.data(), N * sizeof(T));
  }

  /** Construct a 2D Vector from 2 values */
  Vector(
      const T& rFirstValue
    , const T& rSecondValue)
    : mData{rFirstValue, rSecondValue}
  {
    Ensures(N == 2);
  }

  /** Default destructor */
  ~Vector() = default;

  /** Copy constructor */
  Vector(const Vector<T, N>& rRhs)
    : mData{rRhs.mData}
  {}

  /** Read and write access to element */
  T& operator[](gsl::index index)
  {
    Expects(index < N);
    return mData[index];
  }

  /** Read-only access to element */
  const T& operator[](gsl::index index) const
  {
    Expects(index < N);
    return mData[index];
  }

  /**
   * Cumulative add Vector
   *
   * \param rVector RHS vector of the same size
   */
  Vector& operator+=(const Vector<T, N>& rVector)
  {
    for (gsl::index d = 0; d < N; ++d) mData[d] += rVector[d];
    return *this;
  }

  /**
   * Cumulative subtract Vector
   *
   * \param rVector RHS vector of the same size
   */
  Vector& operator-=(const Vector<T, N>& rVector)
  {
    for (gsl::index d = 0; d < N; ++d) mData[d] -= rVector[d];
    return *this;
  }

  /**
   * Cumulative add scalar to each entry in the Vector
   *
   * \param rScalar scalar value
   */
  Vector& operator+=(const T& rScalar)
  {
    for (gsl::index d = 0; d < N; ++d) mData[d] += rScalar;
    return *this;
  }

  /**
   * Cumulative subtract scalar to each entry in the Vector
   *
   * \param rScalar scalar value
   */
  Vector& operator-=(const T& rScalar)
  {
    for (gsl::index d = 0; d < N; ++d) mData[d] -= rScalar;
    return *this;
  }

  /**
   * Cumulative multiply scalar to each entry in the Vector
   *
   * \param rScalar scalar value
   */
  Vector& operator*=(const T& rScalar)
  {
    for (gsl::index d = 0; d < N; ++d) mData[d] *= rScalar;
    return *this;
  }

  /**
   * Cumulative divide scalar to each entry in the Vector
   *
   * \param rScalar scalar value
   */
  Vector& operator/=(const T& rScalar)
  {
    for (gsl::index d = 0; d < N; ++d) mData[d] /= rScalar;
    return *this;
  }


  /**
   * Check equal with another Vector, return TRUE if all elements are equal
   *
   * \param rVector RHS vector of the same size
   */
  bool operator==(const Vector<T, N>& rVector) const
  {
    auto is_equal = true;
    for (gsl::index d = 0; d < N; ++d)
        is_equal &= util::IsNearZero(std::abs(mData[d] - rVector[d]));
    return is_equal;
  }

  /**
   * Check not equal with another Vector, called operator== overload and
   * inverse the result
   *
   * \param rVector RHS vector of the same size
   */
  bool operator!=(const Vector<T, N>& rVector) const
  {
    return !(*this == rVector);
  }

  /**
   * Return the size of mData. For compliance with CheckCloseVector test
   * utility
   */
  std::size_t size() const
  {
    return N;
  }

  /** \return std::vector of mData */
  std::vector<T> ConvertToStdVector() const
  {
    return std::vector<T>(mData.begin(), mData.end());
  }

 private:
  /** Fixed size array to store data */
  std::array<T, N> mData;
};

/** operator<<: output Vector as (x, y, z) */
template<typename T, std::size_t N>
inline std::ostream& operator<<(
    std::ostream& rOs
  , const Vector<T, N>& rVector)
{
  rOs << "(";
  for (gsl::index d = 0; d < N - 1; ++d) rOs << rVector[d] << ", ";
  rOs << rVector[N - 1] << ")";
  return rOs;
}

/***************************************************************************
 * Overload general arithmetic operator for Vector and its base type
 ***************************************************************************/
/** Addition: rScalar + rVector */
template<typename T, std::size_t N>
inline Vector<T, N> operator+(
    const T& rScalar
  , const Vector<T, N>& rVector)
{
  return Vector<T, N>(rVector) += rScalar;
}

/** Addition: rVector + rScalar */
template<typename T, std::size_t N>
inline Vector<T, N> operator+(
    const Vector<T, N>& rVector
  , const T& rScalar)
{
  return Vector<T, N>(rVector) += rScalar;
}

/** Subtraction: rVector + rVector */
template<typename T, std::size_t N>
inline Vector<T, N> operator+(
    const Vector<T, N>& rVector1
  , const Vector<T, N>& rVector2)
{
  return Vector<T, N>(rVector1) += rVector2;
}

/** Subtraction: rScalar + rVector */
template<typename T, std::size_t N>
inline Vector<T, N> operator-(
    const T& rScalar
  , const Vector<T, N>& rVector)
{
  return Vector<T, N>(rVector) -= rScalar;
}

/** Subtraction: rVector + rScalar */
template<typename T, std::size_t N>
inline Vector<T, N> operator-(
    const Vector<T, N>& rVector
  , const T& rScalar)
{
  return Vector<T, N>(rVector) -= rScalar;
}

/** Addition: rVector + rVector */
template<typename T, std::size_t N>
inline Vector<T, N> operator-(
    const Vector<T, N>& rVector1
  , const Vector<T, N>& rVector2)
{
  return Vector<T, N>(rVector1) -= rVector2;
}

/** Multiplication: rScalar * rVector */
template<typename T, std::size_t N>
inline Vector<T, N> operator*(
    const T& rScalar
  , const Vector<T, N>& rVector)
{
  return Vector<T, N>(rVector) *= rScalar;
}

/** Multiplication: rVector * rScalar */
template<typename T, std::size_t N>
inline Vector<T, N> operator*(
    const Vector<T, N>& rVector
  , const T& rScalar)
{
  return Vector<T, N>(rVector) *= rScalar;
}

/** Multiplication: rVector * rVector (inner product) */
template<typename T, std::size_t N>
inline T operator*(
    const Vector<T, N>& rVector1
  , const Vector<T, N>& rVector2)
{
  T scalar_product {0};
  for (gsl::index d = 0; d < N; ++d)
      scalar_product += rVector1[d] * rVector2[d];
  return scalar_product;
}

/** Division: rVector * rScalar */
template<typename T, std::size_t N>
inline Vector<T, N> operator/(
    const Vector<T, N>& rVector
  , const T& rScalar)
{
  return Vector<T, N>(rVector) /= rScalar;
}

/** operator<: returns true if all elements are smaller */
template<typename T, std::size_t N>
inline bool operator<(
    const Vector<T, N>& rLhs
  , const Vector<T, N>& rRhs)
{
  auto is_less_than = true;
  for (gsl::index d = 0; d < N; ++d) is_less_than &= rLhs[d] < rRhs[d];
  return is_less_than;
}

/** operator>: returns true if all elements are larger */
template<typename T, std::size_t N>
inline bool operator>(
    const Vector<T, N>& rLhs
  , const Vector<T, N>& rRhs)
{
  return rRhs < rLhs;
}

/** operator<=: returns true if all elements are smaller or equal */
template<typename T, std::size_t N>
inline bool operator<=(
    const Vector<T, N>& rLhs
  , const Vector<T, N>& rRhs)
{
  auto is_less_or_equal = true;
  for (gsl::index d = 0; d < N; ++d) is_less_or_equal &= rLhs[d] <= rRhs[d];
  return is_less_or_equal;
}

/** operator>=: returns true if all elements are larger or equal */
template<typename T, std::size_t N>
inline bool operator>=(
    const Vector<T, N>& rLhs
  , const Vector<T, N>& rRhs)
{
  return rRhs <= rLhs;
}

// Create type alias
template<typename T>
using Scalar = Vector<T, 1>;

template<typename T>
using Vector2D = Vector<T, 2>;
}  // namespace iblbm

#endif  // SRC_CORE_VECTOR_HPP_
