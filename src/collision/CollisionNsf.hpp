#ifndef SRC_COLLISION_COLLISIONNSF_HPP_
#define SRC_COLLISION_COLLISIONNSF_HPP_

#include "AbstractCollisionModel.hpp"
#include "AbstractLatticeModel.hpp"

namespace iblbm
{
class CollisionNsf : public AbstractCollisionModel
{
 public:
  CollisionNsf(
      AbstractLatticeModel& rLatticeModel
    , double initialDensity);

  ~CollisionNsf() = default;

  void Collide(std::vector<std::vector<double>>& rDF);

  /**
   * Pure virtual function to compute the macroscopic properties of the
   * lattice depending on the equation, density and velocity for
   * Navier-Stokes, only density for Convection-diffusion equation
   * This is used to unify function calling in LatticeBoltzmann TakeStep()
   * method
   *
   * \param rDF Particle distribution functions of the lattice stored
   *        row-wise in a 2D vector
   */
  void ComputeMacroscopicProperties(
      const std::vector<std::vector<double>>& rDF);

  std::vector<std::vector<double>> ComputeVelocity(
      const std::vector<std::vector<double>>& rDF);

  void SetForce(const std::vector<double>& rInitialForce);

 private:
  std::vector<std::vector<double>> mForce;
};
}  // namespace iblbm
#endif  // SRC_COLLISION_COLLISIONNSF_HPP_
