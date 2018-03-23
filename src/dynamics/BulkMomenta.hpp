#ifndef SRC_DYNAMICS_BULKMOMENTA_HPP_
#define SRC_DYNAMICS_BULKMOMENTA_HPP_

#include "AbstractMomentaInterface.hpp"

namespace iblbm
{
template <typename T, template <typename U> class Lattice>
class BulkMomenta : public AbstractMomentaInterface<T, Lattice>
{
 public:
  /**
   * Compute particle density on the cell
   *
   * \param rCell target cell
   * \return particle density
   */
  T ComputeRho(const Cell<T, Lattice>& rCell) const override;
};
}  // namespace iblbm

#include "BulkMomenta.ipp"

#endif  // SRC_DYNAMICS_BULKMOMENTA_HPP_
