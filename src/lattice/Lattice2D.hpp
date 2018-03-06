#ifndef SRC_LATTICE_LATTICE2D_HPP_
#define SRC_LATTICE_LATTICE2D_HPP_

#include <cstddef>
#include <vector>

#include "AbstractLatticeModel.hpp"

namespace iblbm
{
class Lattice2D : public AbstractLatticeModel
{
 public:
  Lattice2D(
      std::size_t nx
    , std::size_t ny
    , const std::vector<double>& r_initial_velocity);

  ~Lattice2D() = default;

  void SetVelocity(const std::vector<double>& r_initial_velocity);
};
}
#endif  // SRC_LATTICE_LATTICE2D_HPP_
