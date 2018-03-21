#ifndef SRC_COLLISION_ABSTRACTCOLLISIONMODEL_HPP_
#define SRC_COLLISION_ABSTRACTCOLLISIONMODEL_HPP_

#include "AbstractLatticeModel.hpp"

namespace iblbm
{
class AbstractCollisionModel
{
 public:
  /**
   * Constructor
   */
  AbstractCollisionModel(
      AbstractLatticeModel& r_lm
    , double initial_density);

  /**
   * Destructor
   */
  virtual ~AbstractCollisionModel() = default;

  /**
   * Updates f_eq
   */
  void ComputeEquilibriumDistribution();

  /**
   * Pure virtual function to be implemented by various child classes
   *
   * \param r_df Reference to the distribution function to be updated
   */
  virtual void Collide(std::vector<std::vector<double>>& r_df) = 0;

  /**
   * Adds a node to exclude it from the collision step
   *
   * \param n index of the node in the lattice
   */
  void AddNodeToSkip(std::size_t n);

  /** \return Const reference to density lattice */
  const std::vector<double>& rGetDensity() const;

  /** \return Const reference to is_skip lattice */
  const std::vector<bool>& rGetIsSkip() const;

  /** \return Const reference to equilibrium distribution function */
  const std::vector<std::vector<double>>&
      rGetEquilibriumDistribution() const;

  /** Set the relaxation time */
  void SetRelaxationTime(double tau);

  /**
   * Set density lattice with the same density value, force updates
   * equilibrium distribution function
   *
   * \param initial_density The initial value for density
   */
  void SetDensity(double density);

 protected:
  /** Reference to the lattice model */
  AbstractLatticeModel& r_lm_;
  /** Convenience variable: cs_sqr_ = c * c / 3.0 */
  double cs_sqr_;
  /** Relaxation time */
  double tau_;
  /** Discrete velocity */
  std::vector<double> weight_;
  /** Discrete velocity */
  std::vector<std::vector<double>> e_;
  /** Density */
  std::vector<double> rho_;
  /** Nodes to skip collision calculation */
  std::vector<bool> is_skip_;
  /** Equilibrium distribution function */
  std::vector<std::vector<double>> edf_;
};
}  // namespace iblbm
#endif  // SRC_COLLISION_ABSTRACTCOLLISIONMODEL_HPP_
