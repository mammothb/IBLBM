#include "AbstractDynamicsInterface.hpp"

#include "CoreUtilities.hpp"
#include "Helpers.hpp"

namespace iblbm
{
template <typename T, template <typename U> class Lattice>
T AbstractDynamicsInterface<T, Lattice>::ComputeEquilibrium(
    gsl::index q
  , T rho
  , const std::vector<T>& rU
  , T uSqr) const
{
  return LbmHelper<T, Lattice>::ComputeEquilibrium(q, rho, rU, uSqr);
}

template <typename T, template <typename U> class Lattice>
void AbstractDynamicsInterface<T, Lattice>::InitializeEquilibrium(
    Cell<T, Lattice>& rCell
  , T rho
  , const std::vector<T>& rU)
{
  auto u_sqr = util::InnerProduct(rU, rU);
  for (gsl::index q = 0u; q < Lattice<T>::sQ; ++q)
      rCell[q] = ComputeEquilibrium(q, rho, rU, u_sqr);
}

// Explicit instantiation
template class AbstractDynamicsInterface<double, descriptor::D2Q9Descriptor>;
template class AbstractDynamicsInterface<double,
    descriptor::ForcedD2Q9Descriptor>;
}
