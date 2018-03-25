#include "AbstractDynamicsInterface.hpp"
#include "Helpers.hpp"

namespace iblbm
{
template <typename T, template <typename U> class Lattice>
T AbstractDynamicsInterface<T, Lattice>::ComputeEquilibrium(
    std::size_t q
  , T rho
  , const std::vector<T>& rU
  , T uSqr) const
{
  return LbmHelper<T, Lattice>::ComputeEquilibrium(q, rho, rU, uSqr);
}

// Explicit instantiation
template class AbstractDynamicsInterface<double, descriptor::D2Q9Descriptor>;
}
