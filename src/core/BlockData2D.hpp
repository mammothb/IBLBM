#ifndef SRC_CORE_BLOCKDATA2D_HPP_
#define SRC_CORE_BLOCKDATA2D_HPP_

#include "BlockStructure2D.hpp"
#include "Serializable.hpp"

namespace iblbm
{
template<typename T, typename BaseType>
class BlockData2D
  : public BlockStructure2D
  , public Serializable
{
 public:
  BlockData2D();

  /** \return length of array mpRawData or equivalent mNx * mNy * mSize */
  virtual std::size_t GetDataSize() const;

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
  /** dimension of data element */
  std::size_t mSize;
  /** Holds data as a 1D array */
  BaseType* mpRawData;
  /**
   * Pointer structure to a 3D data field. It can be interpreted as a 2D
   * matrix[xIndex, yIndex] with elements of dimension mSize.
   * Those elements may be
   *   1. vector valued like velocity, distribution function
   *   2. scalar valued like density, pressure
   *
   */
  BaseType*** mpField;
};
}  // namespace iblbm

#endif // SRC_CORE_BLOCKDATA2D_HPP_
