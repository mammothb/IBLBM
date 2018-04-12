#include "BlockStructure2D.hpp"

namespace iblbm
{
BlockStructure2D::BlockStructure2D(
    std::size_t nx
  , std::size_t ny)
  : mNx{nx},
    mNy{ny}
{}

std::size_t BlockStructure2D::GetNx() const
{
  return mNx;
}

std::size_t BlockStructure2D::GetNy() const
{
  return mNy;
}
}  // namespace iblbm
