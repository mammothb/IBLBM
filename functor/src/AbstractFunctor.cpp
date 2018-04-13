#include "AbstractFunctor.hpp"

namespace iblbm
{
template<typename T, typename S>
AbstractFunctor<T, S>::AbstractFunctor(
    std::size_t targetDimension
  , std::size_t sourceDimension)
  : mName{""},
    mTargetDimension{targetDimension},
    mSourceDimension{sourceDimension}
{}

template<typename T, typename S>
AbstractFunctor<T, S>::~AbstractFunctor()
{}

template<typename T, typename S>
std::string& AbstractFunctor<T, S>::rGetName()
{
  return mName;
}

template<typename T, typename S>
const std::string& AbstractFunctor<T, S>::rGetName() const
{
  return mName;
}

template<typename T, typename S>
std::size_t AbstractFunctor<T, S>::GetSourceDimension() const
{
  return mSourceDimension;
}

template<typename T, typename S>
std::size_t AbstractFunctor<T, S>::GetTargetDimension() const
{
  return mTargetDimension;
}

template<typename T, typename S>
void AbstractFunctor<T, S>::operator()(gsl::span<T> output)
{
  std::vector<S> input;
  operator()(output, input);
}

template<typename T, typename S>
void AbstractFunctor<T, S>::operator()(
    gsl::span<T> output
  , S input0)
{
  std::vector<S> input {input0};
  operator()(output, input);
}

template<typename T, typename S>
void AbstractFunctor<T, S>::operator()(
    gsl::span<T> output
  , S input0
  , S input1)
{
  std::vector<S> input {input0, input1};
  operator()(output, input);
}

template<typename T, typename S>
void AbstractFunctor<T, S>::operator()(
    gsl::span<T> output
  , S input0
  , S input1
  , S input2)
{
  std::vector<S> input {input0, input1, input2};
  operator()(output, input);
}

template<typename T, typename S>
void AbstractFunctor<T, S>::operator()(
    gsl::span<T> output
  , S input0
  , S input1
  , S input2
  , S input3)
{
  std::vector<S> input {input0, input1, input2, input3};
  operator()(output, input);
}

// Explicit instantiation
template class AbstractFunctor<int, int>;

template class AbstractFunctor<bool, double>;
template class AbstractFunctor<bool, int>;
template class AbstractFunctor<double, int>;
template class AbstractFunctor<double, double>;
}  // namespace iblbm
