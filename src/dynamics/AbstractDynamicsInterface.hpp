#ifndef SRC_DYNAMICS_ABSTRACTDYNAMICSINTERFACE_HPP_
#define SRC_DYNAMICS_ABSTRACTDYNAMICSINTERFACE_HPP_

#include "Descriptor.hpp"

namespace iblbm
{
// Forward declaration
template<typename T, template<typename U> class Lattice> class Cell;

template<typename T, template<typename U> class Lattice>
class AbstractDynamicsInterface
{
 public:
  /** Virtual destructor to allow inheritance */
  virtual ~AbstractDynamicsInterface() = default;

  /**
   * Collision step
   *
   * \param rCell target cell
   */
  virtual void Collide(Cell<T, Lattice>& rCell) = 0;

  /**
   * Compute equilibrium distribution function
   *
   * \param q direction
   * \param rho cell density
   * \param rU const reference to fluid velocity
   * \param uSqr velocity dot product (convenience variable)
   */
  virtual T ComputeEquilibrium(
      std::size_t q
    , T rho
    , const std::vector<T>& rU
    , T uSqr) const;

  /**
   * Compute particle density on the cell
   *
   * \param rCell target cell
   * \return particle density
   */
  virtual T ComputeRho(const Cell<T, Lattice>& rCell) const = 0;

  /**
   * \return local relaxation parameter (inverse relaxation time) of the
   * dynamics
   */
  virtual T GetOmega() const = 0;

};
}  // namespace iblbm

//#include "AbstractDynamicsInterface.ipp"

#endif  // SRC_DYNAMICS_ABSTRACTDYNAMICSINTERFACE_HPP_
