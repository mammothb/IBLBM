#include "AbstractCollisionModel.hpp"

#include <iostream>

#include "IblbmUtilities.hpp"

namespace iblbm
{
AbstractCollisionModel::AbstractCollisionModel(
    AbstractLatticeModel& rLatticeModel
  , double initialDensity)
  : mrLatticeModel(rLatticeModel),
    mTau(1.0)
{
  auto c = mrLatticeModel.GetPropagationSpeed();
  mCsSqr = c * c / 3.0;
  auto num_nodes = mrLatticeModel.GetNumberOfNodes();
  auto num_directions = mrLatticeModel.GetNumberOfDirections();
  mIsSkip.assign(num_nodes, false);
  // Initialize all equilibrium distribution functions with zero
  mEDF.assign(num_nodes, std::vector<double>(num_directions, 0.0));
  AbstractCollisionModel::SetDensity(initialDensity);
}

void AbstractCollisionModel::ComputeEquilibriumDistribution()
{
  const auto nc = mrLatticeModel.GetNumberOfDirections();
  const auto num_nodes = mrLatticeModel.GetNumberOfNodes();
  const auto r_e = mrLatticeModel.rGetDiscreteVelocity();
  const auto r_u = mrLatticeModel.rGetVelocity();
  const auto r_weight = mrLatticeModel.rGetWeight();
  for (auto n = 0u; n < num_nodes; ++n) {
    const auto u_sqr = util::InnerProduct(r_u[n], r_u[n]) / 2.0 / mCsSqr;
    for (auto i = 0u; i < nc; ++i) {
      auto e_dot_u = util::InnerProduct(r_e[i], r_u[n]) / mCsSqr;
      mEDF[n][i] = mRho[n] * r_weight[i] * (1.0 + e_dot_u * (1.0 + e_dot_u /
          2.0) - u_sqr);
    }
  }
}

std::vector<double> AbstractCollisionModel::ComputeRho(
    const std::vector<std::vector<double>>& rDF)
{
  std::vector<double> result;
  for (auto n = 0u; n < rDF.size(); ++n)
      result.push_back(util::GetZerothMoment(rDF[n]));
  return result;
}

void AbstractCollisionModel::AddNodeToSkip(std::size_t n)
{
  mIsSkip[n] = true;
}

const std::vector<double>& AbstractCollisionModel::rGetDensity() const
{
  return mRho;
}

const std::vector<bool>& AbstractCollisionModel::rGetIsSkip() const
{
  return mIsSkip;
}

const std::vector<std::vector<double>>& AbstractCollisionModel::
    rGetEquilibriumDistribution() const
{
  return mEDF;
}

void AbstractCollisionModel::SetRelaxationTime(double relaxationTime)
{
  mTau = relaxationTime;
}

void AbstractCollisionModel::SetDensity(double initialDensity)
{
  auto num_nodes = mrLatticeModel.GetNumberOfNodes();
  mRho.assign(num_nodes, initialDensity);
  AbstractCollisionModel::ComputeEquilibriumDistribution();
}
}
