#include "LatticeStatistics.hpp"

namespace iblbm
{
template<typename T>
LatticeStatistics<T>::LatticeStatistics()
  : mInternalAverageValues{std::vector<T>(NUM_AVG_VALUE, T{})},
    mInternalMaxValues{std::vector<T>(NUM_MAX_VALUE, T{})},
    mInternalNumCells{0}
{}

template<typename T>
LatticeStatistics<T>::~LatticeStatistics()
{}

template<typename T>
void LatticeStatistics<T>::IncrementStats(
    T rho
  , T uSqr)
{
  mInternalAverageValues[AVG_RHO] += rho;
  mInternalAverageValues[AVG_ENERGY] += uSqr;
  if (uSqr > mInternalMaxValues[MAX_U]) mInternalMaxValues[MAX_U] = uSqr;
  ++mInternalNumCells;
}

// Explicit instantiation
template class LatticeStatistics<double>;
}  // namespace iblbm
