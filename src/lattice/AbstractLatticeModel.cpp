#include "AbstractLatticeModel.hpp"

#include <cstddef>

namespace iblbm
{
AbstractLatticeModel::AbstractLatticeModel(
    std::size_t numDim
  , std::size_t numDir
  , std::size_t nx
  , std::size_t ny)
  : mNumDim(numDim),
    mNumDir(numDir),
    mNx(nx),
    mNy(ny),
    mNumNodes(nx * ny),
    mC(1.0)
{}

std::size_t AbstractLatticeModel::GetNumberOfDimensions() const
{
  return mNumDim;
}

std::size_t AbstractLatticeModel::GetNumberOfDirections() const
{
  return mNumDir;
}

std::size_t AbstractLatticeModel::GetNx() const
{
  return mNx;
}

std::size_t AbstractLatticeModel::GetNy() const
{
  return mNy;
}

std::size_t AbstractLatticeModel::GetNumberOfNodes() const
{
  return mNumNodes;
}

double AbstractLatticeModel::GetPropagationSpeed() const
{
  return mC;
}

const std::vector<double>& AbstractLatticeModel::rGetWeight() const
{
  return mWeight;
}

const std::vector<int>& AbstractLatticeModel::rGetDistance() const
{
  return mDistance;
}

const std::vector<std::vector<double>>& AbstractLatticeModel::
    rGetDiscreteVelocity() const
{
  return mDiscreteVelocity;
}

const std::vector<std::vector<double>>& AbstractLatticeModel::
    rGetVelocity() const
{
  return mVelocity;
}

void AbstractLatticeModel::SetVelocity(
    const std::vector<std::vector<double>>& rVelocity)
{
  mVelocity = rVelocity;
}
}  // namespace iblbm
