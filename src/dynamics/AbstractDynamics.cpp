#include "AbstractDynamics.hpp"

namespace iblbm
{
template <typename T, template <typename U> class Lattice>
AbstractDynamics<T, Lattice>::AbstractDynamics(
    AbstractMomentaInterface<T, Lattice>& rMomenta)
  : mrMomenta{rMomenta}
{}

template <typename T, template <typename U> class Lattice>
T AbstractDynamics<T, Lattice>::ComputeRho(
    const Cell<T, Lattice>& rCell) const
{
  return mrMomenta.ComputeRho(rCell);
}

template <typename T, template <typename U> class Lattice>
void AbstractDynamics<T, Lattice>::ComputeU(
    const Cell<T, Lattice>& rCell
  , std::vector<T>& rU) const
{
  return mrMomenta.ComputeU(rCell, rU);
}

template <typename T, template <typename U> class Lattice>
void AbstractDynamics<T, Lattice>::ComputeRhoAndU(
    const Cell<T, Lattice>& rCell
  , T& rRho
  , std::vector<T>& rU) const
{
  mrMomenta.ComputeRhoAndU(rCell, rRho, rU);
}

// Explicit instantiation
template class AbstractDynamics<double, descriptor::D2Q9Descriptor>;
template class AbstractDynamics<double, descriptor::ForcedD2Q9Descriptor>;
}

