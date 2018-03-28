#include "Lattice2D.hpp"

#include <cassert>

#include "IblbmEnum.hpp"

namespace iblbm
{
Lattice2D::Lattice2D(
    std::size_t nx
  , std::size_t ny
  , const std::vector<double>& rInitialVelocity)
  : AbstractLatticeModel(2u, 9u, nx, ny)
{
  mWeight = {16.0 / 36.0,
             4.0 / 36.0, 4.0 / 36.0, 4.0 / 36.0, 4.0 / 36.0,
             1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0};
  auto nx_cast = static_cast<int>(nx);
  mDistance = {0,
               1, nx_cast, -1, -nx_cast,
               nx_cast + 1, nx_cast - 1, -nx_cast - 1, -nx_cast + 1};
  mDiscreteVelocity = {{0.0, 0.0},
                       {1.0, 0.0}, {0.0, 1.0}, {-1.0, 0.0}, {0.0, -1.0},
                       {1.0, 1.0}, {-1.0, 1.0}, {-1.0, -1.0}, {1.0, -1.0}};
  for (auto& r_direction : mDiscreteVelocity) {
    for (auto& r_elem : r_direction) r_elem *= mC;
  }
  Lattice2D::SetVelocity(rInitialVelocity);
}

Edge Lattice2D::DetermineOrientation(std::size_t n)
{
  const auto is_left = n % mNx == 0;
  const auto is_right = n % mNx == mNx - 1;
  const auto is_lower = n / mNx == 0;
  const auto is_upper = n / mNx == mNy - 1;
  auto edge = Edge::NOT_EDGE;
  if (is_right) edge = Edge::RIGHT;
  if (is_upper) edge = Edge::UPPER;
  if (is_left) edge = Edge::LEFT;
  if (is_lower) edge = Edge::LOWER;
  if (is_upper && is_right) edge = Edge::UPPER_RIGHT;
  if (is_upper && is_left) edge = Edge::UPPER_LEFT;
  if (is_lower && is_left) edge = Edge::LOWER_LEFT;
  if (is_lower && is_right) edge = Edge::LOWER_RIGHT;

  return edge;
}

void Lattice2D::SetVelocity(const std::vector<double>& rInitialVelocity)
{
  mVelocity.assign(mNumNodes, rInitialVelocity);
}
}  // namespace iblbm
