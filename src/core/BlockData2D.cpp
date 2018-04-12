#include "BlockData2D.hpp"

namespace iblbm
{
template<typename T, typename BaseType>
BlockData2D<T, BaseType>::BlockData2D()
  : BlockStructure2D{0, 0},
    mSize{0},
    mpRawData{nullptr},
    mpField{nullptr}
{}

template<typename T, typename BaseType>
std::size_t BlockData2D<T, BaseType>::GetDataSize() const
{
  return this->mNx * this->mNy * mSize;
}

template<typename T, typename BaseType>
std::size_t BlockData2D<T, BaseType>::GetNumBlock() const
{
  return 4;
}

template<typename T, typename BaseType>
std::size_t BlockData2D<T, BaseType>::GetSerializableSize() const
{
  // mSize, mNx, mNy, and mpRawData
  return 3 * sizeof(std::size_t) + GetDataSize() * sizeof(BaseType);
}

template<typename T, typename BaseType>
bool* BlockData2D<T, BaseType>::pGetBlock(
    gsl::index blockIndex
  , std::size_t& rBlockSize
  , const bool /*isLoad*/)
{
  gsl::index current_block_index {0};
  bool* p_data {0};
  RegisterVar(blockIndex, rBlockSize, current_block_index, p_data, mSize);
  RegisterVar(blockIndex, rBlockSize, current_block_index, p_data,
      this->mNx);
  RegisterVar(blockIndex, rBlockSize, current_block_index, p_data,
      this->mNy);
  RegisterVar(blockIndex, rBlockSize, current_block_index, p_data,
      *mpRawData, GetDataSize());

  return p_data;
}

// explicit instantiation
template class BlockData2D<double, std::size_t>;
template class BlockData2D<double, double>;
}  // namespace iblbm
