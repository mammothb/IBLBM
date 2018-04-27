#include "BlockData2D.hpp"

#include "gsl/gsl"
#include "Cuboid2D.hpp"

namespace iblbm
{
template<typename T, typename BaseType>
BlockData2D<T, BaseType>::BlockData2D()
  : BlockStructure2D{0, 0},
    mDimension{0},
    mpRawData{nullptr},
    mpField{nullptr}
{
  Construct();
}

template<typename T, typename BaseType>
BlockData2D<T, BaseType>::BlockData2D(
    const Cuboid2D<T>& rCuboid
  , std::size_t dimension/*=1*/)
  : BlockStructure2D{rCuboid.GetNx(), rCuboid.GetNy()},
    mDimension{dimension},
    mpRawData{nullptr},
    mpField{nullptr}
{
  Construct();
}

template<typename T, typename BaseType>
BlockData2D<T, BaseType>::BlockData2D(
    std::size_t nx
  , std::size_t ny
  , std::size_t dimension/*=1*/)
  : BlockStructure2D{nx, ny},
    mDimension{dimension},
    mpRawData{nullptr},
    mpField{nullptr}
{
  Construct();
}

template<typename T, typename BaseType>
BlockData2D<T, BaseType>::~BlockData2D()
{
  Deconstruct();
}

template<typename T, typename BaseType>
BlockData2D<T, BaseType>::BlockData2D(const BlockData2D<T, BaseType>& rRhs)
  : BlockStructure2D{rRhs.mNx, rRhs.mNy},
    mDimension(rRhs.mDimension),
    mpRawData{nullptr},
    mpField{nullptr}
{
  if (rRhs.IsConstructed()) {
    Construct();
    std::copy(rRhs.mpRawData, rRhs.mpRawData + GetDataSize(), mpRawData);
  }
}

template<typename T, typename BaseType>
BlockData2D<T, BaseType>& BlockData2D<T, BaseType>::operator=(
    const BlockData2D<T, BaseType>& rRhs)
{
  BlockData2D<T, BaseType> tmp(rRhs);
  Swap(tmp);
  return *this;
}

// benefits of move operator: does not allocate memory
template<typename T, typename BaseType>
BlockData2D<T, BaseType>::BlockData2D(BlockData2D<T, BaseType>&& rRhs)
  : BlockStructure2D(rRhs.mNx, rRhs.mNy),
    mDimension{0},
    mpRawData{nullptr},
    mpField{nullptr}
{
  *this = std::move(rRhs);
}

template<typename T, typename BaseType>
BlockData2D<T, BaseType>& BlockData2D<T, BaseType>::operator=(
    BlockData2D<T, BaseType>&& rRhs)
{
  if (this != &rRhs) {
    // Free the existing resource.
    this->ReleaseMemory();

    // Copy the data pointer and its length from the source object.
    mDimension = rRhs.mDimension;
    mpRawData = rRhs.mpRawData;
    mpField = rRhs.mpField;
    this->mNx = rRhs.mNx;
    this->mNy = rRhs.mNy;

    // Release the data pointer from the source object so that the
    // destructor does not free the memory multiple times
    rRhs.mpRawData = nullptr;
    rRhs.mpField = nullptr;
    rRhs.mNx = 0;
    rRhs.mNy = 0;
  }

  return *this;
}

template<typename T, typename BaseType>
void BlockData2D<T, BaseType>::Swap(BlockData2D<T, BaseType>& rRhs)
{
  // Block2D
  std::swap(this->mNx, rRhs.mNx);
  std::swap(this->mNy, rRhs.mNy);
  // BlockData2D
  std::swap(mDimension, rRhs.mDimension);
  std::swap(mpRawData, rRhs.mpRawData);
  std::swap(mpField, rRhs.mpField);
}

template<typename T, typename BaseType>
bool BlockData2D<T, BaseType>::IsConstructed() const
{
  return mpRawData;
}

template<typename T, typename BaseType>
void BlockData2D<T, BaseType>::Construct()
{
  if (!IsConstructed()) AllocateMemory();
}

template<typename T, typename BaseType>
void BlockData2D<T, BaseType>::Deconstruct()
{
  if (IsConstructed()) ReleaseMemory();
}

template<typename T, typename BaseType>
void BlockData2D<T, BaseType>::Reset()
{
  Expects(IsConstructed());
  for (gsl::index i {0}; i < GetDataSize(); ++i) (*this)[i] = BaseType{};
}

template<typename T, typename BaseType>
BaseType& BlockData2D<T, BaseType>::rGetData(
    gsl::index xIndex
  , gsl::index yIndex
  , gsl::index d/*=0*/)
{
  Expects(xIndex >= 0 && xIndex < this->mNx);
  Expects(yIndex >= 0 && yIndex < this->mNy);
  Expects(d >= 0 && d < this->mDimension);
  Expects(IsConstructed());
  return mpField[xIndex][yIndex][d];
}

template<typename T, typename BaseType>
const BaseType& BlockData2D<T, BaseType>::rGetData(
    gsl::index xIndex
  , gsl::index yIndex
  , gsl::index d/*=0*/) const
{
  Expects(xIndex >= 0 && xIndex < this->mNx);
  Expects(yIndex >= 0 && yIndex < this->mNy);
  Expects(d >= 0 && d < this->mDimension);
  Expects(IsConstructed());
  return mpField[xIndex][yIndex][d];
}

template<typename T, typename BaseType>
BaseType& BlockData2D<T, BaseType>::operator[](gsl::index i)
{
  Expects(i >= 0 && i < this->mNx * this->mNy * this->mDimension);
  Expects(IsConstructed());
  return mpRawData[i];
}

template<typename T, typename BaseType>
const BaseType& BlockData2D<T, BaseType>::operator[](gsl::index i) const
{
  Expects(i >= 0 && i < this->mNx * this->mNy * this->mDimension);
  Expects(IsConstructed());
  return mpRawData[i];
}

template<typename T, typename BaseType>
bool* BlockData2D<T, BaseType>::operator()(
    gsl::index xIndex
  , gsl::index yIndex
  , gsl::index d)
{
  Expects(xIndex >= 0 && xIndex < this->mNx);
  Expects(yIndex >= 0 && yIndex < this->mNy);
  Expects(d >= 0 && d < this->mDimension);
  Expects(IsConstructed());
  return (bool*)&mpField[xIndex][yIndex][d];
}

template<typename T, typename BaseType>
BaseType BlockData2D<T, BaseType>::GetMax()
{
  return *std::max_element(mpRawData, mpRawData + GetDataSize());
}

template<typename T, typename BaseType>
BaseType BlockData2D<T, BaseType>::GetMin()
{
  return *std::min_element(mpRawData, mpRawData + GetDataSize());
}

template<typename T, typename BaseType>
BaseType* BlockData2D<T, BaseType>::pGetRawData() const
{
  return mpRawData;
}

template<typename T, typename BaseType>
std::size_t BlockData2D<T, BaseType>::GetDataSize() const
{
  return this->mNx * this->mNy * mDimension;
}

template<typename T, typename BaseType>
std::size_t BlockData2D<T, BaseType>::GetDimension() const
{
  return mDimension;
}

template<typename T, typename BaseType>
std::size_t BlockData2D<T, BaseType>::GetNumBlock() const
{
  return 4;
}

template<typename T, typename BaseType>
std::size_t BlockData2D<T, BaseType>::GetSerializableSize() const
{
  // mDimension, mNx, mNy, and mpRawData
  return 3 * sizeof(std::size_t) + GetDataSize() * sizeof(BaseType);
}

template<typename T, typename BaseType>
bool* BlockData2D<T, BaseType>::pGetBlock(
    gsl::index blockIndex
  , std::size_t& rBlockSize
  , const bool /*isLoad=false*/)
{
  gsl::index current_block_index {0};
  bool* p_data {nullptr};
  RegisterVar(blockIndex, rBlockSize, current_block_index, p_data,
      mDimension);
  RegisterVar(blockIndex, rBlockSize, current_block_index, p_data,
      this->mNx);
  RegisterVar(blockIndex, rBlockSize, current_block_index, p_data,
      this->mNy);
  RegisterVar(blockIndex, rBlockSize, current_block_index, p_data,
      *mpRawData, GetDataSize());

  return p_data;
}

template<typename T, typename BaseType>
void BlockData2D<T, BaseType>::AllocateMemory()
{
  mpRawData = new BaseType[GetDataSize()];
  mpField = new BaseType**[this->mNx];
  for (gsl::index x {0}; x < this->mNx; ++x) {
    mpField[x] = new BaseType*[this->mNy];
    for (gsl::index y {0}; y < this->mNy; ++y) {
      // connect matrix element to the corresponding array entry of mpRawData
      mpField[x][y] = mpRawData + mDimension * (static_cast<std::size_t>(x) *
          this->mNy + static_cast<std::size_t>(y));
      // initialize data with zero
      for (gsl::index d {0}; d < mDimension; ++d)
          mpField[x][y][d] = BaseType{};
    }
  }
}

template<typename T, typename BaseType>
void BlockData2D<T,BaseType>::ReleaseMemory()
{
  delete[] mpRawData;
  mpRawData = nullptr;
  for (gsl::index x {0}; x < this->mNx; ++x) delete[] mpField[x];
  delete[] mpField;
}

// explicit instantiation
template class BlockData2D<double, int>;
template class BlockData2D<double, double>;
}  // namespace iblbm
