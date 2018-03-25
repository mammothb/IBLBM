#ifndef SRC_DYNAMICS_ABSTRACTDYNAMICS_HPP_
#define SRC_DYNAMICS_ABSTRACTDYNAMICS_HPP_

#include "AbstractDynamicsInterface.hpp"
#include "AbstractMomentaInterface.hpp"

namespace iblbm
{
//// Forward declaration
//template<typename T, template<typename U> class Lattice> class Cell;

template<typename T, template<typename U> class Lattice>
class AbstractDynamics : public AbstractDynamicsInterface<T, Lattice>
{
 public:
  /**
   * Constructor
   *
   * \param rMomenta Interface for classes that compute velocity momenta
   */
  AbstractDynamics(AbstractMomentaInterface<T, Lattice>& rMomenta);

  /** Virtual destructor to allow inheritance */
  virtual ~AbstractDynamics() = default;

//  virtual void Collide(
//      Cell<T, Lattice>& rCell
//    , LatticeStatistics<T>& rStatistics) = 0;

  /**
   * Compute particle density on the cell
   *
   * \param rCell target cell
   * \return particle density
   */
  T ComputeRho(const Cell<T, Lattice>& rCell) const override;

 protected:
  /** Used to compute velocity momenta */
  AbstractMomentaInterface<T, Lattice>& mrMomenta;
};
}  // namespace iblbm

#endif // SRC_DYNAMICS_ABSTRACTDYNAMICS_HPP_
