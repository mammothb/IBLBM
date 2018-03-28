#ifndef SRC_LATTICE_LATTICE2D_HPP_
#define SRC_LATTICE_LATTICE2D_HPP_

#include <cstddef>
#include <vector>

#include "AbstractLatticeModel.hpp"
#include "IblbmEnum.hpp"

namespace iblbm
{
class Lattice2D : public AbstractLatticeModel
{
 public:
  Lattice2D(
      std::size_t nx
    , std::size_t ny
    , const std::vector<double>& rInitialVelocity);

  ~Lattice2D() = default;

  Edge DetermineOrientation(std::size_t n);

  void SetVelocity(const std::vector<double>& rInitialVelocity);
};
}
#endif  // SRC_LATTICE_LATTICE2D_HPP_
