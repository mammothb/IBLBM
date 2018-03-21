#include "LatticeBoltzmannMethod.hpp"

#include "AbstractCollisionModel.hpp"
#include "AbstractLatticeModel.hpp"
#include "AbstractStreamModel.hpp"

namespace iblbm
{
LatticeBoltzmannMethod::LatticeBoltzmannMethod(
    AbstractLatticeModel& r_lm
  , AbstractCollisionModel& r_cm
  , AbstractStreamModel& r_sm)
  : r_lm_(r_lm),
    r_cm_(r_cm),
    r_sm_(r_sm),
    df_(r_cm.rGetEquilibriumDistribution()),
    bc_()
{}

void LatticeBoltzmannMethod::AddBoundaryCondition(
    AbstractBoundaryCondition* p_bc)
{
  bc_.push_back(p_bc);
}

void LatticeBoltzmannMethod::TakeStep()
{
  r_cm_.ComputeEquilibriumDistribution();
  r_cm_.Collide(df_);
  for (auto it : bc_) {
    if (it->IsBeforeStream())
        it->UpdateNodes(df_, /*is_modify_stream=*/false);
  }
  df_ = r_sm_.Stream(df_);
  for (auto it : bc_) {
    if (it->IsDuringStream())
        it->UpdateNodes(df_, /*is_modify_stream=*/true);
    if (!it->IsBeforeStream())
        it->UpdateNodes(df_, /*is_modify_stream=*/false);
  }  // bdr
//  r_cm_.ComputeMacroscopicProperties(df_);
}
}  // namespace iblbm
