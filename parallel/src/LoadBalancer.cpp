#include "LoadBalancer.hpp"

#include <iostream>
#include <utility>

#include "Exception.hpp"
#include "MpiManager.hpp"
#include "OstreamManager.hpp"

namespace iblbm
{
template<typename T>
LoadBalancer<T>::LoadBalancer(std::size_t size/*=1*/)
  : mSize{size}
{}

template<typename T>
LoadBalancer<T>::LoadBalancer(
    std::size_t size
  , std::map<gsl::index, gsl::index>& rLocalIndex
  , std::vector<gsl::index>& rGlobalIndex
  , std::map<gsl::index, std::size_t>& rRank)
  : mSize{size},
    mLocalIndex{rLocalIndex},
    mGlobalIndex{rGlobalIndex},
    mRank{rRank}
{}

template<typename T>
LoadBalancer<T>::~LoadBalancer()
{}

template<typename T>
void LoadBalancer<T>::Initialize(
    CuboidGeometry2D<T>& /*rCuboidGeometry2D*/
  , const double /*ratioFullEmpty = 1*/
  , const double /*weightEmpty = 0.0*/)
{}

template<typename T>
void LoadBalancer<T>::Swap(LoadBalancer<T>& rLoadBalancer)
{
  std::swap(mSize, rLoadBalancer.mSize);
  mLocalIndex.swap(rLoadBalancer.mLocalIndex);
  mGlobalIndex.swap(rLoadBalancer.mGlobalIndex);
  mRank.swap(rLoadBalancer.mRank);
}

template<typename T>
bool LoadBalancer<T>::IsLocal(const gsl::index& rGlobalIndex)
{
  return GetRank(rGlobalIndex) == MpiManager::Instance().GetRank();
}

template<typename T>
bool LoadBalancer<T>::IsLocal(gsl::index globalIndex) const
{
  return GetRank(globalIndex) == MpiManager::Instance().GetRank();
}

template<typename T>
std::size_t LoadBalancer<T>::GetSize() const
{
  return mSize;
}

template<typename T>
gsl::index LoadBalancer<T>::GetLocalIndex(const gsl::index& rGlobalIndex)
{
  Expects(mLocalIndex.find(rGlobalIndex) != mLocalIndex.end());
  return mLocalIndex[rGlobalIndex];
}

template<typename T>
gsl::index LoadBalancer<T>::GetLocalIndex(gsl::index globalIndex) const
{
  auto it_local_index {std::as_const(mLocalIndex).find(globalIndex)};
  Ensures(it_local_index != mLocalIndex.end());
  return it_local_index->second;
}

template<typename T>
gsl::index LoadBalancer<T>::GetGlobalIndex(gsl::index localIndex) const
{
  Expects(localIndex < mGlobalIndex.size());
  return mGlobalIndex[localIndex];
}

template<typename T>
std::size_t LoadBalancer<T>::GetRank(const gsl::index& rGlobalIndex)
{
  Expects(mRank.find(rGlobalIndex) != mRank.end());
  return mRank[rGlobalIndex];
}

template<typename T>
std::size_t LoadBalancer<T>::GetRank(gsl::index globalIndex) const
{
  auto it_rank {std::as_const(mRank).find(globalIndex)};
  Ensures(it_rank != mRank.end());
  return it_rank->second;
}

template<typename T>
bool LoadBalancer<T>::operator==(const LoadBalancer<T>& rRhs) const
{
  return mSize == rRhs.mSize && mLocalIndex == rRhs.mLocalIndex &&
      mGlobalIndex == rRhs.mGlobalIndex && mRank == rRhs.mRank;
}

template<typename T>
bool* LoadBalancer<T>::pGetBlock(
    gsl::index blockIndex
  , std::size_t& rBlockSize
  , bool isLoad/*=false*/)
{
  gsl::index current_block_index {0};
  gsl::index size_buffer_index {0};
  bool* p_data {nullptr};

  // Register mSize (number of cuboids) at block 0
  RegisterVar<std::size_t>(blockIndex, rBlockSize, current_block_index,
      p_data, mSize);
  RegisterMap<gsl::index, gsl::index>(blockIndex, rBlockSize,
      current_block_index, size_buffer_index, p_data, mLocalIndex, isLoad);
  // Register mGlobalIndex (global index of cuboids owned by this
  // LoadBalancer) at block 1
  RegisterVarVector<gsl::index>(blockIndex, rBlockSize,
      current_block_index, size_buffer_index, p_data, mGlobalIndex, isLoad);
  RegisterMap<gsl::index, std::size_t>(blockIndex, rBlockSize,
      current_block_index, size_buffer_index, p_data, mRank, isLoad);

  return p_data;
}

template<typename T>
std::size_t LoadBalancer<T>::GetNumBlock() const
{
  return 4 +  // _size, plus vector length of _loc, _glob and _rank
      mGlobalIndex.size() +
      mLocalIndex.size() +
      mRank.size();
}

template<typename T>
std::size_t LoadBalancer<T>::GetSerializableSize() const
{
  return 4 * sizeof(std::size_t)  +  // mSize, vector length of mLocalIndex,
                                     // mGlobalIndex, and mRank
      mLocalIndex.size() * sizeof(std::pair<gsl::index, gsl::index>) +
      mGlobalIndex.size() * sizeof(gsl::index) +
      mRank.size() * sizeof(std::pair<gsl::index, std::size_t>);
}

template<typename T>
void LoadBalancer<T>::Print(bool isMultiOutput/*=false*/) const
{
  OstreamManager ostream(std::cout, "LoadBalancer");
  ostream.SetIsMultiOutput(isMultiOutput);
  ostream << "mSize = " << mSize << std::endl;
  for (gsl::index i = 0; i < mGlobalIndex.size(); ++i) {
    ostream << "mGlobalIndex[" << i << "] = " << mGlobalIndex[i] <<
        std::endl;
  }
  for (const auto& r_it : mLocalIndex)
      ostream << "loc[" << r_it.first << "]=" << r_it.second << std::endl;
  for (const auto& r_it : mRank)
      ostream << "rank[" << r_it.first << "]=" << r_it.second << std::endl;
  ostream.SetIsMultiOutput(false);
}

// explicit instantiation
template class LoadBalancer<double>;
}  // namespace iblbm
