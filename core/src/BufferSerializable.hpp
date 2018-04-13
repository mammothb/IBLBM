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
   * Method for registering a 'std::vector<TYPE>' of primitive 'TYPE'
   * ('int', 'double', ...). The first block holds a 'std::size_t vector_size'
   * with the size of the vector, followed by 'vector_size' many blocks of
   * 'TYPE'.
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
  template<typename TYPE>
  void RegisterVarVector(
      const gsl::index blockIndex
    , std::size_t& rBlockSize
    , gsl::index& rCurrentBlockIndex
    , gsl::index& rSizeBufferIndex
    , bool*& rpData
    , std::vector<TYPE>& rData
    , const bool isLoad = false)
  {
    if (blockIndex >= rCurrentBlockIndex) {
      // process length of data vector, added rData.size() to current block
      std::size_t vector_size {AddSizeToBuffer(blockIndex, rBlockSize,
          rCurrentBlockIndex, rSizeBufferIndex, rpData, rData.size())};

      // resize data vector from buffer (only for loading)
      if (blockIndex == rCurrentBlockIndex && isLoad)
          rData.resize(mSizeBuffer[rSizeBufferIndex - 1]);

      // Check that we are at least at the current block and that we have
      // data to write, if the vector is size 0, we write nothing
      if (blockIndex >= rCurrentBlockIndex &&
          blockIndex < rCurrentBlockIndex + vector_size) {
        rBlockSize = sizeof(TYPE);
        // Since we are casting a const of a different type
        rpData = reinterpret_cast<bool*>(const_cast<TYPE*>(
            &rData[blockIndex - rCurrentBlockIndex]));
      }
      rCurrentBlockIndex += vector_size;
    }
  }

  /**
   * Method for registering a 'std::map<KEY_TYPE, VAL_TYPE>' of
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
  template<typename KEY_TYPE, typename VAL_TYPE>
  void RegisterMap(
      const gsl::index blockIndex
    , std::size_t& rBlockSize
    , gsl::index& rCurrentBlockIndex
    , gsl::index& rSizeBufferIndex
    , bool*& rpData
    , std::map<KEY_TYPE, VAL_TYPE>& rData
    , const bool isLoad = false)
  {
    if (blockIndex >= rCurrentBlockIndex) {
      // process length of data map
      std::size_t map_size {AddSizeToBuffer(blockIndex, rBlockSize,
          rCurrentBlockIndex, rSizeBufferIndex, rpData, rData.size())};

      // Check that we are at least at the current block and that we have
      // data to write, if the vector is size 0, we write nothing
      if (blockIndex >= rCurrentBlockIndex &&
          blockIndex < rCurrentBlockIndex + map_size + 1) {
        // determine size of pair
        rBlockSize = sizeof(std::pair<KEY_TYPE, VAL_TYPE>);

        if (isLoad) {  // LOADING MODE
          // If pair in dataBuffer => insert into data and delete, if we just
          // reached the block to start reading, this will be false so that
          // we can push_back a new std::pair first. Subsequently, we will
          // insert the last entry (externally filled) into rData
          if (blockIndex > rCurrentBlockIndex) {
            auto p_pair {reinterpret_cast<std::pair<KEY_TYPE, VAL_TYPE>*>(
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
                new std::pair<KEY_TYPE, VAL_TYPE>));
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
