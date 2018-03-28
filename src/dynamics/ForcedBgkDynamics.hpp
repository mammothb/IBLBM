#ifndef SRC_DYNAMICS_FORCEDBGKDYNAMICS_HPP_
#define SRC_DYNAMICS_FORCEDBGKDYNAMICS_HPP_

#include <gsl/gsl>

#include "AbstractDynamics.hpp"

namespace iblbm
{
template<typename T, template<typename U> class Lattice>
class ForcedBgkDynamics : public AbstractDynamics<T, Lattice>
{
 public:
  /**
   * Constructor
   *
   * \param relaxationTime relaxation time, related to the dynamic viscosity
   * \param rMomenta reference to a Momenta object to compute velocity
   *        momenta
   */
  ForcedBgkDynamics(
      T relaxationTime
    , AbstractMomentaInterface<T, Lattice>& rMomenta);

  /**
   * Collision step
   *
   * \param rCell target cell
   * \param rStatistics lattice statistics to store values for
   *        post-processing step
   */
  void Collide(
      Cell<T, Lattice>& rCell
    , LatticeStatistics<T>& rStatistics) override;

  /**
   * Compute fluid velocity on the cell
   *
   * \param rCell target cell
   * \param rU fluid velocity
   */
  void ComputeU(
      const Cell<T, Lattice>& rCell
    , std::vector<T>& rU) const override;

  /**
   * Compute fluid velocity and particle density on the cell.
   *
   * \param rCell target cell
   * \param rRho particle density
   * \param rU fluid velocity
   */
  void ComputeRhoAndU(
      const Cell<T, Lattice>& rCell
    , T& rRho
    , std::vector<T>& rU) const override;

  /**
   * \return local relaxation parameter (inverse relaxation time) of the
   * dynamics
   */
  T GetOmega() const override;

 protected:
  /**
   * Relaxation parameter, inverse of relaxation time since equations use the
   * inverse form
   */
  T mOmega;
  /** Index offset to access force values */
  static const gsl::index msForceOffset {
      Lattice<T>::ExternalField::sForceOffset};
  /** Size of force component (number of dimensions) */
  static const gsl::index msSizeOfForce {
      Lattice<T>::ExternalField::sSizeOfForce};
};
}  // namespace iblbm

#endif  // SRC_DYNAMICS_FORCEDBGKDYNAMICS_HPP_
