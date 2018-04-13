#ifndef BOUNDARY_SRC_MOMENTUMEXCHANGEBOUNCEBACKBOUNDARY_HPP_
#define BOUNDARY_SRC_MOMENTUMEXCHANGEBOUNCEBACKBOUNDARY_HPP_

#include <vector>

#include "AbstractDynamicsInterface.hpp"

namespace iblbm
{
template<typename T, template<typename U> class Lattice>
class MomentumExchangeBounceBackBoundary
  : public AbstractDynamicsInterface<T, Lattice>
{
 public:
  /**
   * Constructor. Arbitrary density value not set
   */
  MomentumExchangeBounceBackBoundary();

  /**
   * Constructor. Arbitrary density value set for the cell
   *
   * \param rho arbitrary density value
   */
  explicit MomentumExchangeBounceBackBoundary(T rho);

  /**
   * Collision step. Bounce back with a fixed velocity mU
   *
   * \param rCell target cell
   * \param rStatistics lattice statistics to store values for
   *        post-processing step
   */
  void Collide(
      Cell<T, Lattice>& rCell
    , LatticeStatistics<T>& rStatistics) override;

  /**
   * Return rho if it's defined, else return zero
   *
   * \param rCell target cell
   */
  T ComputeRho(const Cell<T, Lattice>& rCell) const override;

  /**
   * Set rU to mU
   *
   * \param rCell target cell
   * \param rU fluid velocity
   */
  void ComputeU(
      const Cell<T, Lattice>& rCell
    , std::vector<T>& rU) const override;

  /**
   * Set rU to mU and rRho to mFixedRho if it's defined, else zero
   *
   * \param rCell target cell
   * \param rRho particle density
   * \param rU fluid velocity
   */
  void ComputeRhoAndU(
      const Cell<T, Lattice>& rCell
    , T& rRho
    , std::vector<T>& rU) const override;

  /** \return zero */
  T GetOmega() const override;

 private:
  /** Arbitrary particle density. Default value of zero */
  T mRho;
};
}  // namespace iblbm

#endif  // BOUNDARY_SRC_MOMENTUMEXCHANGEBOUNCEBACKBOUNDARY_HPP_
