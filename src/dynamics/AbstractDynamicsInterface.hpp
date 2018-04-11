#ifndef SRC_DYNAMICS_ABSTRACTDYNAMICSINTERFACE_HPP_
#define SRC_DYNAMICS_ABSTRACTDYNAMICSINTERFACE_HPP_

#include <vector>

#include "gsl/gsl"
#include "Descriptor.hpp"
#include "LatticeStatistics.hpp"

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
   * \param rStatistics lattice statistics to store values for
   *        post-processing step
   */
  virtual void Collide(
      Cell<T, Lattice>& rCell
    , LatticeStatistics<T>& rStatistics) = 0;

  /**
   * Compute particle density on the cell
   *
   * \param rCell target cell
   * \return particle density
   */
  virtual T ComputeRho(const Cell<T, Lattice>& rCell) const = 0;

  /**
   * Compute fluid velocity on the cell
   *
   * \param rCell target cell
   * \param rU fluid velocity
   */
  virtual void ComputeU(
      const Cell<T, Lattice>& rCell
    , std::vector<T>& rU) const = 0;

  /**
   * Compute fluid velocity and particle density on the cell.
   *
   * \param rCell target cell
   * \param rRho particle density
   * \param rU fluid velocity
   */
  virtual void ComputeRhoAndU(
      const Cell<T, Lattice>& rCell
    , T& rRho
    , std::vector<T>& rU) const = 0;

  /**
   * \return local relaxation parameter (inverse relaxation time) of the
   * dynamics
   */
  virtual T GetOmega() const = 0;

  /**
   * Compute equilibrium distribution function
   *
   * \param q direction
   * \param rho particle density
   * \param rU const reference to fluid velocity
   * \param uSqr velocity dot product (convenience variable)
   */
  virtual T ComputeEquilibrium(
      gsl::index q
    , T rho
    , const std::vector<T>& rU
    , T uSqr) const;

  /**
   * Initialize cell at equilibrium distribution
   *
   * \param rCell target cell
   * \param rho particle density
   * \param rU fluid velocity
   */
  void InitializeEquilibrium(
      Cell<T, Lattice>& rCell
    , T rho
    , const std::vector<T>& rU);
};
}  // namespace iblbm

#endif  // SRC_DYNAMICS_ABSTRACTDYNAMICSINTERFACE_HPP_
