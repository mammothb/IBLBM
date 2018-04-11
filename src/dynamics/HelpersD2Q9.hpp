#ifndef SRC_DYNAMICS_HELPERSD2Q9_HPP_
#define SRC_DYNAMICS_HELPERSD2Q9_HPP_

#include <vector>

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
    auto e_dot_u {B::sE[q][0] * rU[0] + B::sE[q][1] * rU[1]};
//    // Original code
//    return rho * B::sWeight[q] * (T{1} + T{3} * e_dot_u + T{4.5} * e_dot_u *
//        e_dot_u - T{1.5} * uSqr) - B::sWeight[q];
    return rho * B::sWeight[q] * (T{1} + T{3} * e_dot_u + T{4.5} * e_dot_u *
        e_dot_u - T{1.5} * uSqr);
  }

  /**
   * Perform the collision step using BGK approximation
   *
   * \param rCell target cell
   * \param rRho particle density
   * \param rU fluid velocity
   * \param rOmega inverse relaxation time
   *
   * \return dot product of fluid velocity (convenience variable)
   */
  static T BgkCollide(
      CellBase<T, descriptor::D2Q9DescriptorBase<T>>& rCell
    , const T& rRho
    , const std::vector<T>& rU
    , const T& rOmega)
  {
    auto ux2 {rU[0] * rU[0]};
    auto uy2 {rU[1] * rU[1]};

    auto ux_cs2 {T{3} * rU[0]};
    auto uy_cs2 {T{3} * rU[1]};

    auto ux2_cs2 {T{3} * ux2};
    auto uy2_cs2 {T{3} * uy2};
    auto ux2_cs2_half {ux2_cs2 / T{2}};
    auto uy2_cs2_half {uy2_cs2 / T{2}};
    auto u2_cs2_half {ux2_cs2_half + uy2_cs2_half};

    auto uxy_cs4_half_pos {T{4.5} * (rU[0] + rU[1]) * (rU[0] + rU[1])};
    auto uxy_cs4_half_neg {T{4.5} * (rU[0] - rU[1]) * (rU[0] - rU[1])};

    auto weight_rho {T{4} / T{9} * rRho};
//    // Original code
//    auto common_factor = T{4} / T{9} * (rRho - T{1});
    auto common_factor {T{4} / T{9} * rRho};

    rCell[0] *= T{1} - rOmega;
    rCell[0] += rOmega * (common_factor + weight_rho * (-ux2_cs2_half -
        uy2_cs2_half));

    weight_rho = T{1} / T{9} * rRho;
//    // Original code
//    common_factor = T{1} / T{9} * (rRho - T{1});
    common_factor = T{1} / T{9} * rRho;

    rCell[6] *= T{1} - rOmega;
    rCell[6] += rOmega * (common_factor + weight_rho * (ux_cs2 + ux2_cs2 -
        uy2_cs2_half));
    rCell[8] *= T{1} - rOmega;
    rCell[8] += rOmega * (common_factor + weight_rho * (uy_cs2 + uy2_cs2 -
        ux2_cs2_half));
    rCell[2] *= T{1} - rOmega;
    rCell[2] += rOmega * (common_factor + weight_rho * (-ux_cs2 + ux2_cs2 -
        uy2_cs2_half));
    rCell[4] *= T{1} - rOmega;
    rCell[4] += rOmega * (common_factor + weight_rho * (-uy_cs2 + uy2_cs2 -
        ux2_cs2_half));

    weight_rho = T{1} / T{36} * rRho;
//    // Original code
//    common_factor = T{1} / T{36} * (rRho - T{1});
    common_factor = T{1} / T{36} * rRho;

    rCell[7] *= T{1} - rOmega;
    rCell[7] += rOmega * (common_factor + weight_rho * (ux_cs2 + uy_cs2 +
        uxy_cs4_half_pos - u2_cs2_half));
    rCell[1] *= T{1} - rOmega;
    rCell[1] += rOmega * (common_factor + weight_rho * (-ux_cs2 + uy_cs2 +
        uxy_cs4_half_neg - u2_cs2_half));
    rCell[3] *= T{1} - rOmega;
    rCell[3] += rOmega * (common_factor + weight_rho * (-ux_cs2 - uy_cs2 +
        uxy_cs4_half_pos - u2_cs2_half));
    rCell[5] *= T{1} - rOmega;
    rCell[5] += rOmega * (common_factor + weight_rho * (ux_cs2 - uy_cs2 +
        uxy_cs4_half_neg - u2_cs2_half));

    return ux2 + uy2;
  }

  /**
   * Compute the sum of distribution function as cell density
   *
   * \param rCell target cell
   */
  static T ComputeRho(
      const CellBase<T, descriptor::D2Q9DescriptorBase<T>>& rCell)
  {
    auto rho {T{1} + rCell[0] + rCell[1] + rCell[2] + rCell[3] + rCell[4] +
        rCell[5] + rCell[6] + rCell[7] + rCell[8]};
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
    T x_neg {};
    T x_zero {};
    T x_pos {};
    T y_neg {};
    T y_pos {};
    ComputePartialRho(rCell, x_neg, x_zero, x_pos, y_neg, y_pos);

    rRho = x_neg + x_zero + x_pos + T{1};
    auto inv_rho {T{1} / rRho};
    rU[0] = (x_pos - x_neg) * inv_rho;
    rU[1] = (y_pos - y_neg) * inv_rho;
  }
};

/** Efficient specialization for D2Q9 lattice model */
template<typename T>
struct LbmExternalHelper<T, descriptor::ForcedD2Q9Descriptor>
{
  static void AddExternalForce(
      Cell<T, descriptor::ForcedD2Q9Descriptor>& rCell
    , const std::vector<double>& rU
    , T omega
    , T amplitude)
  {
    static const gsl::index force_offset {
        descriptor::ForcedD2Q9Descriptor<T>::ExternalField::sForceOffset};
    auto mu {amplitude * (T{1} - omega / T{2})};

    rCell[0] += mu * T{4} / T{3} * (rCell.rGetExternal(force_offset) *
        -rU[0] + rCell.rGetExternal(force_offset + 1) * -rU[1]);
    rCell[1] += mu * T{1} / T{12} * (rCell.rGetExternal(force_offset) *
        (2 * rU[0] - 3 * rU[1] - 1) + rCell.rGetExternal(force_offset + 1) *
        (-3 * rU[0] + 2 * rU[1] + 1));
    rCell[2] += mu * T{1} / T{3} * (rCell.rGetExternal(force_offset) *
        (2 * rU[0] - 1) + rCell.rGetExternal(force_offset + 1) * -rU[1]);
    rCell[3] += mu * T{1} / T{12} * (rCell.rGetExternal(force_offset) *
        (2 * rU[0] + 3 * rU[1] - 1) + rCell.rGetExternal(force_offset + 1) *
        (3 * rU[0] + 2 * rU[1] - 1));
    rCell[4] += mu * T{1} / T{3} * (rCell.rGetExternal(force_offset) *
        -rU[0] + rCell.rGetExternal(force_offset + 1) * (2 * rU[1] - 1));
    rCell[5] += mu * T{1} / T{12} * (rCell.rGetExternal(force_offset) *
        (2 * rU[0] - 3 * rU[1] + 1) + rCell.rGetExternal(force_offset + 1) *
        (-3 * rU[0] + 2 * rU[1] - 1));
    rCell[6] += mu * T{1} / T{3} * (rCell.rGetExternal(force_offset) *
        (2 * rU[0] + 1) + rCell.rGetExternal(force_offset + 1) * -rU[1]);
    rCell[7] += mu * T{1} / T{12} * (rCell.rGetExternal(force_offset) *
        (2 * rU[0] + 3 * rU[1] + 1) + rCell.rGetExternal(force_offset + 1) *
        (3 * rU[0] + 2 * rU[1] + 1));
    rCell[8] += mu * T{1} / T{3} * (rCell.rGetExternal(force_offset) *
        -rU[0] + rCell.rGetExternal(force_offset + 1) * (2 * rU[1] + 1));
  }
};
}  // namespace iblbm

#endif  // SRC_DYNAMICS_HELPERSD2Q9_HPP_
