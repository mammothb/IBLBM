#include "LoadBalancer.hpp"

#include <iostream>

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
void LoadBalancer<T>::swap(LoadBalancer<T>& rLoadBalancer)
{
  std::swap(mSize, rLoadBalancer.mSize);
  mLocalIndex.swap(rLoadBalancer.mLocalIndex);
  mGlobalIndex.swap(rLoadBalancer.mGlobalIndex);
  mRank.swap(rLoadBalancer.mRank);
}

template<typename T>
std::size_t LoadBalancer<T>::GetSize() const
{
  return mSize;
}

template<typename T>
bool* LoadBalancer<T>::GetBlock(
    gsl::index blockIndex
  , std::size_t& rBlockSize
  , bool isLoad/*=false*/)
{
  gsl::index current_block_index {};
  gsl::index size_buffer_index {};
  bool* p_data {nullptr};

  // Register mSize (number of cuboids) at block 0
  this->RegisterVar<std::size_t>(blockIndex, rBlockSize, current_block_index,
      p_data, mSize);
  this->RegisterMap<gsl::index, gsl::index>(blockIndex, rBlockSize,
      current_block_index, size_buffer_index, p_data, mLocalIndex,
      isLoad);
  // Register mGlobalIndex (global index of cuboids owned by this
  // LoadBalancer) at block 1
  this->RegisterVarVector<gsl::index>(blockIndex, rBlockSize,
      current_block_index, size_buffer_index, p_data, mGlobalIndex, isLoad);
  this->RegisterMap<gsl::index, std::size_t>(blockIndex, rBlockSize,
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
  ostream << "mSize = " << this->mSize << std::endl;
  for (gsl::index i = 0; i < this->mGlobalIndex.size(); ++i) {
    ostream << "mGlobalIndex[" << i << "] = " << this->mGlobalIndex[i] <<
        std::endl;
  }
  for (auto it = this->mLocalIndex.cbegin(); it != this->mLocalIndex.cend();
       it++) {
    ostream << "loc[" << (*it).first << "]=" << (*it).second << std::endl;
  }
  for (auto it = this->mRank.cbegin(); it != this->mRank.cend();
       it++) {
    ostream << "rank[" << (*it).first << "]=" << (*it).second << std::endl;
  }
  ostream.SetIsMultiOutput(false);
}

// explicit instantiation
template class LoadBalancer<double>;
}  // namespace iblbm
