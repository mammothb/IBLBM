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
      AbstractLatticeModel& r_lm
    , double initial_density);

  ~CollisionNsf() = default;

  void Collide(std::vector<std::vector<double>>& r_df);

  void SetForce(const std::vector<double>& r_initial_force);

 private:
  std::vector<std::vector<double>> force_;
};
}  // namespace iblbm
#endif  // SRC_COLLISION_COLLISIONNSF_HPP_
