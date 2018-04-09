#include <iomanip>
#include <iostream>

#include "UnitTest++/UnitTest++.h"
#include "LoadBalancer.hpp"
#include "MpiManager.hpp"
#include "Serializer.hpp"

namespace iblbm
{
class TestSerializer
{
 public:
  void ValidateFilename(
      Serializer& rSerializer
    , std::string& rFilename)
  {
    rSerializer.ValidateFilename(rFilename);
  }

  std::string GetFullFilename(
      Serializer& rSerializer
    , const std::string& rFilename)
  {
    return rSerializer.GetFullFilename(rFilename);
  }

  gsl::index GetBlockIndex(Serializer& rSerializer) const
  {
    return rSerializer.mBlockIndex;
  }

  std::string GetFilename(Serializer& rSerializer) const
  {
    return rSerializer.mFilename;
  }

  void SetBlockIndex(
      Serializer& rSerializer
    , gsl::index blockIndex)
  {
    rSerializer.mBlockIndex = blockIndex;
  }
};

SUITE(TestCore)
{
TEST(TestSerializer_Constructor_Default)
{
  TestSerializer tester;
  LoadBalancer<double> balancer;
  Serializer serializer {balancer};

  CHECK_EQUAL(0u, serializer.GetSize());
  CHECK_EQUAL("Serializable", tester.GetFilename(serializer));
}

TEST(TestSerializer_Constructor_UserDefined)
{
  TestSerializer tester;
  LoadBalancer<double> balancer;
  std::string filename {"filename"};
  Serializer serializer {balancer, filename};

  CHECK_EQUAL(0u, serializer.GetSize());
  CHECK_EQUAL(filename, tester.GetFilename(serializer));
}

TEST(TestSerializer_Constructor_UserDefined_Override)
{
  TestSerializer tester;
  LoadBalancer<double> balancer;
  std::string filename {""};
  Serializer serializer {balancer, filename};

  CHECK_EQUAL(0u, serializer.GetSize());
  CHECK_EQUAL("Serializable", tester.GetFilename(serializer));
}

TEST(TestSerializer_ResetCounter)
{
  TestSerializer tester;
  LoadBalancer<double> balancer;
  Serializer serializer {balancer};

  CHECK_EQUAL(0, tester.GetBlockIndex(serializer));
  gsl::index index {10};
  tester.SetBlockIndex(serializer, index);
  CHECK_EQUAL(index, tester.GetBlockIndex(serializer));
  serializer.ResetCounter();
  CHECK_EQUAL(0, tester.GetBlockIndex(serializer));
}

TEST(TestSerializer_ComputeSize_EmptySerializable)
{
  TestSerializer tester;
  LoadBalancer<double> balancer;
  Serializer serializer {balancer};

  serializer.ComputeSize();
  CHECK_EQUAL(4 * sizeof(std::size_t), serializer.GetSize());
}

TEST(TestSerializer_ComputeSize)
{
  TestSerializer tester;
  std::size_t size {1u};
  std::map<gsl::index, gsl::index> local_index {{24, 25}, {26, 27}};
  std::vector<gsl::index> global_index {28, 29};
  std::map<gsl::index, std::size_t> rank_map {{30, 0}, {31, 1}};
  LoadBalancer<double> balancer(size, local_index, global_index, rank_map);
  Serializer serializer {balancer};

  serializer.ComputeSize();
  auto serializable_size {4 * sizeof(std::size_t) + local_index.size() *
      sizeof(std::pair<gsl::index, gsl::index>) + global_index.size() *
      sizeof(gsl::index) + rank_map.size() *
      sizeof(std::pair<gsl::index, gsl::index>)};
  CHECK_EQUAL(serializable_size, serializer.GetSize());
}

TEST(TestSerializer_ComputeSize_ForceRecompute)
{
  TestSerializer tester;
  std::size_t size {1u};
  std::map<gsl::index, gsl::index> local_index {{24, 25}, {26, 27}};
  std::vector<gsl::index> global_index {28, 29};
  std::map<gsl::index, std::size_t> rank_map {{30, 0}, {31, 1}};
  LoadBalancer<double> balancer(size, local_index, global_index, rank_map);
  Serializer serializer {balancer};

  serializer.ComputeSize();
  auto serializable_size {4 * sizeof(std::size_t) + local_index.size() *
      sizeof(std::pair<gsl::index, gsl::index>) + global_index.size() *
      sizeof(gsl::index) + rank_map.size() *
      sizeof(std::pair<gsl::index, gsl::index>)};
  CHECK_EQUAL(serializable_size, serializer.GetSize());

  // Load data into balancer to check forceRecompute
  std::map<gsl::index, gsl::index> local_index_2 {{0, 1}, {2, 3}, {4, 5},
      {6, 7}, {8, 9}, {10, 11}};
  std::vector<gsl::index> global_index_2 {12, 13, 14, 15, 16, 17};
  std::map<gsl::index, std::size_t> rank_map_2 {{18, 0}, {19, 1}, {20, 2},
      {21, 0}, {22, 1}, {23, 2}};

  gsl::index block_index {};
  bool* p_data {nullptr};
  std::size_t block_size {};

  // mSize
  p_data = balancer.pGetBlock(block_index, block_size, /*isLoad=*/true);
  auto p_size {reinterpret_cast<bool*>(&size)};
  for (gsl::index i = 0; i < block_size; ++i) p_data[i] = p_size[i];
  ++block_index;

  // mLocalIndex.size()
  p_data = balancer.pGetBlock(block_index, block_size, /*isLoad=*/true);
  auto local_index_size {local_index_2.size()};
  auto p_local_index_size {reinterpret_cast<bool*>(&local_index_size)};
  for (gsl::index i = 0; i < block_size; ++i)
      p_data[i] = p_local_index_size[i];
  ++block_index;

  for (gsl::index i = 0; i < local_index_2.size() + 1; ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size,
        /*isLoad=*/true);
    if (i < local_index_2.size()) {
      auto it_local_index {local_index_2.begin()};
      std::advance(it_local_index, i);
      auto p_it_local_index {reinterpret_cast<bool*>(&(*it_local_index))};
      for (gsl::index j = 0; j < block_size; ++j)
          p_data[j] = p_it_local_index[j];
    }
  }
  block_index += local_index_2.size();

  // mGlobalIndex.size()
  p_data = balancer.pGetBlock(block_index, block_size);
  auto global_index_size {global_index_2.size()};
  auto p_global_index_size {reinterpret_cast<bool*>(&global_index_size)};
  for (gsl::index i = 0; i < block_size; ++i)
      p_data[i] = p_global_index_size[i];
  ++block_index;

  for (gsl::index i = 0; i < global_index_2.size(); ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size,
        /*isLoad=*/true);
    auto it_global_index {global_index_2.begin()};
    std::advance(it_global_index, i);
    auto p_it_global_index {reinterpret_cast<bool*>(&(*it_global_index))};
    for (gsl::index j = 0; j < block_size; ++j)
        p_data[j] = p_it_global_index[j];
  }
  block_index += global_index_2.size();

  // mRank.size()
  p_data = balancer.pGetBlock(block_index, block_size);
  auto rank_map_size {rank_map_2.size()};
  auto p_rank_map_size {reinterpret_cast<bool*>(&rank_map_size)};
  for (gsl::index i = 0; i < block_size; ++i)
      p_data[i] = p_rank_map_size[i];
  ++block_index;

  for (gsl::index i = 0; i < rank_map_2.size() + 1; ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size,
        /*isLoad=*/true);
    if (i < rank_map_2.size()) {
      auto it_rank_map {rank_map_2.begin()};
      std::advance(it_rank_map, i);
      auto p_it_rank_map {reinterpret_cast<bool*>(&(*it_rank_map))};
      for (gsl::index j = 0; j < block_size; ++j)
          p_data[j] = p_it_rank_map[j];
    }
  }

  // Check that size remained the same without forceRecompute flag
  serializer.ComputeSize();
  CHECK_EQUAL(serializable_size, serializer.GetSize());
  serializer.ComputeSize(/*forceRecompute=*/true);
  serializable_size = 4 * sizeof(std::size_t) + (local_index.size() +
      local_index_2.size()) * sizeof(std::pair<gsl::index, gsl::index>) +
      global_index_2.size() * sizeof(gsl::index) + (rank_map.size() +
      rank_map_2.size()) * sizeof(std::pair<gsl::index, gsl::index>);
  CHECK_EQUAL(serializable_size, serializer.GetSize());
}

TEST(TestSerializer_ValidateFilename_Default)
{
  TestSerializer tester;
  LoadBalancer<double> balancer;
  Serializer serializer {balancer};

  std::string empty_filename {""};
  tester.ValidateFilename(serializer, empty_filename);
  CHECK_EQUAL("Serializable", empty_filename);

  std::string filename {"filename"};
  tester.ValidateFilename(serializer, filename);
  CHECK_EQUAL("filename", filename);
}

TEST(TestSerializer_ValidateFilename_UserDefined)
{
  TestSerializer tester;
  LoadBalancer<double> balancer;
  std::string filename {"filename"};
  Serializer serializer {balancer, filename};

  std::string empty_filename {""};
  tester.ValidateFilename(serializer, empty_filename);
  CHECK_EQUAL(filename, empty_filename);

  std::string filename_2 {"filename_2"};
  tester.ValidateFilename(serializer, filename_2);
  CHECK_EQUAL("filename_2", filename_2);
}

TEST(TestSerializer_GetFullFilename)
{
  TestSerializer tester;
  LoadBalancer<double> balancer;
  Serializer serializer {balancer};

  std::string filename {"filename"};
  std::stringstream filename_stream;
  auto string_length {filename_stream.str().length()};
  filename_stream << filename << "_rank" << std::setw(string_length) <<
      std::setfill('0') << MpiManager::Instance().GetRank() << "_size" <<
      std::setw(string_length) << std::setfill('0') <<
      MpiManager::Instance().GetSize() << ".dat";
  CHECK_EQUAL(filename_stream.str(), tester.GetFullFilename(serializer,
      filename));
}
}
}  // namespace iblbm
