#ifndef SRC_DYNAMICS_HELPERSD2Q9_HPP_
#define SRC_DYNAMICS_HELPERSD2Q9_HPP_

namespace iblbm
{
// Forward declarations
template<typename T, class Descriptor> struct LbmDynamicsHelper;

/** Efficient specialization for D2Q9 lattice model */
template<typename T>
struct LbmDynamicsHelper<T, descriptor::D2Q9DescriptorBase<T>>
{
  static T ComputeRho(
      const CellBase<T, descriptor::D2Q9DescriptorBase<T>>& rCell)
  {
    auto rho = static_cast<T>(1) + rCell[0] + rCell[1] + rCell[2] +
        rCell[3] + rCell[4] + rCell[5] + rCell[6] + rCell[7] + rCell[8];
    return rho;
  }

  /**
   * Compute the sum of distribution function into several components. Since
   * Discrete velocity contain zeros, some distribution function values can
   * be ignored, so that's what we do here to speed up computation
   *
   * \param rXNeg distribution function in the negative x direction
   * \param rXZero distribution function in the zero x direction (for use in
   *        computing cell density)
   * \param rXPos distribution function in the positive x direction
   * \param rYNeg distribution function in the negative y direction
   * \param rYPos distribution function in the positive y direction
   */
  static void ComputePartialRho(
      const CellBase<T, descriptor::D2Q9DescriptorBase<T>>& rCell
    , T& rXNeg
    , T& rXZero
    , T& rXPos
    , T& rYNeg
    , T& rYPos)
  {
    rXNeg = rCell[1] + rCell[2] + rCell[3];
    rXZero = rCell[0] + rCell[4] + rCell[8];
    rXPos = rCell[5] + rCell[6] + rCell[7];

    rYNeg = rCell[3] + rCell[4] + rCell[5];
    rYPos = rCell[7] + rCell[8] + rCell[1];
  }

  static void ComputeRhoAndU(
      const CellBase<T, descriptor::D2Q9DescriptorBase<T>>& rCell
    , T& rRho
    , std::vector<T>& rU)
  {
    T x_neg;
    T x_zero;
    T x_pos;
    T y_neg;
    T y_pos;
    ComputePartialRho(rCell, x_neg, x_zero, x_pos, y_neg, y_pos);

    rRho = x_neg + x_zero + x_pos + static_cast<T>(1);
    T inv_rho = 1.0 / rRho;
    rU[0] = (x_pos - x_neg) * inv_rho;
    rU[1] = (y_pos - y_neg) * inv_rho;
  }
};

}  // namespace iblbm

#endif  // SRC_DYNAMICS_HELPERSD2Q9_HPP_
