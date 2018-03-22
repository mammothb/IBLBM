#include "CollisionNsf.hpp"

#include <memory>

#include "IblbmUtilities.hpp"

namespace iblbm
{
CollisionNsf::CollisionNsf(
    AbstractLatticeModel& rLatticeModel
  , double initialDensity)
  : AbstractCollisionModel(rLatticeModel, initialDensity)
{
  const auto nd = mrLatticeModel.GetNumberOfDimensions();
  CollisionNsf::SetForce(std::vector<double>(nd, 0.0));
}

void CollisionNsf::Collide(std::vector<std::vector<double>>& rDF)
{
  const auto nc = mrLatticeModel.GetNumberOfDirections();
  const auto num_nodes = mrLatticeModel.GetNumberOfNodes();
  const auto r_e = mrLatticeModel.rGetDiscreteVelocity();
  const auto r_u = mrLatticeModel.rGetVelocity();
  const auto r_weight = mrLatticeModel.rGetWeight();
  for (auto n = 0u; n < num_nodes; ++n) {
    if (!mIsSkip[n]) {
      for (auto i = 0u; i < nc; ++i) {
        const auto e_dot_u = util::InnerProduct(r_e[i], r_u[n]) / mCsSqr;
        std::vector<double> src_dot_product = {
            (r_e[i][0] - r_u[n][0] + e_dot_u * r_e[i][0]) / mCsSqr,
            (r_e[i][1] - r_u[n][1] + e_dot_u * r_e[i][1]) / mCsSqr};
        const auto src_i = (1.0 - 0.5 / mTau) * r_weight[i] *
            util::InnerProduct(src_dot_product, mForce[n]);
        rDF[n][i] += (mEDF[n][i] - rDF[n][i]) / mTau + src_i;
      }
    }
  }
}

void CollisionNsf::ComputeMacroscopicProperties(
    const std::vector<std::vector<double>>& rDF)
{
  mRho = CollisionNsf::ComputeRho(rDF);
  mrLatticeModel.SetVelocity(CollisionNsf::ComputeVelocity(rDF));
}

std::vector<std::vector<double>> CollisionNsf::ComputeVelocity(
    const std::vector<std::vector<double>>& rDF)
{
  const auto num_nodes = mrLatticeModel.GetNumberOfNodes();
  const auto r_e = mrLatticeModel.rGetDiscreteVelocity();
  std::vector<std::vector<double>> velocities;
  for (auto n = 0u; n < num_nodes; ++n) {
    velocities.push_back(util::GetFirstMoment(rDF[n], r_e));
    velocities[n][0] += 0.5 * mForce[n][0];
    velocities[n][0] /= mRho[n];
    velocities[n][1] += 0.5 * mForce[n][1];
    velocities[n][1] /= mRho[n];
  }  // n
  return velocities;
}

void CollisionNsf::SetForce(const std::vector<double>& rInitialForce)
{
  mForce.assign(mrLatticeModel.GetNumberOfNodes(), rInitialForce);
}
}  // namespace iblbm
