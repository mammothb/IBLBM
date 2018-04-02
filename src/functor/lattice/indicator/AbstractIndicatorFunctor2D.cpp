#include "AbstractIndicatorFunctor2D.hpp"

#include <iostream>

namespace iblbm
{
template<typename S>
AbstractIndicatorFunctor2D<S>::AbstractIndicatorFunctor2D()
  : AbstractFunctor<bool, S>{/*targetDimension=*/1, /*sourceDimension=*/2}
{}

//template <typename S>
//bool AbstractIndicatorFunctor2D<S>::operator()(const std::vector<S>& rInput)
//{
//  bool output {};
//  this->operator()(gsl::make_span(&output), rInput);
//  return output;
//}

// Explicit instantiation
template class AbstractIndicatorFunctor2D<double>;
}  // namespace iblbm
