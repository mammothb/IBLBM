#include "AbstractCollisionModel.hpp"

#include "IblbmUtilities.hpp"

namespace iblbm
{
AbstractCollisionModel::AbstractCollisionModel(AbstractLatticeModel& r_lm)
  : r_lm_(r_lm)
{
  auto c = r_lm_.GetPropagationSpeed();
  cs_sqr_ = c * c / 3.0;
  auto num_nodes = r_lm_.GetNumberOfNodes();
  auto num_directions = r_lm_.GetNumberOfDirections();
  // Initialize all equilibrium distribution functions with zero
  edf_.assign(num_nodes, std::vector<double>(num_directions, 0.0));
}

void AbstractCollisionModel::InitializeDensity(double initial_density)
{
  auto num_nodes = r_lm_.GetNumberOfNodes();
  rho_.assign(num_nodes, initial_density);
}
void AbstractCollisionModel::ComputeEquilibriumDistribution()
{
  const auto nc = r_lm_.GetNumberOfDirections();
  const auto num_nodes = r_lm_.GetNumberOfNodes();
  for (auto n = 0u; n < num_nodes; ++n) {
    const auto u_sqr = util::InnerProduct(r_lm_.u_[n], r_lm_.u_[n]) / 2.0 /
        cs_sqr_;
    for (auto i = 0u; i < nc; ++i) {
      auto c_dot_u = util::InnerProduct(r_lm_.e_[i], r_lm_.u_[n]) / cs_sqr_;
      edf_[n][i] = lm_.
    }
  }
}
}
