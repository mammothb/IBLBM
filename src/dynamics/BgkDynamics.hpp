#ifndef SRC_DYNAMICS_BGKDYNAMICS_HPP_
#define SRC_DYNAMICS_BGKDYNAMICS_HPP_

#include "AbstractDynamics.hpp"

namespace iblbm
{
template<typename T, template<typename U> class Lattice>
class BgkDynamics : public AbstractDynamics<T, Lattice>
{
 public:
  /**
   * Constructor
   *
   * \param relaxationTime relaxation time, related to the dynamic viscosity
   * \param rMomenta reference to a Momenta object to compute velocity
   *        momenta
   */
  BgkDynamics(
      T relaxationTime
    , AbstractMomentaInterface<T, Lattice>& rMomenta);

  /**
   * Collision step
   *
   * \param rCell target cell
   */
  void Collide(Cell<T, Lattice>& rCell) override;

  /**
   * \return local relaxation parameter (inverse relaxation time) of the
   * dynamics
   */
  T GetOmega() const override;

 private:
  /**
   * Relaxation parameter, inverse of relaxation time since equations use the
   * inverse form
   */
  T mOmega;
};
}  // namespace iblbm

#include "BgkDynamics.ipp"

#endif  // SRC_DYNAMICS_BGKDYNAMICS_HPP_
