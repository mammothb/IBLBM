#include "BulkMomenta.hpp"

#include "Descriptor.hpp"
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
  T dummy_rho {0};
  LbmHelper<T, Lattice>::ComputeRhoAndU(rCell, dummy_rho, rU);
}

// Explicit instantiation
template class BulkMomenta<double, descriptor::D2Q9Descriptor>;
template class BulkMomenta<double, descriptor::ForcedD2Q9Descriptor>;
}  // namespace iblbm
