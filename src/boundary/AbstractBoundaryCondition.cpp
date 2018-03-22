#include "AbstractBoundaryCondition.hpp"

namespace iblbm
{
AbstractBoundaryCondition::AbstractBoundaryCondition(
    bool isBeforeStream
  , bool isDuringStream
  , AbstractLatticeModel& rLatticeModel)
  : mIsBeforeStream(isBeforeStream),
    mIsDuringStream(isDuringStream),
    mrLatticeModel(rLatticeModel)
{}

bool AbstractBoundaryCondition::IsBeforeStream() const
{
  return mIsBeforeStream;
}

bool AbstractBoundaryCondition::IsDuringStream() const
{
  return mIsDuringStream;
}
}  // namespace iblbm
