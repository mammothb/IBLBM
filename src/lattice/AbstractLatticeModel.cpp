#include "AbstractLatticeModel.hpp"

#include <cstddef>

namespace iblbm
{
AbstractLatticeModel::AbstractLatticeModel(
    std::size_t num_dim
  , std::size_t num_dir
  , std::size_t nx
  , std::size_t ny)
  : num_dim_(num_dim),
    num_dir_(num_dir),
    nx_(nx),
    ny_(ny),
    num_nodes_(nx * ny),
    c_(1.0)
{}

std::size_t AbstractLatticeModel::GetNumberOfDimensions() const
{
  return num_dim_;
}

std::size_t AbstractLatticeModel::GetNumberOfDirections() const
{
  return num_dir_;
}

std::size_t AbstractLatticeModel::GetNx() const
{
  return nx_;
}

std::size_t AbstractLatticeModel::GetNy() const
{
  return ny_;
}

std::size_t AbstractLatticeModel::GetNumberOfNodes() const
{
  return num_nodes_;
}

double AbstractLatticeModel::GetPropagationSpeed() const
{
  return c_;
}

const std::vector<double>& AbstractLatticeModel::rGetWeight() const
{
  return weight_;
}

const std::vector<std::vector<double>>& AbstractLatticeModel::
    rGetDiscreteVelocity() const
{
  return e_;
}

const std::vector<std::vector<double>>& AbstractLatticeModel::
    rGetVelocity() const
{
  return u_;
}
}  // namespace iblbm
