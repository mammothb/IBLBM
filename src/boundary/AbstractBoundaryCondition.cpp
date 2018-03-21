#include "AbstractBoundaryCondition.hpp"

namespace iblbm
{
AbstractBoundaryCondition::AbstractBoundaryCondition(
    bool is_before_stream
  , bool is_during_stream
  , AbstractLatticeModel& r_lm)
  : is_before_stream_(is_before_stream),
    is_during_stream_(is_during_stream),
    r_lm_(r_lm)
{}

bool AbstractBoundaryCondition::IsBeforeStream() const
{
  return is_before_stream_;
}

bool AbstractBoundaryCondition::IsDuringStream() const
{
  return is_during_stream_;
}
}  // namespace iblbm
