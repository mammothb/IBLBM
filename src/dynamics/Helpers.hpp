#ifndef SRC_DYNAMICS_HELPERS_HPP_
#define SRC_DYNAMICS_HELPERS_HPP_

#include "Cell.hpp"
#include "Descriptor.hpp"

namespace iblbm
{
// Forward declarations
template<typename T, class Descriptor> struct LbmDynamicsHelper;

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
      std::size_t q
    , T rho
    , const std::vector<T>& rU
    , T uSqr)
  {
    return LbmDynamicsHelper<T, typename Lattice<T>::BaseDescriptor>::
        ComputeEquilibrium(q, rho, rU, uSqr);
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
      std::size_t q
    , T rho
    , const std::vector<T>& rU
    , T uSqr)
  {
    auto e_dot_u = T();
    for (auto d = 0u; d < Descriptor::sD; ++d)
        e_dot_u += Descriptor::sE[q][d] * rU[d];
    return rho * Descriptor::sWeight[q] * (static_cast<T>(1) +
        Descriptor::sInvCsSqr * e_dot_u + Descriptor::sInvCsSqr *
        Descriptor::sInvCsSqr * static_cast<T>(0.5) * e_dot_u * e_dot_u -
        Descriptor::sInvCsSqr * static_cast<T>(0.5) * uSqr) -
        Descriptor::sWeight[q];
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
    auto rho = static_cast<T>(1);
    for (auto q = 0u; q < Descriptor::sQ; ++q) rho += rCell[q];
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
    rRho = T();
    for (auto d = 0u; d < Descriptor::sD; ++d) rU[d] = T();
    for (auto q = 0u; q < Descriptor::sQ; ++q) {
      rRho += rCell[q];
      for (auto d = 0u; d < Descriptor::sD; ++d)
          rU[d] += rCell[q] * Descriptor::sE[q][d];
    }
    rRho += static_cast<T>(1);
    for (auto d = 0u; d < Descriptor::sD; ++d) rU[d] /= rRho;
  }
};
}  // namespace iblbm

#include "HelpersD2Q9.hpp"

#endif  // SRC_DYNAMICS_HELPERS_HPP_
