#ifndef SRC_DYNAMICS_HELPERS_HPP_
#define SRC_DYNAMICS_HELPERS_HPP_

namespace iblbm
{
// Forward declarations
template<typename T, class Descriptor> struct LbmDynamicsHelper;

template<typename T, template<typename U> class Lattice>
struct LbmHelper
{
  static T ComputeRho(const Cell<T, Lattice>& rCell)
  {
    return LbmDynamicsHelper<T, typename Lattice<T>::BaseDescriptor>::
        ComputeRho(rCell);
  }

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
  static T ComputeRho(const CellBase<T, Descriptor>& rCell)
  {
    auto rho = static_cast<T>(1);
    for (gsl::index i = 0; i < Descriptor::sQ; ++i) rho += rCell[i];
    return rho;
  }

  /**
   * Compute fluid velocity and particle density on the cell.
   *
   * \param rCell target cell
   * \param rRho particle density
   * \param rU fluid velocity
   */
  static void ComputeRhoAndU(
      const CellBase<T, Descriptor>& rCell
    , T& rRho
    , std::vector<T>& rU)
  {
    rRho = T();
    for (gsl::index d = 0; d < Descriptor::sD; ++d) rU[d] = T();
    for (gsl::index i = 0; i < Descriptor::sQ; ++i) {
      rRho += rCell[i];
      for (gsl::index d = 0; d < Descriptor::sD; ++d)
          rU[d] += rCell[i] * Descriptor::sE[i][d];
    }
    rRho += static_cast<T>(1);
    for (gsl::index d = 0; d < Descriptor::sD; ++d) rU[d] /= rRho;
  }
};
}  // namespace iblbm

#include "HelpersD2Q9.hpp"

#endif  // SRC_DYNAMICS_HELPERS_HPP_
