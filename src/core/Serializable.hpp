#ifndef SRC_CORE_SERIALIZABLE_HPP_
#define SRC_CORE_SERIALIZABLE_HPP_

#include <map>

#include "gsl/gsl"

namespace iblbm
{
/**
 * Base class for serializable objects of _constant size_. For
 * _dynamic size_ use 'BufferSerializable'.
 * All serializable classes have to implement their individual 'GetBlock()'
 * method. An individual 'GetNblock()' method must also be provided.
 * An individual 'GetSerializableSize()' method should also be provided for
 * efficiency reasons.
 *
 * The 'SumNblock' and 'SumSerializableSize' operator structs can be used for
 * accumulation of 'GetNblock()' methods and 'GetSerializableSize()'
 * respectively, e.g. with an array or a 'std::vector<Serializable>'.
 *
 * All 'Serializable' subclasses with _dynamic size_ (unknown at compile
 * time, e.g. holding 'std::vector' or 'std::map' members) have to inherit
 * from 'BufferSerializable'. _Note: If the dynamic size is computable
 * through __constant__ values (see 'BlockLattice2D'), the 'Serializable'
 * does not need to be a 'BufferSerializable'.
 *
 * ### The Basic Serialization Concept ###
 *
 * Any serializable class inherits from either 'Serializable' or
 * 'BufferSerializable' (if it contains _dynamic-sized_ member variables) and
 * has to implement its individual 'GetBlock()' method.
 *
 * 'GetBlock()' is called by the 'Serializer' repeatedly with an increasing
 * counter 'blockIndex'. 'GetBlock()' returns a '(bool*)' address to the _i-th_
 * data block and fills 'sizeBlock' with the corresponding size. As long as
 * 'GetBlock()' does not return a 'nullptr', 'blockIndex' is increased and
 * 'GetBlock()' is called again by 'Serializer'.
 *
 * It is _strongly recommended_ (and __obligatory__ for the correct usage of
 * 'Register' methods) to define 'std::size_t rCurrentBlockIndex = 0;' within
 * 'GetBlock()'. 'rCurrentBlockIndex' will be increased by the 'register' methods
 * by the number of blocks they occupy.
 *
 * For user's convenience the 'Serializable' class provides 'register'
 * methods for _data of constant size_:
 * Method                             | Suitable for
 * -----------------------------------| --------------------------
 * 'RegisterVar()'                    | Primitive data types and arrays of
 *                                    | those (e.g. 'int', 'double',
 *                                    | 'std::string', ...)
 * 'RegisterConstSizeSerializable()'  | Constant-sized 'Serializable' object
 * 'RegisterConstSizeSerializables()' | Array of constant-sized
 *                                    | 'Serializable' objects
 *
 * - In 'RegisterVar()', 'rCurrentBlockIndex' is counted up by 1
 * - 'arrays' are treated as one block of size 'arrayLength *
 *   sizeof(DataType)
 * - In 'RegisterConstSizeSerializable()', 'rCurrentBlockIndex' is increased by
 *   'GetNblock()' of the given 'Serializable'.
 * - In 'RegisterConstSizeSerializables()', 'rCurrentBlockIndex' is increased by
 *   'arrayLength * data.GetNblock()' of the given 'Serializable'.
 *
 *
 * __Note:__ Dynamic-sized objects need to inherit from 'BufferSerializable',
 * which uses buffers and provides additional 'register' methods for:
 *
 * Method                                  | Suitable for
 * ----------------------------------------| --------------------------
 * 'RegisterSerializable()'                | Dynamic-sized 'Serializable'
 *                                         | object (for constant-sized
 *                                         | 'Serializable', use
 *                                         | 'RegisterConstSizeSerializable()'
 * 'RegisterVarVector()'                   | 'std::vector<DataType>' (for
 *                                         | primitive 'DataType', e.g. 'int',
 *                                         | 'double', ...)
 * 'RegisterConstSizeSerializableVector()' | 'std::vector<DataType>' (for
 *                                         | constant size 'Serializable')
 * 'RegisterSerializableVector()'          | 'std::vector<DataType>' (for
 *                                         | dynamic size 'Serializable')
 * 'RegisterMap()'                         | 'std::map<DataTypeKey,
 *                                         | DataTypeValue>' (for primitive
 *                                         | types)
 */
class Serializable
{
 public:
  /** Empty destructor */
  virtual ~Serializable();

  /**
   * Returns the address of the i-th block and its size.
   *
   * \param blockIndex Index of the block to be returned
   * \param blockSize Reference to the size of the returned block
   * \param isLoad flag to decide if we are loading or saving
   *
   * \return Pointer to the current block
   *
   * Each 'GetBlock()' method should look like this:
   * \code
   *   std::size_t current_block = 0;
   *   bool* p_data = nullptr;
   *   // ... register methods...
   *   return p_data;
   */
  virtual bool* GetBlock(
      const gsl::index blockIndex
    , std::size_t& rBlockSize
    , const bool isLoad = false) = 0;

  /** \return the binary size of the data to be saved */
  virtual std::size_t GetSerializableSize() const = 0;

 protected:
  /**
   * Register _primitive data types_ ('int', 'double', ...) or arrays of
   * those. The address of the data is returned in combination with the size
   * 'arrayLength * sizeof(DataType)'.
   *
   * \param blockIndex 'blockIndex' from 'GetBlock()' - to determine if this
   *        is the current block.
   * \param rBlockSize 'rBlockSize' from 'GetBlock()' - will be filled if
   *        this is the current block.
   * \param rCurrentBlockIndex _local_ variable of 'GetBlock()' - will always be
   *        counted up by the number of blocks this method registers (_always
   *        1 in this case_).
   * \param rpData Reference to 'p_data' from 'GetBlock()' - will be filled
   *        with pointer to the data at 'blockIndex' if this is the current block.
   * \param rData Reference to the data to be registered by this method.
   *        Fills 'rpData' with a '(bool*)'-casted pointer to 'data' if this
   *        is the current block.
   * \param length Number of elements of 'DataType' in 'data', if 'data' is
   *        an array. _Defaults to 1 for single values._
   */
  template<typename TYPE>
  void RegisterVar(
      const gsl::index blockIndex
    , std::size_t& rBlockSize
    , gsl::index& rCurrentBlockIndex
    , bool*& rpData
    , const TYPE& rData
    , const std::size_t length = 1) const
  {
    if (blockIndex == rCurrentBlockIndex) {
      rBlockSize = sizeof(TYPE) * length;
      // Since we are casting a const of a different type
      rpData = reinterpret_cast<bool*>(const_cast<TYPE*>(&rData));
    }
    ++rCurrentBlockIndex;
  }
};
}  // namespace iblbm

#endif  // SRC_CORE_SERIALIZABLE_HPP_
