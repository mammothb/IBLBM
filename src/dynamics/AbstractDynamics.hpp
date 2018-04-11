#ifndef SRC_DYNAMICS_ABSTRACTDYNAMICS_HPP_
#define SRC_DYNAMICS_ABSTRACTDYNAMICS_HPP_

#include <vector>

#include "AbstractDynamicsInterface.hpp"
#include "AbstractMomentaInterface.hpp"

namespace iblbm
{
template<typename T, template<typename U> class Lattice>
class AbstractDynamics : public AbstractDynamicsInterface<T, Lattice>
{
 public:
  /**
   * Constructor
   *
   * \param rMomenta Interface for classes that compute velocity momenta
   */
  explicit AbstractDynamics(AbstractMomentaInterface<T, Lattice>& rMomenta);

  /** Virtual destructor to allow inheritance */
  virtual ~AbstractDynamics() = default;

  /**
   * Compute particle density on the cell
   *
   * \param rCell target cell
   * \return particle density
   */
  T ComputeRho(const Cell<T, Lattice>& rCell) const override;

  /**
   * Compute fluid velocity on the cell
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

 protected:
  /** Used to compute velocity momenta */
  AbstractMomentaInterface<T, Lattice>& mrMomenta;
};
}  // namespace iblbm

#endif  // SRC_DYNAMICS_ABSTRACTDYNAMICS_HPP_
