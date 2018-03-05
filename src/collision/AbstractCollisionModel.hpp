#ifndef SRC_COLLISION_ABSTRACTCOLLISIONMODEL_HPP_
#define SRC_COLLISION_ABSTRACTCOLLISIONMODEL_HPP_

#include "AbstractLatticeModel.hpp"

namespace iblbm
{
class AbstractCollisionModel
{
 public:
  AbstractCollisionModel(AbstractLatticeModel& r_lm);

  void InitializeDensity(double initial_density);

  void ComputeEquilibriumDistribution();

 protected:
  /** Reference to the lattice model */
  AbstractLatticeModel& r_lm_;
  /** Convenience variable: cs_sqr_ = c * c / 3.0 */
  double cs_sqr_;
  /** Density */
  std::vector<double> rho_;
  /** Equilibrium distribution function */
  std::vector<std::vector<double>> edf_;
};
}  // namespace iblbm
#endif  // SRC_COLLISION_ABSTRACTCOLLISIONMODEL_HPP_
