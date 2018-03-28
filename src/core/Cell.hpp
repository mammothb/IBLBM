#ifndef SRC_CORE_CELL_HPP_
#define SRC_CORE_CELL_HPP_

#include <vector>

#include "gsl/gsl"
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
   * Default constructor. Initializes mDF to all zeros
   */
  CellBase();

  /**
   * Virtual destructor to allow inheritance
   */
  virtual ~CellBase() = default;

  /**
   * Read-write access to distribution functions.
   *
   * \param iPop index of the accessed distribution function
   */
  T& operator[](const gsl::index& rIndex)
  {
    IBLBM_PRECONDITION(rIndex < Descriptor::sQ);
    return mDF[rIndex];
  }

  /**
   * Read-only access to distribution functions.
   *
   * \param iPop index of the accessed distribution function
   */
  const T& operator[](const gsl::index& rIndex) const
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
  T& rGetExternal(gsl::index index)
  {
    IBLBM_PRECONDITION(index < Lattice<T>::ExternalField::sNumScalars);
    return mExternal[index];
  }

  /** \return a const reference to an external field */
  const T& rGetExternal(gsl::index index) const
  {
    IBLBM_PRECONDITION(index < Lattice<T>::ExternalField::sNumScalars);
    return mExternal[index];
  }

  /**
   * Set the external field data
   *
   * \param offset index offset in the data vector
   * \param rExternalField vector containing data for external field
   */
//  void SetExternalField(
//      std::size_t offset
//    , std::size_t fieldSize
//    , const std::vector<T>& rExternalField)
  void SetExternalField(
      gsl::index offset
    , const std::vector<T>& rExternalField)
  {
    IBLBM_PRECONDITION(offset + rExternalField.size() <=
        Lattice<T>::ExternalField::sNumScalars);
    for (gsl::index i = 0; i < rExternalField.size(); ++i)
        mExternal[offset + i] = rExternalField[i];
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

  /** Initialize data members of external field */
  void InitializeExternal();
};

}  // namespace iblbm

#endif  // SRC_CORE_CELL_HPP_
