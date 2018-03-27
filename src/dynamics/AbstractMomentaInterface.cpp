#include "AbstractMomentaInterface.hpp"
#include "Descriptor.hpp"

namespace iblbm
{
template<typename T, template<typename U> class Lattice>
void AbstractMomentaInterface<T, Lattice>::ComputeRhoAndU(
    const Cell<T, Lattice>& rCell
  , T& rRho
  , std::vector<T>& rU) const
{
  rRho = this->ComputeRho(rCell);
  this->ComputeU(rCell, rU);
}

// Explicit instantiation
template class AbstractMomentaInterface<double, descriptor::D2Q9Descriptor>;
template class AbstractMomentaInterface<double,
    descriptor::ForcedD2Q9Descriptor>;
}  // namespace iblbm

