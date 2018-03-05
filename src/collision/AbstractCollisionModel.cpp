#include "AbstractCollisionModel.hpp"

#include "IblbmUtilities.hpp"

namespace iblbm
{
AbstractCollisionModel::AbstractCollisionModel(AbstractLatticeModel& r_lm)
  : r_lm_(r_lm),
    weight_(r_lm_.rGetWeight()),
    e_(r_lm_.rGetDiscreteVelocity())
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
  const auto r_u = r_lm_.rGetVelocity();
  for (auto n = 0u; n < num_nodes; ++n) {
    const auto u_sqr = util::InnerProduct(r_u[n], r_u[n]) / 2.0 / cs_sqr_;
    for (auto i = 0u; i < nc; ++i) {
      auto c_dot_u = util::InnerProduct(e_[i], r_u[n]) / cs_sqr_;
      edf_[n][i] = rho_[n] * weight_[i] * (1.0 + c_dot_u * (1.0 + c_dot_u) -
          u_sqr);
    }
  }
}

const std::vector<double>& AbstractCollisionModel::rGetDensity() const
{
  return rho_;
}

const std::vector<std::vector<double>>& AbstractCollisionModel::
    rGetEquilibriumDistribution() const
{
  return edf_;
}
}
