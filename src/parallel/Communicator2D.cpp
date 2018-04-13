#include "Communicator2D.hpp"

#include "SuperStructure2D.hpp"

namespace iblbm
{
template<typename T>
Communicator2D<T>::Communicator2D(SuperStructure2D<T>& rSuperStructure)
  : mrSuperStructure{rSuperStructure},
    mNumCuboid{0},
    mIsInitialized{false}
{}

template<typename T>
void Communicator2D<T>::InitializeNeighborhood()
{
  mNumCuboid = mrSuperStructure.rGetCuboidGeometry().GetNumberOfCuboids();
}

// explicit instantiation
template class Communicator2D<double>;
}  // namespace iblbm
