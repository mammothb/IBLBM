#include "Helpers.hpp"

namespace iblbm
{
template<typename T, template<typename U> class Lattice>
T BulkMomenta<T, Lattice>::ComputeRho(const Cell<T, Lattice>& rCell) const
{
  return LbmHelper<T, Lattice>::ComputeRho(rCell);
}

template<typename T, template<typename U> class Lattice>
void BulkMomenta<T, Lattice>::ComputeU(
    const Cell<T, Lattice>& rCell
  , std::vector<T>& rU) const
{
  T dummy_rho;
  LbmHelper<T, Lattice>::ComputeRhoAndU(rCell, dummy_rho, rU);
}
}  // namespace iblbm
