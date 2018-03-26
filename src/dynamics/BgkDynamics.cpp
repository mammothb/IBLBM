#include "BgkDynamics.hpp"
#include "Descriptor.hpp"
#include "Helpers.hpp"

namespace iblbm
{
template<typename T, template<typename U> class Lattice>
BgkDynamics<T, Lattice>::BgkDynamics(
    T relaxationTime
  , AbstractMomentaInterface<T, Lattice>& rMomenta)
  : AbstractDynamics<T, Lattice>(rMomenta),
    mOmega(1.0 / relaxationTime)
{}

template<typename T, template<typename U> class Lattice>
void BgkDynamics<T, Lattice>::Collide(Cell<T, Lattice>& rCell)
{
  auto rho = T();
  std::vector<T> u(Lattice<T>::sD, T());
  this->mrMomenta.ComputeRhoAndU(rCell, rho, u);
  auto u_sqr = LbmHelper<T, Lattice>::BgkCollide(rCell, rho, u, mOmega);
}

template<typename T, template<typename U> class Lattice>
T BgkDynamics<T, Lattice>::GetOmega() const
{
  return mOmega;
}

// Explicit instantiation
template class BgkDynamics<double, descriptor::D2Q9Descriptor>;
}  // namespace iblbm
