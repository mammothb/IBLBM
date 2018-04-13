#include "BufferSerializable.hpp"

namespace iblbm
{
std::size_t BufferSerializable::AddSizeToBuffer(
    const gsl::index blockIndex
  , std::size_t& rBlockSize
  , gsl::index& rCurrentBlockIndex
  , gsl::index& rSizeBufferIndex
  , bool*& rpData
  , const std::size_t data) const
{
  std::size_t return_size {0};

  // write size into mSizeBuffer vector
  if (blockIndex == rCurrentBlockIndex)
      mSizeBuffer.push_back(*new std::size_t{data});

  if (blockIndex >= rCurrentBlockIndex)
      return_size = mSizeBuffer[rSizeBufferIndex];

  // register size as var
  RegisterVar(blockIndex, rBlockSize, rCurrentBlockIndex, rpData,
      mSizeBuffer[rSizeBufferIndex]);
  ++rSizeBufferIndex;

  return return_size;
}
}  // namespace iblbm
