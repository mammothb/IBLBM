#ifndef SRC_DYNAMICS_HELPERS_HPP_
#define SRC_DYNAMICS_HELPERS_HPP_

#include <numeric>
#include <vector>

#include "gsl/gsl"
#include "Cell.hpp"
#include "CoreUtilities.hpp"
#include "Descriptor.hpp"

namespace iblbm
{
// Forward declarations
template<typename T, class Descriptor> struct LbmDynamicsHelper;
template<typename T, template<typename U> class Lattice>
struct LbmExternalHelper;

template<typename T, template<typename U> class Lattice>
struct LbmHelper
{
  /**
   * Compute equilibrium distribution function
   *
   * \param q direction
   * \param rho cell density
   * \param rU const reference to fluid velocity
   * \param uSqr
   */
  static T ComputeEquilibrium(
      gsl::index q
    , T rho
    , const std::vector<T>& rU
    , T uSqr)
  {
    return LbmDynamicsHelper<T, typename Lattice<T>::BaseDescriptor>::
        ComputeEquilibrium(q, rho, rU, uSqr);
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
      Cell<T, Lattice>& rCell
    , const T& rRho
    , const std::vector<T>& rU
    , const T& rOmega)
  {
    return LbmDynamicsHelper<T, typename Lattice<T>::BaseDescriptor>::
        BgkCollide(rCell, rRho, rU, rOmega);
  }

  /**
   * Compute particle density on the cell
   *
   * \param rCell target cell
   *
   * \return particle density
   */
  static T ComputeRho(const Cell<T, Lattice>& rCell)
  {
    return LbmDynamicsHelper<T, typename Lattice<T>::BaseDescriptor>::
        ComputeRho(rCell);
  }

  /**
   * Compute fluid velocity and particle density on the cell.
   *
   * \param rCell target cell
   * \param rRho particle density
   * \param rU fluid velocity
   */
  static void ComputeRhoAndU(
      const Cell<T, Lattice>& rCell
    , T& rRho
    , std::vector<T>& rU)
  {
    LbmDynamicsHelper<T, typename Lattice<T>::BaseDescriptor>::
        ComputeRhoAndU(rCell, rRho, rU);
  }

  /**
   *
   */
  static void AddExternalForce(
      Cell<T, Lattice>& rCell
    , const std::vector<T>& rU
    , T omega
    , T amplitude = 1)
  {
    LbmExternalHelper<T, Lattice>::AddExternalForce(rCell, rU, omega,
        amplitude);
  }
};

template<typename T, class Descriptor>
struct LbmDynamicsHelper
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
      gsl::index q
    , T rho
    , const std::vector<T>& rU
    , T uSqr)
  {
    auto e_dot_u {util::InnerProduct(Descriptor::sE[q], rU)};
//    // Original code
//    return rho * Descriptor::sWeight[q] * (T{1} + Descriptor::sInvCsSqr *
//        e_dot_u + Descriptor::sInvCsSqr * Descriptor::sInvCsSqr * T{0.5} *
//        e_dot_u * e_dot_u - Descriptor::sInvCsSqr * T{0.5} * uSqr) -
//        Descriptor::sWeight[q];
    // Formula according to most papers
    return rho * Descriptor::sWeight[q] * (T{1} + Descriptor::sInvCsSqr *
        e_dot_u + Descriptor::sInvCsSqr * Descriptor::sInvCsSqr * T{0.5} *
        e_dot_u * e_dot_u - Descriptor::sInvCsSqr * T{0.5} * uSqr);
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
      CellBase<T, Descriptor>& rCell
    , const T& rRho
    , const std::vector<T>& rU
    , const T& rOmega)
  {
    const auto u_sqr {util::InnerProduct(rU, rU)};
    for (gsl::index q = 0; q  < Descriptor::sQ; ++q) {
      rCell[q] *= T{1} - rOmega;
      rCell[q] += rOmega * LbmDynamicsHelper<T, Descriptor>::
          ComputeEquilibrium(q, rRho, rU, u_sqr);
    }
    return u_sqr;
  }

  /**
   * Compute particle density on the cell
   *
   * \param rCell target cell
   *
   * \return particle density
   */
  static T ComputeRho(const CellBase<T, Descriptor>& rCell)
  {
    T rho {1};
    for (gsl::index q = 0; q < Descriptor::sQ; ++q) rho += rCell[q];
    return rho;
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
      const CellBase<T, Descriptor>& rCell
    , T& rRho
    , std::vector<T>& rU)
  {
    rRho = T{1};
    for (gsl::index d = 0; d < Descriptor::sD; ++d) rU[d] = T{};
    for (gsl::index q = 0; q < Descriptor::sQ; ++q) {
      rRho += rCell[q];
      for (gsl::index d = 0; d < Descriptor::sD; ++d)
          rU[d] += rCell[q] * Descriptor::sE[q][d];
    }
    for (gsl::index d = 0; d < Descriptor::sD; ++d) rU[d] /= rRho;
  }
};

template<typename T, template<typename U> class Lattice>
struct LbmExternalHelper
{
  /**
   * Add a force term after BGK collision
   */
  static void AddExternalForce(
      Cell<T, Lattice>& rCell
    , const std::vector<T>& rU
    , T omega
    , T amplitude)
  {
    static const auto force_offset {Lattice<T>::ExternalField::sForceOffset};
    T* p_force {rCell.pGetExternal(force_offset)};
    for (gsl::index q = 0; q < Lattice<T>::sQ; ++q) {
      auto e_dot_u {util::InnerProduct(Lattice<T>::sE[q], rU) *
          Lattice<T>::sInvCsSqr * Lattice<T>::sInvCsSqr};
      T force_term {0};
      for (gsl::index d = 0; d < Lattice<T>::sD; ++d) {
        force_term += ((Lattice<T>::sE[q][d] - rU[d]) *
            Lattice<T>::sInvCsSqr + e_dot_u * Lattice<T>::sE[q][d]) *
            p_force[d];
      }
      force_term *= Lattice<T>::sWeight[q];
      force_term *= T{1} - omega / T{2};
      force_term *= amplitude;
      rCell[q] += force_term;
    }
  }
};
}  // namespace iblbm

#ifndef IBLBM_USE_GENERIC
#include "HelpersD2Q9.hpp"
#endif  // IBLBM_USE_GENERIC

#endif  // SRC_DYNAMICS_HELPERS_HPP_
