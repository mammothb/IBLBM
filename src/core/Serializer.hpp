#ifndef SRC_CORE_SERIALIZER_HPP_
#define SRC_CORE_SERIALIZER_HPP_

#include <cstddef>
#include <string>

#include "gsl/gsl"

namespace iblbm
{
// Forward declaration
class Serializable;

/**
 * Class for writing, reading, sending and receiving `Serializable` objects.
 * Used for checkpointing simulations
 */
class Serializer
{
  friend class TestSerializer;

 public:
  /**
   * Constructor.
   * \param rSerializable object to be (de-)serialized
   * \param rFilename filename for I/O
   */
  Serializer(
      Serializable& rSerializable
    , const std::string& rFilename = "Serializable");

  /** Resets mBlockIndex */
  void ResetCounter();

  /** \return total memory size in bits */
  std::size_t GetSize() const;

  /**
   * Return pointer to the memory of the current block and increments
   * mBlockIndex
   *
   * \param blockSize Reference to the size of the returned block
   * \param isLoad flag to decide if we are loading or saving
   */
  bool* pGetNextBlock(
      std::size_t& rBlockSize
    , const bool isLoad);

  /**
   * Load mrSerializable from file mFilename. Always in parallel, i.e., one
   * file per rank.
   *
   * \param rFilename filename of file to be loaded
   * \param forceUnsigned flag to determine if we are forcing it to write
   *        to unsigned instead of std::size_t
   */
  bool Load(
      std::string filename = "Serializable"
    , const bool forceUnsigned = false);

  /**
   * Save mrSerializable into file mFilename. Always in parallel, i.e., one
   * file per rank.
   *
   * \param rFilename filename of file to be saved
   * \param forceUnsigned flag to determine if we are forcing it to write
   *        to unsigned instead of std::size_t
   */
  bool Save(
      std::string filename = "Serializable"
    , const bool forceUnsigned = false);

  /**
   * Compute mSize based on the individual definition of GetBlock()
   */
  void ComputeSize(const bool forceRecompute = false);

 private:
  /**
   * Set filename to mFilename, which defaults to "Serializable" if it's
   * empty
   */
  void ValidateFilename(std::string& rFilename);
  /** \return full file name for rFilename */
  const std::string GetFullFilename(const std::string& rFilename);

  /** Object to be (de-)serialized */
  Serializable& mrSerializable;
  /** Counter for the current block number being processed */
  gsl::index mBlockIndex;
  /** Total memory size in bit (computed by ComputeSize()) */
  std::size_t mSize;
  /** Default filename for I/O */
  std::string mFilename;
};
}  // namespace iblbm

#endif  // SRC_CORE_SERIALIZER_HPP_
