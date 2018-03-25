#include "Cell.hpp"

namespace iblbm
{

template <typename T, template <typename U> class Lattice>
Cell<T, Lattice>::Cell()
  : mpDynamics(nullptr)
{
  Cell<T, Lattice>::InitializeDistributionFunction();
  Cell<T, Lattice>::InitializeExternal();
}

template <typename T, template <typename U> class Lattice>
Cell<T, Lattice>::Cell(AbstractDynamicsInterface<T, Lattice>* pDynamics)
  : mpDynamics(pDynamics)
{
  Cell<T, Lattice>::InitializeDistributionFunction();
  Cell<T, Lattice>::InitializeExternal();
}

template <typename T, template <typename U> class Lattice>
const AbstractDynamicsInterface<T, Lattice>*
    Cell<T, Lattice>::pGetDynamics() const
{
  return mpDynamics;
}

template <typename T, template <typename U> class Lattice>
AbstractDynamicsInterface<T, Lattice>* Cell<T, Lattice>::pGetDynamics()
{
  return mpDynamics;
}

template <typename T, template <typename U> class Lattice>
void Cell<T, Lattice>::InitializeDistributionFunction()
{
  this->mDF.assign(Lattice<T>::sQ, T());
}

template<typename T, template<typename U> class Lattice>
void Cell<T,Lattice>::InitializeExternal()
{
  mExternal.SetData(std::vector<T>(Lattice<T>::ExternalField::sNumScalars,
      T()));
}

// Explicit instantiation
template class Cell<double, descriptor::D2Q9Descriptor>;
}
