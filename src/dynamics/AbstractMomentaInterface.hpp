#ifndef SRC_DYNAMICS_ABSTRACTMOMENTAINTERFACE_HPP_
#define SRC_DYNAMICS_ABSTRACTMOMENTAINTERFACE_HPP_

#include "Cell.hpp"

namespace iblbm
{
template <typename T, template <typename U> class Lattice>
class AbstractMomentaInterface
{
 public:
  /** Virtual destructor to allow inheritance */
  virtual ~AbstractMomentaInterface() = default;

  /**
   * Compute particle density on the cell
   *
   * \param rCell target cell
   * \return particle density
   */
  virtual T ComputeRho(const Cell<T, Lattice>& rCell) const = 0;
};
}  // namespace iblbm

#endif  // SRC_DYNAMICS_ABSTRACTMOMENTAINTERFACE_HPP_
