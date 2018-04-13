#include "MomentumExchangeBounceBackBoundary.hpp"

#include <utility>

#include "gsl/gsl"
#include "Cell.hpp"

namespace iblbm
{
template<typename T, template<typename U> class Lattice>
MomentumExchangeBounceBackBoundary<T, Lattice>::
    MomentumExchangeBounceBackBoundary()
  : mRho{0}
{}

template<typename T, template<typename U> class Lattice>
MomentumExchangeBounceBackBoundary<T, Lattice>::
    MomentumExchangeBounceBackBoundary(T rho)
  : mRho{rho}
{}

template<typename T, template<typename U> class Lattice>
void MomentumExchangeBounceBackBoundary<T, Lattice>::Collide(
    Cell<T, Lattice>& rCell
  , LatticeStatistics<T>& /*rStatistics*/)
{
  // Ignore element 0
  for (gsl::index q = 1; q <= Lattice<T>::sQ / 2; ++q)
      std::swap(rCell[q], rCell[Lattice<T>::sOpposite[q]]);
}

template<typename T, template<typename U> class Lattice>
T MomentumExchangeBounceBackBoundary<T, Lattice>::ComputeRho(
    const Cell<T, Lattice>& /*rCell*/) const
{
  return mRho;
}

template<typename T, template<typename U> class Lattice>
void MomentumExchangeBounceBackBoundary<T, Lattice>::ComputeU(
    const Cell<T, Lattice>& /*rCell*/
  , std::vector<T>& rU) const
{
  for (gsl::index d = 0; d < Lattice<T>::sD; ++d) rU[d] = T{};
}

template<typename T, template<typename U> class Lattice>
void MomentumExchangeBounceBackBoundary<T, Lattice>::ComputeRhoAndU(
    const Cell<T, Lattice>& rCell
  , T& rRho
  , std::vector<T>& rU) const
{
  rRho = ComputeRho(rCell);
  ComputeU(rCell, rU);
}

template<typename T, template<typename U> class Lattice>
T MomentumExchangeBounceBackBoundary<T, Lattice>::GetOmega() const
{
  return T{};
}

// Explicit Instantiation
template class MomentumExchangeBounceBackBoundary<double,
    descriptor::D2Q9Descriptor>;
template class MomentumExchangeBounceBackBoundary<double,
    descriptor::ForcedD2Q9Descriptor>;
}  // namespace iblbm
