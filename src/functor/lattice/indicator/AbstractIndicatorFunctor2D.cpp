#include "AbstractIndicatorFunctor2D.hpp"

#include <iostream>

namespace iblbm
{
template<typename S>
AbstractIndicatorFunctor2D<S>::AbstractIndicatorFunctor2D()
  : AbstractFunctor<bool, S>{/*targetDimension=*/1, /*sourceDimension=*/2}
{}

template<typename S>
const Vector2D<S>& AbstractIndicatorFunctor2D<S>::rGetMin() const
{
  return mMin;
}

template<typename S>
const Vector2D<S>& AbstractIndicatorFunctor2D<S>::rGetMax() const
{
  return mMax;
}

template<typename S>
const Vector2D<S> AbstractIndicatorFunctor2D<S>::GetRange() const
{
  return mMax - mMin;
}

template <typename S>
void AbstractIndicatorFunctor2D<S>::operator()(
    gsl::span<bool> output
  , const Vector2D<S>& rInput)
{
  this->operator()(output, rInput.ConvertToStdVector());
}

// Explicit instantiation
template class AbstractIndicatorFunctor2D<double>;
}  // namespace iblbm
