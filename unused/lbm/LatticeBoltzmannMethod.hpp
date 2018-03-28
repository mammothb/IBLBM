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
      AbstractLatticeModel& rLatticeModel
    , AbstractCollisionModel& rCollisionModel
    , AbstractStreamModel& rStreamModel);

  ~LatticeBoltzmannMethod() = default;

  void AddBoundaryCondition(AbstractBoundaryCondition* pBoundaryCondition);

  void TakeStep();

  const std::vector<std::vector<double>>& rGetDistribution() const;

 private:
  AbstractLatticeModel& mrLatticeModel;
  AbstractCollisionModel& mrCollisionModel;
  AbstractStreamModel& mrStreamModel;
  std::vector<std::vector<double>> mDF;
  std::vector<AbstractBoundaryCondition*> mpBoundaryConditions;

};
}  // namespace iblbm

#endif  // SRC_LBM_LATTICEBOLTZMANNMETHOD_HPP_
