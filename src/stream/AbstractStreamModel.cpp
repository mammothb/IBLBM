#include "AbstractStreamModel.hpp"

#include "AbstractLatticeModel.hpp"

namespace iblbm
{
AbstractStreamModel::AbstractStreamModel(AbstractLatticeModel& r_lm)
  : r_lm_(r_lm)
{}
}  // namespace iblbm
