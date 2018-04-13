#ifndef DYNAMICS_SRC_ABSTRACTMOMENTAINTERFACE_HPP_
#define DYNAMICS_SRC_ABSTRACTMOMENTAINTERFACE_HPP_

#include <vector>

#include "Cell.hpp"

namespace iblbm
{
template<typename T, template<typename U> class Lattice>
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
    , std::vector<T>& rU) const;
};
}  // namespace iblbm

#endif  // DYNAMICS_SRC_ABSTRACTMOMENTAINTERFACE_HPP_
