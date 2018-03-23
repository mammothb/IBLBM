#ifndef SRC_DYNAMICS_DESCRIPTOR_HPP_
#define SRC_DYNAMICS_DESCRIPTOR_HPP_

#include <cstddef>
#include <gsl/gsl>
#include <stdexcept>
#include <vector>

/** \file
 * Descriptor for various lattice arrangement.
 *  -- template instantiation
 */
namespace iblbm
{
/**
 * The lattice directions must always be ordered in such a way that
 * sE[i] = -sE[i + (q - 1) / 2] for i = 1..(q - 1) / 2, and sE[0] = 0 must
 * be the rest velocity.
 * Furthermore, the velocities sE[i] for i = 1..(q - 1) / 2 must verify
 *  - in 2D: (c[i][0] < 0) || (c[i][0] == 0 && c[i][1] < 0)
 */
namespace descriptor
{
struct NoExternalField
{
  static const std::size_t sNumScalars = 0;
  static const std::size_t sNumSpecies = 0;
  static const int sForceBeginsAt = 0;
  static const std::size_t sSizeOfForce = 0;
  static const int sVelocityBeginsAt = 0;
  static const std::size_t sSizeOfVelocity = 0;
};

struct NoExternalFieldBase
{
  /** Declares ExternalField to be an alias for NoExternalField */
  typedef NoExternalField ExternalField;
};

struct Force2dDescriptor
{
  static const std::size_t sNumScalars = 2;
  static const std::size_t sNumSpecies = 1;
  static const int sForceBeginsAt = 0;
  static const std::size_t sSizeOfForce = 2;
};

struct Force2dDescriptorBase
{
  /** Declares ExternalField to be an alias for Force2dDescriptor */
  typedef Force2dDescriptor ExternalField;
};

template <typename T, typename ExternalField>
class ExternalFieldVector
{
 public:
  /**
   * Read-write access to stored data.
   *
   * \param rIndex index of the accessed distribution function
   */
  T& operator[](const gsl::index& rIndex)
  {
    return mData[rIndex];
  }

  /**
   * Read-only access to stored data.
   *
   * \param rIndex index of the accessed distribution function
   */
  const T& operator[](const gsl::index& rIndex) const
  {
    return mData[rIndex];
  }

  void SetData(const std::vector<T>& rData)
  {
    mData = rData;
  }

 protected:
  /** Distribution function */
  std::vector<T> mData;
};

template <typename T>
class ExternalFieldVector<T, NoExternalField>
{
 public:
  /**
   * Force stops the program when called
   *
   * \param rIndex index of the accessed distribution function
   */
  T& operator[](const gsl::index& /*rIndex*/)
  {
    static T data = T();
    assert(false);
    return data;
  }

  /**
   * Force stops the program when called
   *
   * \param rIndex index of the accessed distribution function
   */
  const T& operator[](const gsl::index& /*rIndex*/) const
  {
    static T data = T();
    assert(false);
    return data;
  }

  /** Does nothing when called */
  void SetData(const std::vector<T>& /*rData*/) {}
};

/**
 * Descriptors for the D2Q9 model. The lattice directions are arranged as
 * follows:
 *
 * 1  8  7
 *  \ | /
 * 2--0--6
 *  / | \
 * 3  4  5
 */
template <typename T>
struct D2Q9DescriptorBase
{
  /** Declares BaseDescriptor to be an alias for D2Q9DescriptorBase<T> */
  typedef D2Q9DescriptorBase<T> BaseDescriptor;
  /** Number of dimensions */
  static constexpr std::size_t sD = 2u;
  /** Number of distribution functions */
  static constexpr std::size_t sQ = 9u;
  /** Size of neighborhood */
  static const std::size_t sVicinity;
  /** Lattice directions */
  static const std::vector<std::vector<T>> sE;
  /** Opposite directions */
  static const std::vector<std::size_t> sOpposite;
  /** Lattice weights */
  static const std::vector<T> sWeight;
  /** Inverse of square of speed of sound, convenience variable */
  static const T sInvCsSqr;
};

template <typename T>
struct D2Q9Descriptor
  : public D2Q9DescriptorBase<T>
  , public NoExternalFieldBase
{};

template <typename T>
struct ForcedD2Q9Descriptor
  : public D2Q9DescriptorBase<T>
  , public Force2dDescriptorBase
{};
}  // namespace descriptor
}  // namespace iblbm

// Implementation of descriptor
#include "Descriptor.ipp"
#endif  // SRC_DYNAMICS_DESCRIPTOR_HPP_
