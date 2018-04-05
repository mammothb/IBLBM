#ifndef SRC_CORE_SERIALIZER_HPP_
#define SRC_CORE_SERIALIZER_HPP_

#include <cstddef>

#include <string>

namespace iblbm
{
// Forward declaration
class Serializable;

class Serializer
{
 public:
  /**
   * Constructor.
   * \param rSerializable object to be (de-)serialized
   * \param rFilename filename for I/O
   */
  Serializer(
      Serializable& rSerializable
    , const std::string& rFilename = "Serializable");

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
  std::size_t mBlockNo;
  /** Total memory size in bit (computed by ComputeSize()) */
  std::size_t mSize;
  /** Default filename for I/O */
  std::string mFilename;
};
}  // namespace iblbm

#endif  // SRC_CORE_SERIALIZER_HPP_
