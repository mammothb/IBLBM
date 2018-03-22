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
      AbstractLatticeModel& rLatticeModel
    , double initialDensity);

  /**
   * Destructor
   */
  virtual ~AbstractCollisionModel() = default;

  /**
   * Updates f_eq
   */
  void ComputeEquilibriumDistribution();

  /**
   * Compute density at each node by summing up its distribution functions
   *
   * \param r_df 2D vector containing distribution functions
   *
   * \return density of lattice stored row-wise in a 1D vector
   */
  std::vector<double> ComputeRho(
      const std::vector<std::vector<double>> &rDF);

  /**
   * Pure virtual function to be implemented by various child classes
   *
   * \param r_df Reference to the distribution function to be updated
   */
  virtual void Collide(std::vector<std::vector<double>>& rDF) = 0;

  /**
   * Pure virtual function to compute the macroscopic properties of the
   * lattice depending on the equation, density and velocity for
   * Navier-Stokes, only density for Convection-diffusion equation
   * This is used to unify function calling in LatticeBoltzmann TakeStep()
   * method
   *
   * \param r_df Particle distribution functions of the lattice stored
   *        row-wise in a 2D vector
   */
  virtual void ComputeMacroscopicProperties(
      const std::vector<std::vector<double>>& rDF) = 0;

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
  void SetRelaxationTime(double relaxationTime);

  /**
   * Set density lattice with the same density value, force updates
   * equilibrium distribution function
   *
   * \param initial_density The initial value for density
   */
  void SetDensity(double density);

 protected:
  /** Reference to the lattice model */
  AbstractLatticeModel& mrLatticeModel;
  /** Convenience variable: cs_sqr_ = c * c / 3.0 */
  double mCsSqr;
  /** Relaxation time */
  double mTau;
  /** Density */
  std::vector<double> mRho;
  /** Nodes to skip collision calculation */
  std::vector<bool> mIsSkip;
  /** Equilibrium distribution function */
  std::vector<std::vector<double>> mEDF;
};
}  // namespace iblbm
#endif  // SRC_COLLISION_ABSTRACTCOLLISIONMODEL_HPP_
