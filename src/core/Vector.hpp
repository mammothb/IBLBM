#include <array>
#include <cstring>
#include <cstddef>
#include <iostream>
#include <vector>

#include "gsl/gsl"
#include "IblbmDebug.hpp"

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
  explicit Vector(const T& rScalar = T{});

  /** Constructor from std::vector<T> */
  explicit Vector(const std::vector<T>& rStdVector);

  /** Construct a 2D Vector from 2 values */
  Vector(
      const T& rFirstValue
    , const T& rSecondValue);

  /** Default destructor */
  ~Vector() = default;

  /** Copy constructor */
  Vector(const Vector<T, N>& rRhs);

  /** Read and write access to element */
  T& operator[](gsl::index index);

  /** Read-only access to element */
  const T& operator[](gsl::index index) const;

  /**
   * Cumulative add Vector
   *
   * \param rVector RHS vector of the same size
   */
   Vector& operator+=(const Vector<T, N>& rVector);

 private:
  /** Fixed size array to store data */
  std::array<T, N> mData;
};

template<typename T, std::size_t N>
inline Vector<T, N>::Vector(const T& rScalar/*=T{}*/)
{
  for (gsl::index d = 0; d < N; ++d) mData[d] = rScalar;
}

template<typename T, std::size_t N>
inline Vector<T, N>::Vector(const std::vector<T>& rStdVector)
{
  IBLBM_PRECONDITION(rStdVector.size() == N);
  std::memcpy(mData.data(), rStdVector.data(), N * sizeof(T));
}

template<typename T, std::size_t N>
inline Vector<T, N>::Vector(
    const T& rFirstValue
  , const T& rSecondValue)
  : mData{rFirstValue, rSecondValue}
{
  IBLBM_POSTCONDITION(N == 2);
}

template<typename T, std::size_t N>
inline Vector<T, N>::Vector(const Vector<T, N>& rRhs)
  : mData{rRhs.mData}
{}

template<typename T, std::size_t N>
inline T& Vector<T, N>::operator[](gsl::index index)
{
  IBLBM_PRECONDITION(index < N);
  return mData[index];
}

template<typename T, std::size_t N>
inline const T& Vector<T, N>::operator[](gsl::index index) const
{
  IBLBM_PRECONDITION(index < N);
  return mData[index];
}

template<typename T, std::size_t N>
inline Vector<T, N>& Vector<T, N>::operator+=(const Vector<T, N>& rVector)
{
  for (gsl::index d = 0; d < N; ++d) mData[d] += rVector[d];
  return *this;
}
}  // namespace
