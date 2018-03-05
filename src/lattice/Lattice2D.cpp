#include "Lattice2D.hpp"

namespace iblbm
{
Lattice2D::Lattice2D(
    std::size_t nx
  , std::size_t ny)
  : AbstractLatticeModel(2u, 9u, nx, ny)
{
  weight_ = {16.0 / 36.0,
             4.0 / 36.0, 4.0 / 36.0, 4.0 / 36.0, 4.0 / 36.0,
             1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0};
  e_ = {{0.0, 0.0},
        {1.0, 0.0}, {0.0, 1.0}, {-1.0, 0.0}, {0.0, -1.0},
        {1.0, 1.0}, {-1.0, 1.0}, {-1.0, -1.0}, {1.0, -1.0}};
}

void Lattice2D::InitializeVelocity(
    const std::vector<double>& r_initial_velocity)
{
  u_.assign(num_nodes_, r_initial_velocity);
}
}  // namespace iblbm
