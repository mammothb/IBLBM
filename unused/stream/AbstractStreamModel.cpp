#include "AbstractStreamModel.hpp"

#include "AbstractLatticeModel.hpp"

namespace iblbm
{
AbstractStreamModel::AbstractStreamModel(AbstractLatticeModel& rLatticeModel)
  : mrLatticeModel(rLatticeModel)
{}
}  // namespace iblbm
