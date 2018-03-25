#ifndef SRC_DYNAMICS_HELPERSD2Q9_HPP_
#define SRC_DYNAMICS_HELPERSD2Q9_HPP_

namespace iblbm
{
/** Efficient specialization for D2Q9 lattice model */
template<typename T>
struct LbmDynamicsHelper<T, descriptor::D2Q9DescriptorBase<T>>
{
  /**
   * Compute equilibrium distribution function
   *
   * \param q direction
   * \param rho cell density
   * \param rU const reference to fluid velocity
   * \param uSqr velocity dot product (convenience variable)
   */
  static T ComputeEquilibrium(
      std::size_t q
    , T rho
    , const std::vector<T>& rU
    , T uSqr)
  {
    typedef descriptor::D2Q9DescriptorBase<T> B;
    auto e_dot_u = B::sE[q][0] * rU[0] + B::sE[q][1] * rU[1];
    return rho * B::sWeight[q] * (static_cast<T>(1) + static_cast<T>(3) *
        e_dot_u + static_cast<T>(4.5) * e_dot_u * e_dot_u -
        static_cast<T>(1.5) * uSqr) - B::sWeight[q];
  }

  /**
   * Compute the sum of distribution function as cell density
   *
   * \param rCell target cell
   */
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

  /**
   * Compute particle density and fluid velocity in one function since they
   * both require looking up particle distribution function.
   *
   * \param rCell target cell
   * \param rRho particle density to be computed
   * \param rU fluid velocity to be computed
   */
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
