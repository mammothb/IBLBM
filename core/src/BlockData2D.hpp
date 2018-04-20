#ifndef CORE_SRC_BLOCKDATA2D_HPP_
#define CORE_SRC_BLOCKDATA2D_HPP_

#include "BlockStructure2D.hpp"
#include "Serializable.hpp"

namespace iblbm
{
template<typename T> class Cuboid2D;

template<typename T, typename BaseType>
class BlockData2D
  : public BlockStructure2D
  , public Serializable
{
 public:
  /**
   * Constructor
   */
  BlockData2D();

  /**
   * Constructor from cuboid
   *
   * \param rCuboid reference to cuboid, to retrieve nx and ny information
   * \param dimension dimension of data in mpField
   */
  BlockData2D(
      const Cuboid2D<T>& rCuboid
    , std::size_t dimension = 1);

  /**
   * Constructor from nx and ny node count
   *
   * \param nx number of nodes in the x-direction
   * \param ny number of nodes in the y-direction
   * \param dimension dimension of data in mpField
   */
  BlockData2D(
      std::size_t nx
    , std::size_t ny
    , std::size_t dimension = 1);

  /**
   * Destructor
   */
  virtual ~BlockData2D();

  /**
   * Copy constructor
   */
  BlockData2D(const BlockData2D<T, BaseType>& rRhs);

  /**
   * Copy assignment
   */
  BlockData2D<T, BaseType>& operator=(const BlockData2D<T, BaseType>& rRhs);

  /**
   * Move constructor
   */
  BlockData2D(BlockData2D<T, BaseType>&& rRhs);

  /**
   * Move assignment
   * https://docs.microsoft.com/en-us/cpp/cpp/move-constructors-and-move-assignment-operators-cpp
   */
  BlockData2D<T, BaseType>& operator=(BlockData2D<T, BaseType>&& rRhs);

  /**
   * Swap method
   */
  void Swap(BlockData2D<T, BaseType>& rRhs);

  /**
   * Check if memory has already been allocated to mpRawData and mpField
   */
  bool IsConstructed() const;

  /**
   * Allocate memory to mpRawData and mpField if we have not already done so
   */
  void Construct();

  /**
   * Frees memory from mpRawData and mpField if we have allocated it
   */
  void Deconstruct();

  /**
   * Reset all memory to 0, expects that we have already allocated memory
   */
  void Reset();

  /**
   * read and write access to data at mpField[x][y][d]
   *
   * \param x x-th column
   * \param y y-th row
   * \param d d-th dimension
   */
  BaseType& rGetData(
      gsl::index x
    , gsl::index y
    , gsl::index d = 0);

  /**
   * read-only access to data at mpField[x][y][d]
   *
   * \param x x-th column
   * \param y y-th row
   * \param d d-th dimension
   */
  const BaseType& rGetData(
      gsl::index x
    , gsl::index y
    , gsl::index d = 0) const;

  /**
   * read and write access to data at mpRawData[i]
   *
   * \param i index
   */
  BaseType& operator[](gsl::index i);

  /**
   * read-only access to data at mpRawData[i]
   *
   * \param i index
   */
  const BaseType& operator[](gsl::index i) const;

  /**
   * \return max of data, for vector valued data it determines the max
   * component
   */
  BaseType GetMax();

  /**
   * \return min of data, for vector valued data it determines the min
   * component
   */
  BaseType GetMin();

  /** \return _rawData array */
  BaseType* pGetRawData() const;

  /** \return length of array mpRawData or equivalent mNx * mNy * mSize */
  virtual std::size_t GetDataSize() const;

  /** \return read-only access to mDimension */
  std::size_t GetDimension() const;

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

 protected:
  /**
   * Initialize mpRawData and mpField
   */
  void AllocateMemory();

  /**
   * Frees mpRawData and mpField
   */
  void ReleaseMemory();

  /** dimension of data element */
  std::size_t mDimension;
  /** Holds data as a 1D array */
  BaseType* mpRawData;
  /**
   * Pointer structure to a 3D data field. It can be interpreted as a 2D
   * matrix[xIndex, yIndex] with elements of dimension mDimension.
   * Those elements may be
   *   1. vector valued like velocity, distribution function
   *   2. scalar valued like density, pressure
   */
  BaseType*** mpField;
};
}  // namespace iblbm

#endif // CORE_SRC_BLOCKDATA2D_HPP_
