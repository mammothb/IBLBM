#ifndef SRC_DYNAMICS_ABSTRACTDYNAMICSINTERFACE_HPP_
#define SRC_DYNAMICS_ABSTRACTDYNAMICSINTERFACE_HPP_

#include "Cell.hpp"
#include "Descriptor.hpp"

namespace iblbm
{
// Forward declaration
template <typename T, template <typename U> class Lattice> class Cell;

template <typename T, template <typename U> class Lattice>
class AbstractDynamicsInterface
{
 public:
  /** Virtual destructor to allow inheritance */
  virtual ~AbstractDynamicsInterface() = default;

//  virtual void Collide(
//      Cell<T, Lattice>& rCell
//    , LatticeStatistics<T>& rStatistics) = 0;

  /**
   * Compute particle density on the cell
   *
   * \param rCell target cell
   * \return particle density
   */
  virtual T ComputeRho(const Cell<T, Lattice>& rCell) const = 0;

};
}  // namespace iblbm

#endif  // SRC_DYNAMICS_ABSTRACTDYNAMICSINTERFACE_HPP_
