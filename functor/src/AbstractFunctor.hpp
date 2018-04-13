#ifndef SRC_FUNCTOR_ABSTRACTFUNCTOR_HPP_
#define SRC_FUNCTOR_ABSTRACTFUNCTOR_HPP_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "gsl/gsl"

namespace iblbm
{
/**
 * AbstractFunctor is a base class, that can represent continuous as well as
 * discrete functions.
 * Please take care about the source and target dimensions in the constructor
 *                F: S^m -> T^n (S=source, T=target)
 */
template<typename T, typename S>
class AbstractFunctor
{
 public:
  /** Virtual destructor to allow inheriting */
  virtual ~AbstractFunctor();

  virtual void operator()(
      gsl::span<T> output
    , const std::vector<S>& rInput) = 0;

  /** \return read and write access to mName */
  std::string& rGetName();

  /** \return read-only access to mName */
  const std::string& rGetName() const;

  /** \return source dimension */
  std::size_t GetSourceDimension() const;

  /** \return target dimension */
  std::size_t GetTargetDimension() const;

  /**
   * Wrapper that call the pure virtual
   * operator()(std::vector<T>& rOutput, const std::vector<S>& rInput) from
   * above. It is aimed that it even calls the implemented pure virtual
   * operator() of derived classes
   */
  void operator()(gsl::span<T> output);
  void operator()(
      gsl::span<T> output
    , S input0);
  void operator()(
      gsl::span<T> output
    , S input0
    , S input1);
  void operator()(
      gsl::span<T> output
    , S input0
    , S input1
    , S input2);
  void operator()(
      gsl::span<T> output
    , S input0
    , S input1
    , S input2
    , S input3);

  /** Memory management, frees resources */
  std::shared_ptr<AbstractFunctor<T, S>> pCalcClass;

 protected:
  /**
   * Constructor
   *
   * \param targetDimension dimension of target category
   * \param sourceDimension dimension of source category
   */
  AbstractFunctor(
      std::size_t targetDimension
    , std::size_t sourceDimention);

 private:
  /** Functor name, e.g., velocity, pressure */
  std::string mName;
  /** Target dimension */
  std::size_t mTargetDimension;
  /** Source dimension */
  std::size_t mSourceDimension;
};
}  // namespace iblbm

#endif  // SRC_FUNCTOR_ABSTRACTFUNCTOR_HPP_
