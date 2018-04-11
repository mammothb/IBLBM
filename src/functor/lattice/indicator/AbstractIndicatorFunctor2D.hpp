#ifndef SRC_FUNCTOR_LATTICE_INDICATOR_ABSTRACTINDICATORFUNCTOR2D_HPP_
#define SRC_FUNCTOR_LATTICE_INDICATOR_ABSTRACTINDICATORFUNCTOR2D_HPP_

#include "gsl/gsl"
#include "AbstractFunctor.hpp"
#include "Vector.hpp"

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
  /** Calls the implemented pure virtual operator() of the parent class */
  using AbstractFunctor<bool, S>::operator();

  /** \return Maximum vector of the domain */
  virtual const Vector2D<S>& rGetMin() const;

  /** \return Minimum vector of the domain */
  virtual const Vector2D<S>& rGetMax() const;

  /** \return Difference between maximum and minimum vector of the domain */
  virtual const Vector2D<S> GetRange() const;

  /**
   * Indicator specific function operator overload
   * \param Domain indicator i.e. TRUE iff the input lies within the
   *        described domain.
   */
  virtual void operator()(
      gsl::span<bool> output
    , const Vector2D<S>& rInput);

 protected:
  /** Constructor */
  AbstractIndicatorFunctor2D();
  /** Minimum vector of the domain */
  Vector2D<S> mMin;
  /** Maximum vector of the domain */
  Vector2D<S> mMax;
};
}  // namespace iblbm

#endif  // SRC_FUNCTOR_LATTICE_INDICATOR_ABSTRACTINDICATORFUNCTOR2D_HPP_
