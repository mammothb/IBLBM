#ifndef CORE_SRC_BUFFERSERIALIZABLE_HPP_
#define CORE_SRC_BUFFERSERIALIZABLE_HPP_

#include <map>
#include <utility>
#include <vector>

#include "Serializable.hpp"

namespace iblbm
{
class BufferSerializable : public Serializable
{
 protected:
  /**
   * Register 'Serializable' object of _dynamic size_.
   * This method is suitable for all 'Serializable' objects of
   * _dynamic size_, e.g. where 'GetNumBlock()' cannot be evaluated correctly
   * on an empty object.
   *
   * This method registers a 'Serializable' by buffering the dynamic
   * 'GetNumBlock()' value and afterwards delegating the 'pGetBlock()' call
   * to the 'Serializable'.
   *
   * _Note:_ 'pGetNumBlock()' only works for save mode, since it is dynamic.
   */
  template<typename DataType>
  void RegisterSerializable(
      const gsl::index blockIndex
    , std::size_t& rBlockSize
    , gsl::index& rCurrentBlockIndex
    , gsl::index& rSizeBufferIndex
    , bool*& rpData
    , DataType& rData
    , const bool isLoad = false)
  {
    static_assert(std::is_base_of<Serializable, DataType>::value,
        "DataType must be a Serializable.");

    std::size_t num_block {0};

    // hold GetNumblock() in sizeBuffer
    if (isLoad) {
      // loading -> set to 0 and wait for reading next round
      num_block = AddSizeToBuffer(blockIndex, rBlockSize, rCurrentBlockIndex,
          rSizeBufferIndex, rpData, 0);
    }
    else {
      // saving -> save getNblock from data object
      num_block = AddSizeToBuffer(blockIndex, rBlockSize, rCurrentBlockIndex,
          rSizeBufferIndex, rpData, rData.GetNumBlock());
    }

    if (blockIndex >= rCurrentBlockIndex) {
      if (blockIndex < rCurrentBlockIndex + num_block) {
        rpData = rData.pGetBlock(blockIndex - rCurrentBlockIndex, rBlockSize,
            isLoad);
      }
    }
  }

  /**
   * Method for registering a 'std::vector<DataType>' of constant-sized
   * 'Serializable'
   * This method registers a vector of a constant-sized 'Serializable'. The
   * first block holds a 'size_t vector_size' with the size of the vector,
   * the second holding the number of blocks in one 'DataType', followed by
   * 'vector_size' blocks of 'DataType', each of which consists of
   * 'DataType.getNblock()' subblocks.
   *
   * The 'sizeBuffer' is used to store the _constant(!)_ number of subblocks
   * ('DataType.getNblock()') of the 'Serializable'.
   *
   * The total number of blocks occupied by this method is
   * '2 + vector_size * nSubBlock'.
   *
   * \param blockIndex 'blockIndex' from 'GetBlock()' - to determine if this
   *        is the current block.
   * \param rBlockSize 'rBlockSize' from 'GetBlock()' - will be filled if
   *        this is the current block.
   * \param rCurrentBlockIndex _local_ variable of 'GetBlock()' - will always be
   *        counted up by the number of blocks this method registers (_always
   *        1 in this case_).
   * \param rSizeBufferIndex Index counter for size buffer. Is increased by
   *        __two__ by this method, and the size of the registered
   *        'std::vector' is stored in the corresponding _first_ 'sizeBuffer'
   *        element, the fixed number of blocks of 'DataType' is stored in
   *        the _second_ one.
   * \param rpData Reference to 'p_data' from 'GetBlock()' - will be filled
   *        with pointer to the data at 'blockIndex' if this is the current
   *        block.
   * \param rData Reference to the data to be registered by this method.
   *        Fills 'rpData' with a (bool*)'-casted pointer (_if this is the
   *        current block_) to:
   *        - _First block_ - number of elements in the vector
   *        - _second block_ - fixed number of block in 'DataType'
   *        - _third block_ to _last block_ - pointer to 'i-th' vector element
   * \param isLoad flag to decide if we are loading or saving
   */
  template<typename DataType>
  void RegisterConstSizeSerializableVector(
      const gsl::index blockIndex
    , std::size_t& rBlockSize
    , gsl::index& rCurrentBlockIndex
    , gsl::index& rSizeBufferIndex
    , bool*& rpData
    , DataType& rData
    , const bool isLoad = false)
  {
    static_assert(std::is_base_of<Serializable, DataType>::value,
        "DataType must be a Serializable.");

    if (blockIndex >= rCurrentBlockIndex) {
      // process length of data vector
      auto vector_size {AddSizeToBuffer(blockIndex, rBlockSize,
          rCurrentBlockIndex, rSizeBufferIndex, rpData, rData.size())};

      // resize data vector from buffer (only for loading)
      if (blockIndex == rCurrentBlockIndex && isLoad)
          rData.resize(mSizeBuffer[rSizeBufferIndex - 1]);

      // process Serializables
      if (blockIndex >= rCurrentBlockIndex && vector_size > 0) {
        for (DataType& r_it_data : rData) {
          RegisterConstSizeSerializable(blockIndex, rBlockSize,
              rCurrentBlockIndex, rpData, r_it_data, isLoad);
        }
      }
    }
  }

  /**
   * Method for registering a 'std::vector<DataType>' of dynamic-sized
   * 'DataType'
   * This method registers a vector of a dynamic-sized 'Serializable'. The
   * first block holds a 'std::size_t vector_size' with the size of the
   * vector, followed by 'vector_size' many 'registerSerializable()' calls,
   * which occupy 'data[i].getNblock()' blocks each. 'getNblock()' may differ
   * for any 'data[i]'.
   *
   * The 'sizeBuffer' is used to store the length of the registered
   * 'std::vector' as well as the number of blocks occupied by each
   * 'Serializable' in the vector. This method occupies '1 + vector_size'
   * many 'sizeBuffer' elements.
   *
   * The total number of blocks occupied by this method is
   * '1 + vector_size * (1 + data[i].getNblock())'.
   *
   * \param rSizeBufferIndex Index counter for size buffer. Is increased by
   *        __two__ by this method, and the size of the registered
   *        'std::vector' is stored in the corresponding _first_ 'sizeBuffer'
   *        element, the fixed number of blocks of 'DataType' is stored in
   *        the _second_ one.
   * \param data Reference to the data to be registered by this method. Fills
   *        'rpData' with a '(bool*)'-casted pointer (_if this is the
   *        current block_) to:
   *        - _First block_ - number of elements in the vector
   *        - For each 'std::vector' element:
   *          - _First sub-block_ - number of blocks occupied by this
   *            'Serializable' object
   *          - _second block_ to _last block_ - pointer to 'i-th' block of
   *            'data[i]'
   */
  template<typename DataType>
  void RegisterSerializableVector(
      const gsl::index blockIndex
    , std::size_t& rBlockSize
    , gsl::index& rCurrentBlockIndex
    , gsl::index& rSizeBufferIndex
    , bool*& rpData
    , DataType& rData
    , const bool isLoad = false)
  {
    static_assert(std::is_base_of<Serializable, DataType>::value,
        "DataType must be a Serializable.");

    if (blockIndex >= rCurrentBlockIndex) {
      // process length of data vector
      auto vector_size {AddSizeToBuffer(blockIndex, rBlockSize,
          rCurrentBlockIndex, rSizeBufferIndex, rpData, rData.size())};

      // resize data vector from buffer (only for loading)
      if (blockIndex == rCurrentBlockIndex && isLoad)
          rData.resize(mSizeBuffer[rSizeBufferIndex - 1]);

      // process Serializables
      if (blockIndex >= rCurrentBlockIndex && vector_size > 0) {
        for (DataType& r_it_data : rData) {
          RegisterSerializable(blockIndex, rBlockSize, rCurrentBlockIndex,
              rSizeBufferIndex, rpData, r_it_data, isLoad);
        }
      }
    }
  }

  /**
   * Method for registering a 'std::vector<DataType>' of primitive 'DataType'
   * ('int', 'double', ...). The first block holds a 'std::size_t vector_size'
   * with the size of the vector, followed by 'vector_size' many blocks of
   * 'DataType'.
   *
   * The total number of blocks occupied by this method is '1 + vector_size'.
   *
   * \param blockIndex 'blockIndex' from 'GetBlock()' - to determine if this
   *        is the current block.
   * \param rBlockSize 'rBlockSize' from 'GetBlock()' - will be filled if
   *        this is the current block.
   * \param rCurrentBlockIndex _local_ variable of 'GetBlock()' - will always be
   *        counted up by the number of blocks this method registers (_always
   *        1 in this case_).
   * \param rSizeBufferIndex Index counter for size buffer. Is increased by
   *        one by this method, and the size of the registered 'std::vector'
   *        is stored in the corresponding 'mSizeBuffer' element.
   * \param rpData Reference to 'p_data' from 'GetBlock()' - will be filled
   *        with pointer to the data at 'blockIndex' if this is the current
   *        block.
   * \param rData Reference to the data to be registered by this method.
   *        Fills 'rpData' with a '(bool*)'-casted pointer (_if this is the
   *        current block_) to:
   *        - _First block_ - number of elements in the vector
   *        - _second block_ to _last block_ - pointer to 'i-th' vector
   *          element
   * \param isLoad flag to decide if we are loading or saving
   */
  template<typename DataType>
  void RegisterVarVector(
      const gsl::index blockIndex
    , std::size_t& rBlockSize
    , gsl::index& rCurrentBlockIndex
    , gsl::index& rSizeBufferIndex
    , bool*& rpData
    , std::vector<DataType>& rData
    , const bool isLoad = false)
  {
    if (blockIndex >= rCurrentBlockIndex) {
      // process length of data vector, added rData.size() to current block
      auto vector_size {AddSizeToBuffer(blockIndex, rBlockSize,
          rCurrentBlockIndex, rSizeBufferIndex, rpData, rData.size())};

      // resize data vector from buffer (only for loading)
      if (blockIndex == rCurrentBlockIndex && isLoad)
          rData.resize(mSizeBuffer[rSizeBufferIndex - 1]);

      // Check that we are at least at the current block and that we have
      // data to write, if the vector is size 0, we write nothing
      if (blockIndex >= rCurrentBlockIndex &&
          blockIndex < rCurrentBlockIndex + vector_size) {
        rBlockSize = sizeof(DataType);
        // Since we are casting a const of a different type
        rpData = reinterpret_cast<bool*>(const_cast<DataType*>(
            &rData[blockIndex - rCurrentBlockIndex]));
      }
      rCurrentBlockIndex += vector_size;
    }
  }

  /**
   * Method for registering a 'std::map<KeyType, ValType>' of
   * fixed-sized types (i.e. 'int', 'double'). The first block holds a
   * 'std::size_t map_size' with the size of the map, followed by
   * 'map_size' blocks of 'std::pairs'.
   *
   * In case of _loading_ the data, the 'dataBuffer' has to be used in order
   * to provide a valid pointer to a newly created 'std::pair' before
   * inserting that same pair into the map in the next iteration.
   *
   * The total number of blocks occupied by this method is '1 + map_size'.
   *
   * \param blockIndex 'blockIndex' from 'GetBlock()' - to determine if this
   *        is the current block.
   * \param rBlockSize 'rBlockSize' from 'GetBlock()' - will be filled if
   *        this is the current block.
   * \param rCurrentBlockIndex _local_ variable of 'GetBlock()' - will always be
   *        counted up by the number of blocks this method registers (_always
   *        1 in this case_).
   * \param rSizeBufferIndex Index counter for size buffer. Is increased by
   *        one by this method, and the size of the registered 'std::vector'
   *        is stored in the corresponding 'sizeBuffer' element.
   * \param rpData Reference to 'p_data' from 'GetBlock()' - will be filled
   *        with pointer to the data at 'blockIndex' if this is the current
   *        block.
   * \param rData Reference to the data to be registered by this method.
   *        Fills 'rpData' with a '(bool*)'-casted pointer (_if this is the
   *        current block_) to:
   *        - _First block_ - number of elements in the map
   *        - _second block_ to _last block_ - pointer to 'i-th' map element
   * \param isLoad flag to decide if we are loading or saving
   *
   * __Note:__ In _writing mode_, 'rpData' holds a pointer to the 'i-th' map
   * element (which is a 'std::pair'). In _reading mode_, 'rpData' holds a
   * pointer to the bool* buffer, which holds a newly created 'std::pair' to
   * be filled and that pair is inserted into 'rData' in the following round.
   */
  template<typename KeyType, typename ValType>
  void RegisterMap(
      const gsl::index blockIndex
    , std::size_t& rBlockSize
    , gsl::index& rCurrentBlockIndex
    , gsl::index& rSizeBufferIndex
    , bool*& rpData
    , std::map<KeyType, ValType>& rData
    , const bool isLoad = false)
  {
    if (blockIndex >= rCurrentBlockIndex) {
      // process length of data map
      auto map_size {AddSizeToBuffer(blockIndex, rBlockSize,
          rCurrentBlockIndex, rSizeBufferIndex, rpData, rData.size())};

      // Check that we are at least at the current block and that we have
      // data to write, if the vector is size 0, we write nothing
      if (blockIndex >= rCurrentBlockIndex &&
          blockIndex < rCurrentBlockIndex + map_size + 1) {
        // determine size of pair
        rBlockSize = sizeof(std::pair<KeyType, ValType>);

        if (isLoad) {  // LOADING MODE
          // If pair in dataBuffer => insert into data and delete, if we just
          // reached the block to start reading, this will be false so that
          // we can push_back a new std::pair first. Subsequently, we will
          // insert the last entry (externally filled) into rData
          if (blockIndex > rCurrentBlockIndex) {
            auto p_pair {reinterpret_cast<std::pair<KeyType, ValType>*>(
                mDataBuffer.back())};
            // copy pair into map
            rData.insert(*p_pair);
            // delete pair object that was created (with new!) in the buffer
            delete p_pair;
            // remove pointer to deleted pair from buffer vector
            mDataBuffer.pop_back();
          }

          // push new pair into buffer and return pointer
          if (blockIndex < rCurrentBlockIndex + map_size) {
            mDataBuffer.push_back(reinterpret_cast<bool*>(
                new std::pair<KeyType, ValType>));
            rpData = mDataBuffer.back();
          }
        }
        else {  // SAVING MODE
          if (blockIndex < rCurrentBlockIndex + map_size) {
            // advance through iterator to n-th element and return pointer to
            // pair
            auto it_map {rData.begin()};
            std::advance(it_map, blockIndex - rCurrentBlockIndex);
            rpData = reinterpret_cast<bool*>(&(*it_map));
          }
        }
      }
      rCurrentBlockIndex += map_size;
    }
  }

  /**
   * Add a 'std::size_t' to the 'mSizeBuffer' in the 'n-th' round and return
   * that 'size_t' in all successive rounds
   * - increase 'rCurrentBlockIndex' by one
   * - increase 'rSizeBufferIndex' by one.
   * - 'n-th' round: push given std::size_t to mSizeBuffer and provide
   *   pointer to it.
   *
   * \param blockIndex 'blockIndex' from 'GetBlock()' - to determine if this
   *        is the current block.
   * \param rBlockSize 'rBlockSize' from 'GetBlock()' - will be filled if
   *        this is the current block.
   * \param rCurrentBlockIndex _local_ variable of 'GetBlock()' - will always
   *        be counted up by the number of blocks this method registers
   *        (_always 1 in this case_).
   * \param rSizeBufferIndex Index counter for size buffer. Is increased by
   *        one by this method, and the size of the registered 'std::vector'
   *        is stored in the corresponding 'mSizeBuffer' element.
   * \param rpData Reference to 'p_data' from 'GetBlock()' - will be filled
   *        with pointer to the data at 'blockIndex' if this is the current
   *        block.
   * \param rData Data to be registered by this method.
   *        Fills 'rpData' with a '(bool*)'-casted pointer to 'data' if this
   *        is the current block.
   * \param data size
   */
  std::size_t AddSizeToBuffer(
      const gsl::index blockIndex
    , std::size_t& rBlockSize
    , gsl::index& rCurrentBlockIndex
    , gsl::index& rSizeBufferIndex
    , bool*& rpData
    , const std::size_t data) const;

  /**
   * Data buffer for data that has to be buffered between two 'GetBlock()'
   * iterations. This is currently only used within 'registerMap()'.
   *
   * _This is only used for loading._
   */
  mutable std::vector<bool*> mDataBuffer;

  /**
   * 'std::vector' of integer buffers (e.g. for 'std::vector' size) to be
   * buffered for the whole iteration process. Each register method for
   * dynamic-sized objects (e.g. 'std::vector' or 'std::map') uses the size
   * buffer to provide to correctly increase the 'rCurrentBlockIndex'
   * variable for all following register methods.
   */
  mutable std::vector<std::size_t> mSizeBuffer;
};
}  // namespace iblbm

#endif  // CORE_SRC_BUFFERSERIALIZABLE_HPP_
