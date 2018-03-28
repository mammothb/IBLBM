#include "LatticeBoltzmannMethod.hpp"

#include "AbstractCollisionModel.hpp"
#include "AbstractLatticeModel.hpp"
#include "AbstractStreamModel.hpp"

namespace iblbm
{
LatticeBoltzmannMethod::LatticeBoltzmannMethod(
    AbstractLatticeModel& rLatticeModel
  , AbstractCollisionModel& rCollisionModel
  , AbstractStreamModel& rStreamModel)
  : mrLatticeModel(rLatticeModel),
    mrCollisionModel(rCollisionModel),
    mrStreamModel(rStreamModel),
    mDF(rCollisionModel.rGetEquilibriumDistribution()),
    mpBoundaryConditions()
{}

void LatticeBoltzmannMethod::AddBoundaryCondition(
    AbstractBoundaryCondition* pBoundaryCondition)
{
  mpBoundaryConditions.push_back(pBoundaryCondition);
}

void LatticeBoltzmannMethod::TakeStep()
{
  mrCollisionModel.ComputeEquilibriumDistribution();
  mrCollisionModel.Collide(mDF);
  for (auto it : mpBoundaryConditions) {
    if (it->IsBeforeStream())
        it->UpdateNodes(mDF, /*is_modify_stream=*/false);
  }
  mDF = mrStreamModel.Stream(mDF);
  for (auto it : mpBoundaryConditions) {
    if (it->IsDuringStream())
        it->UpdateNodes(mDF, /*is_modify_stream=*/true);
    if (!it->IsBeforeStream())
        it->UpdateNodes(mDF, /*is_modify_stream=*/false);
  }  // bdr
  mrCollisionModel.ComputeMacroscopicProperties(mDF);
}

const std::vector<std::vector<double>>& LatticeBoltzmannMethod::
    rGetDistribution() const
{
  return mDF;
}
}  // namespace iblbm
