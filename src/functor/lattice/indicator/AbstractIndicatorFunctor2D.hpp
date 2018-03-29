#ifndef SRC_FUNCTOR_LATTICE_ABSTRACTINDICATORFUNCTOR2D_HPP_
#define SRC_FUNCTOR_LATTICE_ABSTRACTINDICATORFUNCTOR2D_HPP_

#include "gsl/gsl"
#include "AbstractFunctor.hpp"

namespace iblbm
{
/**
 * AbstractIndicatorFunctor2D is an application from
 * $ Omega subset R^3 to {0, 1} $.
 * \param _myMin holds minimal(component wise) vector of the domain $ Omega $
 * \param _myMax holds maximal(component wise) vector of the domain $ Omega $
 */
template<typename S>
class AbstractIndicatorFunctor2D : public AbstractFunctor<bool, S>
{
 public:
  using AbstractFunctor<bool, S>::operator();

  /**
   * Indicator specific function operator overload
   * \return Domain indicator i.e. TRUE iff the input lies within the
   *         described domain.
   */
  virtual bool operator()(const std::vector<S>& rInput);

 protected:
  /** Constructor */
  AbstractIndicatorFunctor2D();
  /** Minimum vector of the domain */
  std::vector<S> mMin;
  /** Maximum vector of the domain */
  std::vector<S> mMax;
};
}  // namespace iblbm

#endif  // SRC_FUNCTOR_LATTICE_ABSTRACTINDICATORFUNCTOR2D_HPP_
