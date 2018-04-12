#include "ForcedBgkDynamics.hpp"

#include "gsl/gsl"
#include "Helpers.hpp"
#include "Exception.hpp"

namespace iblbm
{
template<typename T, template<typename U> class Lattice>
ForcedBgkDynamics<T, Lattice>::ForcedBgkDynamics(
    T relaxationTime
  , AbstractMomentaInterface<T, Lattice>& rMomenta)
  : AbstractDynamics<T, Lattice>(rMomenta),
    mOmega{T {1} / relaxationTime}
{
  // Ensure both that the constant sSizeOfForce is defined in ExternalField
  // and that it has the proper size
  Expects(Lattice<T>::sD ==
      Lattice<T>::ExternalField::sSizeOfForce);
}

template<typename T, template<typename U> class Lattice>
void ForcedBgkDynamics<T, Lattice>::Collide(
    Cell<T, Lattice>& rCell
  , LatticeStatistics<T>& rStatistics)
{
  T rho {0};
  std::vector<T> u(Lattice<T>::sD, T{});
  ComputeRhoAndU(rCell, rho, u);
  auto u_sqr = LbmHelper<T, Lattice>::BgkCollide(rCell, rho, u, mOmega);
  LbmHelper<T, Lattice>::AddExternalForce(rCell, u, mOmega, rho);
  rStatistics.IncrementStats(rho, u_sqr);
}

template<typename T, template<typename U> class Lattice>
void ForcedBgkDynamics<T, Lattice>::ComputeU(
    const Cell<T, Lattice>& rCell
  , std::vector<T>& rU) const
{
  T rho {0};
  this->mrMomenta.ComputeRhoAndU(rCell, rho, rU);
  for (gsl::index d = 0; d < Lattice<T>::sD; ++d)
      rU[d] += rCell.rGetExternal(msForceOffset + d) / T{2};
}

template<typename T, template<typename U> class Lattice>
void ForcedBgkDynamics<T, Lattice>::ComputeRhoAndU(
    const Cell<T, Lattice>& rCell
  , T& rRho
  , std::vector<T>& rU) const
{
  this->mrMomenta.ComputeRhoAndU(rCell, rRho, rU);
  for (gsl::index d = 0; d < Lattice<T>::sD; ++d)
      rU[d] += rCell.rGetExternal(msForceOffset + d) / T{2};
}

template<typename T, template<typename U> class Lattice>
T ForcedBgkDynamics<T, Lattice>::GetOmega() const
{
  return mOmega;
}

// Explicit instantiation
template class ForcedBgkDynamics<double, descriptor::ForcedD2Q9Descriptor>;
}  // namespace iblbm
