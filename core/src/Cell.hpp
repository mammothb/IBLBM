#ifndef CORE_SRC_CELL_HPP_
#define CORE_SRC_CELL_HPP_

#include <vector>

#include "gsl/gsl"
#include "AbstractDynamicsInterface.hpp"
#include "Descriptor.hpp"
#include "Exception.hpp"
#include "Serializable.hpp"

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
    Expects(rIndex < Descriptor::sQ);
    return mDF[rIndex];
  }

  /**
   * Read-only access to distribution functions.
   *
   * \param iPop index of the accessed distribution function
   */
  const T& operator[](const gsl::index& rIndex) const
  {
    Expects(rIndex < Descriptor::sQ);
    return mDF[rIndex];
  }

 protected:
  /** Initialize distribution function */
  void InitializeDistributionFunction();

  /** Distribution function */
  T mDF[Descriptor::sQ];
};

template<typename T, template<typename U> class Lattice>
class Cell
  : public CellBase<T, typename Lattice<T>::BaseDescriptor>
  , public Serializable
{
 public:
  /** Additional per-cell scalars for external fields, e.g. forces */
  typedef descriptor::ExternalFieldArray<T,
      typename Lattice<T>::ExternalField> External;

  /** Default constructor */
  Cell();

  /**
   * Constructor
   *
   * \param pDynamics dynamics of the cell
   */
  explicit Cell(AbstractDynamicsInterface<T, Lattice>* pDynamics);

  /** \return a reference to an external field */
  T* pGetExternal(gsl::index index)
  {
    Expects(index < Lattice<T>::ExternalField::sNumScalars);
    return mExternal[index];
  }

  /** \return a const reference to an external field */
  const T* pGetExternal(gsl::index index) const
  {
    Expects(index < Lattice<T>::ExternalField::sNumScalars);
    return mExternal[index];
  }

  /** \return a modifiable pointer to the dynamics */
  AbstractDynamicsInterface<T, Lattice>* pGetDynamics();

  /** \return a non-modifiable pointer to the dynamics */
  const AbstractDynamicsInterface<T, Lattice>* pGetDynamics() const;

  /** \return Number of data blocks for the serializable interface */
  std::size_t GetNumBlock() const override;

  /** \return the binary size of the data to be saved */
  std::size_t GetSerializableSize() const override;

  /**
   * \return a pointer to the memory of the current block and its size for
   * the serializable interface
   *
   * \param blockIndex Index of the block to be returned
   * \param blockSize Reference to the size of the returned block
   * \param isLoad flag to decide if we are loading or saving
   *
   * \return Pointer to the current block
   */
  bool* pGetBlock(
      gsl::index blockIndex
    , std::size_t& rBlockSize
    , const bool isLoad);

 private:
  /** Initialize data members of external field */
  void InitializeExternal();

  /** External field */
  External mExternal;
  /** Pointer to cell dynamics */
  AbstractDynamicsInterface<T, Lattice>* mpDynamics;
};

}  // namespace iblbm

#endif  // CORE_SRC_CELL_HPP_
