#include "CollisionNsf.hpp"
#include "IblbmUtilities.hpp"

namespace iblbm
{
CollisionNsf::CollisionNsf(
    AbstractLatticeModel& r_lm
  , double initial_density)
  : AbstractCollisionModel(r_lm, initial_density)
{
  const auto nd = r_lm_.GetNumberOfDimensions();
  CollisionNsf::SetForce(std::vector<double>(nd, 0.0));
}

void CollisionNsf::Collide(std::vector<std::vector<double>>& r_df)
{
  const auto nc = r_lm_.GetNumberOfDirections();
  const auto num_nodes = r_lm_.GetNumberOfNodes();
  const auto r_u = r_lm_.rGetVelocity();
  for (auto n = 0u; n < num_nodes; ++n) {
    for (auto i = 0u; i < nc; ++i) {
      const auto c_dot_u = util::InnerProduct(e_[i], r_u[n]) / cs_sqr_;
      std::vector<double> src_dot_product = {
          (e_[i][0] - r_u[n][0] + c_dot_u * e_[i][0]) / cs_sqr_,
          (e_[i][1] - r_u[n][1] + c_dot_u * e_[i][1]) / cs_sqr_};
      const auto src_i = (1.0 - 0.5 / tau_) * weight_[i] *
          util::InnerProduct(src_dot_product, force_[n]);
      r_df[n][i] += (edf_[n][i] - r_df[n][i]) / tau_ + src_i;
    }
  }
}

void CollisionNsf::SetForce(const std::vector<double>& r_initial_force)
{
  force_.assign(r_lm_.GetNumberOfNodes(), r_initial_force);
}
}  // namespace iblbm
