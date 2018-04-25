#include "Cell.hpp"

namespace iblbm
{
template<typename T, class Descriptor>
CellBase<T, Descriptor>::CellBase()
{
  InitializeDistributionFunction();
}

template<typename T, class Descriptor>
void CellBase<T, Descriptor>::InitializeDistributionFunction()
{
  for (gsl::index q {0}; q < Descriptor::sQ; ++q) mDF[q] = T{};
}

template<typename T, template<typename U> class Lattice>
Cell<T, Lattice>::Cell()
  : mpDynamics{nullptr}
{
  Cell<T, Lattice>::InitializeExternal();
}

template<typename T, template<typename U> class Lattice>
Cell<T, Lattice>::Cell(AbstractDynamicsInterface<T, Lattice>* pDynamics)
  : mpDynamics{pDynamics}
{
  Cell<T, Lattice>::InitializeExternal();
}

template<typename T, template<typename U> class Lattice>
AbstractDynamicsInterface<T, Lattice>* Cell<T, Lattice>::pGetDynamics()
{
  return mpDynamics;
}

template<typename T, template<typename U> class Lattice>
const AbstractDynamicsInterface<T, Lattice>*
    Cell<T, Lattice>::pGetDynamics() const
{
  return mpDynamics;
}

template<typename T, template<typename U> class Lattice>
void Cell<T, Lattice>::InitializeExternal()
{
  for (gsl::index i {0}; i < Lattice<T>::ExternalField::sNumScalars; ++i)
      *(mExternal[i]) = T{};
}

template<typename T, template<typename U> class Lattice>
std::size_t Cell<T, Lattice>::GetNumBlock() const
{
  return Lattice<T>::ExternalField::sNumScalars > 0 ? 2 : 1;
}

template<typename T, template<typename U> class Lattice>
std::size_t Cell<T, Lattice>::GetSerializableSize() const
{
  return Lattice<T>::sQ * sizeof(T) +  // distribution function
      Lattice<T>::ExternalField::sNumScalars * sizeof(T);  // externals
}

template<typename T, template<typename U> class Lattice>
bool* Cell<T, Lattice>::pGetBlock(
    gsl::index blockIndex
  , std::size_t& rBlockSize
  , const bool /*isLoad*/)
{
  gsl::index current_block_index {0};
  bool* p_data {nullptr};
  this->RegisterVar(blockIndex, rBlockSize, current_block_index, p_data,
      this->mDF[0], Lattice<T>::sQ);
  if (Lattice<T>::ExternalField::sNumScalars > 0) {
    this->RegisterVar(blockIndex, rBlockSize, current_block_index, p_data,
        *(mExternal[0]), Lattice<T>::ExternalField::sNumScalars);
  }
  return p_data;
}

// Explicit instantiation
template class Cell<double, descriptor::D2Q9Descriptor>;
template class Cell<double, descriptor::ForcedD2Q9Descriptor>;
}  // namespace iblbm
