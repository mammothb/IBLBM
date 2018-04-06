#include "Cell.hpp"

namespace iblbm
{
template <typename T, class Descriptor>
CellBase<T, Descriptor>::CellBase()
  : mDF{std::vector<T>(Descriptor::sQ, T{})}
{}

template <typename T, template <typename U> class Lattice>
Cell<T, Lattice>::Cell()
  : mpDynamics{nullptr}
{
  Cell<T, Lattice>::InitializeExternal();
}

template <typename T, template <typename U> class Lattice>
Cell<T, Lattice>::Cell(AbstractDynamicsInterface<T, Lattice>* pDynamics)
  : mpDynamics{pDynamics}
{
  Cell<T, Lattice>::InitializeExternal();
}

template <typename T, template <typename U> class Lattice>
AbstractDynamicsInterface<T, Lattice>* Cell<T, Lattice>::pGetDynamics()
{
  return mpDynamics;
}

template <typename T, template <typename U> class Lattice>
const AbstractDynamicsInterface<T, Lattice>*
    Cell<T, Lattice>::pGetDynamics() const
{
  return mpDynamics;
}

template<typename T, template<typename U> class Lattice>
void Cell<T,Lattice>::InitializeExternal()
{
  mExternal.SetData(std::vector<T>(Lattice<T>::ExternalField::sNumScalars,
      T{}));
}

// Explicit instantiation
template class Cell<double, descriptor::D2Q9Descriptor>;
template class Cell<double, descriptor::ForcedD2Q9Descriptor>;
}
