#ifndef SRC_LBM_LATTICEBOLTZMANNMETHOD_HPP_
#define SRC_LBM_LATTICEBOLTZMANNMETHOD_HPP_

#include "AbstractBoundaryCondition.hpp"
#include "AbstractCollisionModel.hpp"
#include "AbstractLatticeModel.hpp"
#include "AbstractStreamModel.hpp"

namespace iblbm
{
class LatticeBoltzmannMethod
{
 public:
  LatticeBoltzmannMethod(
      AbstractLatticeModel& r_lm
    , AbstractCollisionModel& r_cm
    , AbstractStreamModel& r_sm);

  ~LatticeBoltzmannMethod() = default;

  void AddBoundaryCondition(AbstractBoundaryCondition* p_bc);

  void TakeStep();

 private:
  AbstractLatticeModel& r_lm_;
  AbstractCollisionModel& r_cm_;
  AbstractStreamModel& r_sm_;
  std::vector<std::vector<double>> df_;
  std::vector<AbstractBoundaryCondition*> bc_;

};
}  // namespace iblbm

#endif  // SRC_LBM_LATTICEBOLTZMANNMETHOD_HPP_
