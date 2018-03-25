#ifndef SRC_CORE_CELL_HPP_
#define SRC_CORE_CELL_HPP_

#include <vector>

#include "AbstractDynamicsInterface.hpp"
#include "Descriptor.hpp"
#include "IblbmDebug.hpp"

namespace iblbm
{
template<typename T, class Descriptor>
class CellBase
{
 public:
  /**
   * Read-write access to distribution functions.
   *
   * \param iPop index of the accessed distribution function
   */
  T& operator[](const std::size_t& rIndex)
  {
    IBLBM_PRECONDITION(rIndex < Descriptor::sQ);
    return mDF[rIndex];
  }

  /**
   * Read-only access to distribution functions.
   *
   * \param iPop index of the accessed distribution function
   */
  const T& operator[](const std::size_t& rIndex) const
  {
    IBLBM_PRECONDITION(rIndex < Descriptor::sQ);
    return mDF[rIndex];
  }

 protected:
  /** Distribution function */
  std::vector<T> mDF;
};

template<typename T, template<typename U> class Lattice>
class Cell : public CellBase<T, typename Lattice<T>::BaseDescriptor>
{
 public:
  /** Additional per-cell scalars for external fields, e.g. forces */
  typedef descriptor::ExternalFieldVector<T,
      typename Lattice<T>::ExternalField> External;

  /** Default constructor */
  Cell();

  /**
   * Constructor
   *
   * \param pDynamics dynamics of the cell
   */
  Cell(AbstractDynamicsInterface<T, Lattice>* pDynamics);

  /** \return a reference to an external field */
  T& rGetExternal(std::size_t index)
  {
    IBLBM_PRECONDITION(index < Lattice<T>::ExternalField::sNumScalars);
    return mExternal[index];
  }

  /** \return a const reference to an external field */
  const T& rGetExternal(std::size_t index) const
  {
    IBLBM_PRECONDITION(index < Lattice<T>::ExternalField::sNumScalars);
    return mExternal[index];
  }

  /** \return a non-modifiable pointer to the dynamics */
  const AbstractDynamicsInterface<T, Lattice>* pGetDynamics() const;
  /** \return a modifiable pointer to the dynamics */
  AbstractDynamicsInterface<T, Lattice>* pGetDynamics();

 private:
  /** External field */
  External mExternal;
  /** Pointer to cell dynamics */
  AbstractDynamicsInterface<T, Lattice>* mpDynamics;

  /** Initialize the distribution function with default values of T */
  void InitializeDistributionFunction();
  /** Initialize data members of external field */
  void InitializeExternal();
};

}  // namespace iblbm

//#include "Cell.ipp"

#endif  // SRC_CORE_CELL_HPP_
