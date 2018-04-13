#include <cstddef>
#include <iostream>
#include <map>
#include <stdexcept>
#include <utility>

#include "gsl/gsl"
#include "UnitTest++/UnitTest++.h"
#include "LoadBalancer.hpp"
#include "MpiManager.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
SUITE(TestParallel)
{
std::map<gsl::index, gsl::index> g_local_index {{0, 1}, {2, 3}, {4, 5},
    {6, 7}, {8, 9}, {10, 11}};
std::vector<gsl::index> g_global_index {12, 13, 14, 15, 16, 17};
std::map<gsl::index, std::size_t> g_rank_map {{18, 0}, {19, 1}, {20, 2},
    {21, 0}, {22, 1}, {23, 2}};

TEST(TestLoadBalancer_Constructor_Empty)
{
  auto size {12u};
  LoadBalancer<double> balancer(size);

  // Using const methods for all calls first which uses find() and guarantee
  // to not modify map
  CHECK_EQUAL(size, balancer.GetSize());
  // Default num block with empty maps and vectors
  CHECK_EQUAL(4u, balancer.GetNumBlock());
  CHECK_EQUAL(4u * sizeof(std::size_t), balancer.GetSerializableSize());
}

TEST(TestLoadBalancer_Constructor_Load)
{
  auto size {1u};
  LoadBalancer<double> balancer(size, g_local_index, g_global_index,
      g_rank_map);

  CHECK(std::as_const(balancer).IsLocal(18) ==
        (MpiManager::Instance().GetRank() == g_rank_map[18]));
  CHECK(std::as_const(balancer).IsLocal(19) ==
        (MpiManager::Instance().GetRank() == g_rank_map[19]));
  CHECK(std::as_const(balancer).IsLocal(20) ==
        (MpiManager::Instance().GetRank() == g_rank_map[20]));
  CHECK(std::as_const(balancer).IsLocal(21) ==
        (MpiManager::Instance().GetRank() == g_rank_map[21]));
  CHECK(std::as_const(balancer).IsLocal(22) ==
        (MpiManager::Instance().GetRank() == g_rank_map[22]));
  CHECK(std::as_const(balancer).IsLocal(23) ==
        (MpiManager::Instance().GetRank() == g_rank_map[23]));
  CHECK_EQUAL(size, balancer.GetSize());
  CHECK_EQUAL(1, std::as_const(balancer).GetLocalIndex(0));
  CHECK_EQUAL(3, std::as_const(balancer).GetLocalIndex(2));
  CHECK_EQUAL(5, std::as_const(balancer).GetLocalIndex(4));
  CHECK_EQUAL(7, std::as_const(balancer).GetLocalIndex(6));
  CHECK_EQUAL(9, std::as_const(balancer).GetLocalIndex(8));
  CHECK_EQUAL(11, std::as_const(balancer).GetLocalIndex(10));
  CHECK_EQUAL(12, std::as_const(balancer).GetGlobalIndex(0));
  CHECK_EQUAL(13, std::as_const(balancer).GetGlobalIndex(1));
  CHECK_EQUAL(14, std::as_const(balancer).GetGlobalIndex(2));
  CHECK_EQUAL(15, std::as_const(balancer).GetGlobalIndex(3));
  CHECK_EQUAL(16, std::as_const(balancer).GetGlobalIndex(4));
  CHECK_EQUAL(17, std::as_const(balancer).GetGlobalIndex(5));
  CHECK_EQUAL(0, std::as_const(balancer).GetRank(18));
  CHECK_EQUAL(1, std::as_const(balancer).GetRank(19));
  CHECK_EQUAL(2, std::as_const(balancer).GetRank(20));
  CHECK_EQUAL(0, std::as_const(balancer).GetRank(21));
  CHECK_EQUAL(1, std::as_const(balancer).GetRank(22));
  CHECK_EQUAL(2, std::as_const(balancer).GetRank(23));

  auto num_block {4 + g_global_index.size() + g_local_index.size() +
      g_rank_map.size()};
  CHECK_EQUAL(num_block, balancer.GetNumBlock());
  auto serializable_size {4 * sizeof(std::size_t) + g_local_index.size() *
      sizeof(std::pair<gsl::index, gsl::index>) + g_global_index.size() *
      sizeof(gsl::index) + g_rank_map.size() *
      sizeof(std::pair<gsl::index, std::size_t>)};
  CHECK_EQUAL(serializable_size, balancer.GetSerializableSize());

  CHECK(balancer.IsLocal(18) ==
        (MpiManager::Instance().GetRank() == g_rank_map[18]));
  CHECK(balancer.IsLocal(19) ==
        (MpiManager::Instance().GetRank() == g_rank_map[19]));
  CHECK(balancer.IsLocal(20) ==
        (MpiManager::Instance().GetRank() == g_rank_map[20]));
  CHECK(balancer.IsLocal(21) ==
        (MpiManager::Instance().GetRank() == g_rank_map[21]));
  CHECK(balancer.IsLocal(22) ==
        (MpiManager::Instance().GetRank() == g_rank_map[22]));
  CHECK(balancer.IsLocal(23) ==
        (MpiManager::Instance().GetRank() == g_rank_map[23]));
  CHECK_EQUAL(size, balancer.GetSize());
  CHECK_EQUAL(1, balancer.GetLocalIndex(0));
  CHECK_EQUAL(3, balancer.GetLocalIndex(2));
  CHECK_EQUAL(5, balancer.GetLocalIndex(4));
  CHECK_EQUAL(7, balancer.GetLocalIndex(6));
  CHECK_EQUAL(9, balancer.GetLocalIndex(8));
  CHECK_EQUAL(11, balancer.GetLocalIndex(10));
  CHECK_EQUAL(12, balancer.GetGlobalIndex(0));
  CHECK_EQUAL(13, balancer.GetGlobalIndex(1));
  CHECK_EQUAL(14, balancer.GetGlobalIndex(2));
  CHECK_EQUAL(15, balancer.GetGlobalIndex(3));
  CHECK_EQUAL(16, balancer.GetGlobalIndex(4));
  CHECK_EQUAL(17, balancer.GetGlobalIndex(5));
  CHECK_EQUAL(0, balancer.GetRank(18));
  CHECK_EQUAL(1, balancer.GetRank(19));
  CHECK_EQUAL(2, balancer.GetRank(20));
  CHECK_EQUAL(0, balancer.GetRank(21));
  CHECK_EQUAL(1, balancer.GetRank(22));
  CHECK_EQUAL(2, balancer.GetRank(23));

  // Check that using non-const methods did not modify maps since the keys
  // were all present
  CHECK_EQUAL(num_block, balancer.GetNumBlock());
  CHECK_EQUAL(serializable_size, balancer.GetSerializableSize());
}

TEST(TestLoadBalancer_EqualOperator)
{
  auto size {1u};
  LoadBalancer<double> balancer_1(size, g_local_index, g_global_index,
      g_rank_map);
  LoadBalancer<double> balancer_2(size, g_local_index, g_global_index,
      g_rank_map);
  LoadBalancer<double> balancer_3(size);

  CHECK(balancer_1 == balancer_2);
  CHECK(!(balancer_1 == balancer_3));
  CHECK(!(balancer_2 == balancer_3));
}

TEST(TestLoadBalancer_Swap)
{
  auto size_1 {1u};
  std::map<gsl::index, gsl::index> local_index {{0, 1}, {2, 3}, {4, 5},
      {6, 7}, {8, 9}, {10, 11}};
  std::vector<gsl::index> global_index {12, 13, 14, 15, 16, 17};
  std::map<gsl::index, std::size_t> rank_map {{18, 0}, {19, 1}, {20, 2},
      {21, 0}, {22, 1}, {23, 2}};
  LoadBalancer<double> balancer_1(size_1, g_local_index, g_global_index,
      g_rank_map);

  auto size_2 {12u};
  LoadBalancer<double> balancer_2(size_2);

  balancer_1.Swap(balancer_2);

  CHECK_EQUAL(size_2, balancer_1.GetSize());
  // Default num block with empty maps and vectors
  CHECK_EQUAL(4u, balancer_1.GetNumBlock());
  CHECK_EQUAL(4u * sizeof(std::size_t), balancer_1.GetSerializableSize());

  CHECK(std::as_const(balancer_2).IsLocal(18) ==
        (MpiManager::Instance().GetRank() == g_rank_map[18]));
  CHECK(std::as_const(balancer_2).IsLocal(19) ==
        (MpiManager::Instance().GetRank() == g_rank_map[19]));
  CHECK(std::as_const(balancer_2).IsLocal(20) ==
        (MpiManager::Instance().GetRank() == g_rank_map[20]));
  CHECK(std::as_const(balancer_2).IsLocal(21) ==
        (MpiManager::Instance().GetRank() == g_rank_map[21]));
  CHECK(std::as_const(balancer_2).IsLocal(22) ==
        (MpiManager::Instance().GetRank() == g_rank_map[22]));
  CHECK(std::as_const(balancer_2).IsLocal(23) ==
        (MpiManager::Instance().GetRank() == g_rank_map[23]));
  CHECK_EQUAL(size_1, balancer_2.GetSize());
  CHECK_EQUAL(1, std::as_const(balancer_2).GetLocalIndex(0));
  CHECK_EQUAL(3, std::as_const(balancer_2).GetLocalIndex(2));
  CHECK_EQUAL(5, std::as_const(balancer_2).GetLocalIndex(4));
  CHECK_EQUAL(7, std::as_const(balancer_2).GetLocalIndex(6));
  CHECK_EQUAL(9, std::as_const(balancer_2).GetLocalIndex(8));
  CHECK_EQUAL(11, std::as_const(balancer_2).GetLocalIndex(10));
  CHECK_EQUAL(12, std::as_const(balancer_2).GetGlobalIndex(0));
  CHECK_EQUAL(13, std::as_const(balancer_2).GetGlobalIndex(1));
  CHECK_EQUAL(14, std::as_const(balancer_2).GetGlobalIndex(2));
  CHECK_EQUAL(15, std::as_const(balancer_2).GetGlobalIndex(3));
  CHECK_EQUAL(16, std::as_const(balancer_2).GetGlobalIndex(4));
  CHECK_EQUAL(17, std::as_const(balancer_2).GetGlobalIndex(5));
  CHECK_EQUAL(0, std::as_const(balancer_2).GetRank(18));
  CHECK_EQUAL(1, std::as_const(balancer_2).GetRank(19));
  CHECK_EQUAL(2, std::as_const(balancer_2).GetRank(20));
  CHECK_EQUAL(0, std::as_const(balancer_2).GetRank(21));
  CHECK_EQUAL(1, std::as_const(balancer_2).GetRank(22));
  CHECK_EQUAL(2, std::as_const(balancer_2).GetRank(23));

  auto num_block {4 + g_global_index.size() + g_local_index.size() +
      g_rank_map.size()};
  CHECK_EQUAL(num_block, balancer_2.GetNumBlock());
  auto serializable_size {4 * sizeof(std::size_t) + g_local_index.size() *
      sizeof(std::pair<gsl::index, gsl::index>) + g_global_index.size() *
      sizeof(gsl::index) + g_rank_map.size() *
      sizeof(std::pair<gsl::index, std::size_t>)};
  CHECK_EQUAL(serializable_size, balancer_2.GetSerializableSize());

  CHECK(balancer_2.IsLocal(18) ==
        (MpiManager::Instance().GetRank() == g_rank_map[18]));
  CHECK(balancer_2.IsLocal(19) ==
        (MpiManager::Instance().GetRank() == g_rank_map[19]));
  CHECK(balancer_2.IsLocal(20) ==
        (MpiManager::Instance().GetRank() == g_rank_map[20]));
  CHECK(balancer_2.IsLocal(21) ==
        (MpiManager::Instance().GetRank() == g_rank_map[21]));
  CHECK(balancer_2.IsLocal(22) ==
        (MpiManager::Instance().GetRank() == g_rank_map[22]));
  CHECK(balancer_2.IsLocal(23) ==
        (MpiManager::Instance().GetRank() == g_rank_map[23]));
  CHECK_EQUAL(size_1, balancer_2.GetSize());
  CHECK_EQUAL(1, balancer_2.GetLocalIndex(0));
  CHECK_EQUAL(3, balancer_2.GetLocalIndex(2));
  CHECK_EQUAL(5, balancer_2.GetLocalIndex(4));
  CHECK_EQUAL(7, balancer_2.GetLocalIndex(6));
  CHECK_EQUAL(9, balancer_2.GetLocalIndex(8));
  CHECK_EQUAL(11, balancer_2.GetLocalIndex(10));
  CHECK_EQUAL(12, balancer_2.GetGlobalIndex(0));
  CHECK_EQUAL(13, balancer_2.GetGlobalIndex(1));
  CHECK_EQUAL(14, balancer_2.GetGlobalIndex(2));
  CHECK_EQUAL(15, balancer_2.GetGlobalIndex(3));
  CHECK_EQUAL(16, balancer_2.GetGlobalIndex(4));
  CHECK_EQUAL(17, balancer_2.GetGlobalIndex(5));
  CHECK_EQUAL(0, balancer_2.GetRank(18));
  CHECK_EQUAL(1, balancer_2.GetRank(19));
  CHECK_EQUAL(2, balancer_2.GetRank(20));
  CHECK_EQUAL(0, balancer_2.GetRank(21));
  CHECK_EQUAL(1, balancer_2.GetRank(22));
  CHECK_EQUAL(2, balancer_2.GetRank(23));

  // Check that using non-const methods did not modify maps since the keys
  // were all present
  CHECK_EQUAL(num_block, balancer_2.GetNumBlock());
  CHECK_EQUAL(serializable_size, balancer_2.GetSerializableSize());
}

TEST(TestLoadBalancer_pGetBlock_Saving_Empty)
{
  auto size {12u};
  LoadBalancer<double> balancer(size);

  CHECK_EQUAL(size, balancer.GetSize());
  CHECK_EQUAL(4u, balancer.GetNumBlock());

  bool* p_data {nullptr};
  std::size_t block_size {0};

  // mSize
  p_data = balancer.pGetBlock(0, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(size, *reinterpret_cast<std::size_t*>(p_data));

  // mLocalIndex.size()
  p_data = balancer.pGetBlock(1, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(0, *reinterpret_cast<std::size_t*>(p_data));

  // mGlobalIndex.size()
  p_data = balancer.pGetBlock(2, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(0, *reinterpret_cast<std::size_t*>(p_data));

  // mRank.size()
  p_data = balancer.pGetBlock(3, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(0, *reinterpret_cast<std::size_t*>(p_data));

  // We managed to enter the inner if block in RegisterMap due to how the
  // conditional is handled.
  p_data = balancer.pGetBlock(4, block_size);
  CHECK_EQUAL(sizeof(std::pair<gsl::index, std::size_t>), block_size);
  CHECK(p_data == nullptr);

  CHECK_EQUAL(4 * sizeof(std::size_t), balancer.GetSerializableSize());
}

TEST(TestLoadBalancer_pGetBlock_Saving_WithLoad)
{
  auto size {1u};

  gsl::index block_index {0};
  std::size_t block_size {0};
  bool* p_data {nullptr};
  LoadBalancer<double> balancer(size, g_local_index, g_global_index,
      g_rank_map);

  // Checks saving mode of pGetBlock()
  // mSize
  p_data = balancer.pGetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(size, *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  // mLocalIndex.size()
  p_data = balancer.pGetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(g_local_index.size(), *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  // mLocalIndex content
  for (gsl::index i = 0; i < g_local_index.size(); ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size);
    CHECK_EQUAL(sizeof(std::pair<gsl::index, gsl::index>), block_size);
    auto exp_it {g_local_index.begin()};
    std::advance(exp_it, i);
    auto act_it {reinterpret_cast<std::pair<gsl::index, gsl::index>*>(
        p_data)};
    CHECK_EQUAL(exp_it->first, act_it->first);
    CHECK_EQUAL(exp_it->second, act_it->second);
  }
  block_index += g_local_index.size();

  // mGlobalIndex.size()
  p_data = balancer.pGetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(g_global_index.size(),
      *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  // mGlobalIndex content
  for (gsl::index i = 0; i < g_global_index.size(); ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size);
    CHECK_EQUAL(sizeof(gsl::index), block_size);
    CHECK_EQUAL(g_global_index[i], *reinterpret_cast<gsl::index*>(p_data));
  }
  block_index += g_global_index.size();

  // mRank.size()
  p_data = balancer.pGetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(g_rank_map.size(), *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  // mRank content
  for (gsl::index i = 0; i < g_rank_map.size(); ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size);
    CHECK_EQUAL(sizeof(std::pair<gsl::index, gsl::index>), block_size);
    auto exp_it {g_rank_map.begin()};
    std::advance(exp_it, i);
    auto act_it {reinterpret_cast<std::pair<gsl::index, std::size_t>*>(
        p_data)};
    CHECK_EQUAL(exp_it->first, act_it->first);
    CHECK_EQUAL(exp_it->second, act_it->second);
  }
  block_index += g_rank_map.size();

  // We managed to enter the inner if-block in RegisterMap due to how the
  // conditional is handled.
  p_data = balancer.pGetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::pair<gsl::index, std::size_t>), block_size);
  CHECK(p_data == nullptr);

  auto num_block {4 + g_global_index.size() + g_local_index.size() +
      g_rank_map.size()};
  CHECK_EQUAL(num_block, balancer.GetNumBlock());

  auto serializable_size {4 * sizeof(std::size_t) + g_local_index.size() *
      sizeof(std::pair<gsl::index, gsl::index>) + g_global_index.size() *
      sizeof(gsl::index) + g_rank_map.size() *
      sizeof(std::pair<gsl::index, std::size_t>)};
  CHECK_EQUAL(serializable_size, balancer.GetSerializableSize());
}

TEST(TestLoadBalancer_pGetBlock_Loading_Empty)
{
  std::size_t size {1u};
  LoadBalancer<double> balancer(0);

  CHECK_EQUAL(0, balancer.GetSize());
  CHECK_EQUAL(4u, balancer.GetNumBlock());

  gsl::index block_index {0};
  bool* p_data {nullptr};
  std::size_t block_size {0};

  // mSize
  p_data = balancer.pGetBlock(block_index, block_size, /*isLoad=*/true);
  auto p_size {reinterpret_cast<bool*>(&size)};
  for (gsl::index i = 0; i < block_size; ++i) p_data[i] = p_size[i];
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(size, *reinterpret_cast<std::size_t*>(p_data));
  CHECK_EQUAL(size, balancer.GetSize());
  ++block_index;

  // mLocalIndex.size()
  p_data = balancer.pGetBlock(block_index, block_size, /*isLoad=*/true);
  auto local_index_size {g_local_index.size()};
  auto p_local_index_size {reinterpret_cast<bool*>(&local_index_size)};
  for (gsl::index i = 0; i < block_size; ++i)
      p_data[i] = p_local_index_size[i];
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(g_local_index.size(), *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  for (gsl::index i = 0; i < g_local_index.size() + 1; ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size,
        /*isLoad=*/true);
    if (i < g_local_index.size()) {
      auto it_local_index {g_local_index.begin()};
      std::advance(it_local_index, i);
      auto p_it_local_index {reinterpret_cast<bool*>(&(*it_local_index))};
      for (gsl::index j = 0; j < block_size; ++j)
          p_data[j] = p_it_local_index[j];
      CHECK_EQUAL(sizeof(std::pair<gsl::index, gsl::index>), block_size);
    }
  }
  block_index += g_local_index.size();

  CHECK_EQUAL(1, balancer.GetLocalIndex(0));
  CHECK_EQUAL(3, balancer.GetLocalIndex(2));
  CHECK_EQUAL(5, balancer.GetLocalIndex(4));
  CHECK_EQUAL(7, balancer.GetLocalIndex(6));
  CHECK_EQUAL(9, balancer.GetLocalIndex(8));
  CHECK_EQUAL(11, balancer.GetLocalIndex(10));

  // mGlobalIndex.size()
  p_data = balancer.pGetBlock(block_index, block_size);
  auto global_index_size {g_global_index.size()};
  auto p_global_index_size {reinterpret_cast<bool*>(&global_index_size)};
  for (gsl::index i = 0; i < block_size; ++i)
      p_data[i] = p_global_index_size[i];
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(g_global_index.size(),
      *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  for (gsl::index i = 0; i < g_global_index.size(); ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size,
        /*isLoad=*/true);
    auto it_global_index {g_global_index.begin()};
    std::advance(it_global_index, i);
    auto p_it_global_index {reinterpret_cast<bool*>(&(*it_global_index))};
    for (gsl::index j = 0; j < block_size; ++j)
        p_data[j] = p_it_global_index[j];
    CHECK_EQUAL(sizeof(gsl::index), block_size);
  }
  block_index += g_global_index.size();

  CHECK_EQUAL(12, balancer.GetGlobalIndex(0));
  CHECK_EQUAL(13, balancer.GetGlobalIndex(1));
  CHECK_EQUAL(14, balancer.GetGlobalIndex(2));
  CHECK_EQUAL(15, balancer.GetGlobalIndex(3));
  CHECK_EQUAL(16, balancer.GetGlobalIndex(4));
  CHECK_EQUAL(17, balancer.GetGlobalIndex(5));

  // mRank.size()
  p_data = balancer.pGetBlock(block_index, block_size);
  auto rank_map_size {g_rank_map.size()};
  auto p_rank_map_size {reinterpret_cast<bool*>(&rank_map_size)};
  for (gsl::index i = 0; i < block_size; ++i)
      p_data[i] = p_rank_map_size[i];
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(g_rank_map.size(), *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  for (gsl::index i = 0; i < g_rank_map.size() + 1; ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size,
        /*isLoad=*/true);
    if (i < g_rank_map.size()) {
      auto it_rank_map {g_rank_map.begin()};
      std::advance(it_rank_map, i);
      auto p_it_rank_map {reinterpret_cast<bool*>(&(*it_rank_map))};
      for (gsl::index j = 0; j < block_size; ++j)
          p_data[j] = p_it_rank_map[j];
      CHECK_EQUAL(sizeof(std::pair<gsl::index, std::size_t>), block_size);
    }
  }
  block_index += g_rank_map.size();

  CHECK_EQUAL(0, balancer.GetRank(18));
  CHECK_EQUAL(1, balancer.GetRank(19));
  CHECK_EQUAL(2, balancer.GetRank(20));
  CHECK_EQUAL(0, balancer.GetRank(21));
  CHECK_EQUAL(1, balancer.GetRank(22));
  CHECK_EQUAL(2, balancer.GetRank(23));

  auto num_block {4 + g_global_index.size() + g_local_index.size() +
      g_rank_map.size()};
  CHECK_EQUAL(num_block, balancer.GetNumBlock());
}

TEST(TestLoadBalancer_pGetBlock_Loading_WithLoad)
{
  std::size_t size {1u};
  std::map<gsl::index, gsl::index> local_index {{24, 25}, {26, 27}};
  std::vector<gsl::index> global_index {28, 29};
  std::map<gsl::index, std::size_t> rank_map {{30, 0}, {31, 1}};
  LoadBalancer<double> balancer(0, local_index, global_index, rank_map);

  auto num_block {4 + global_index.size() + local_index.size() +
      rank_map.size()};
  CHECK_EQUAL(0, balancer.GetSize());
  CHECK_EQUAL(num_block, balancer.GetNumBlock());

  gsl::index block_index {0};
  bool* p_data {nullptr};
  std::size_t block_size {0};

  // mSize
  p_data = balancer.pGetBlock(block_index, block_size, /*isLoad=*/true);
  auto p_size {reinterpret_cast<bool*>(&size)};
  for (gsl::index i = 0; i < block_size; ++i) p_data[i] = p_size[i];
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(size, *reinterpret_cast<std::size_t*>(p_data));
  CHECK_EQUAL(size, balancer.GetSize());
  ++block_index;

  // mLocalIndex.size()
  p_data = balancer.pGetBlock(block_index, block_size, /*isLoad=*/true);
  auto local_index_size {g_local_index.size()};
  auto p_local_index_size {reinterpret_cast<bool*>(&local_index_size)};
  for (gsl::index i = 0; i < block_size; ++i)
      p_data[i] = p_local_index_size[i];
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(g_local_index.size(), *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  for (gsl::index i = 0; i < g_local_index.size() + 1; ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size,
        /*isLoad=*/true);
    if (i < g_local_index.size()) {
      auto it_local_index {g_local_index.begin()};
      std::advance(it_local_index, i);
      auto p_it_local_index {reinterpret_cast<bool*>(&(*it_local_index))};
      for (gsl::index j = 0; j < block_size; ++j)
          p_data[j] = p_it_local_index[j];
      CHECK_EQUAL(sizeof(std::pair<gsl::index, gsl::index>), block_size);
    }
  }
  block_index += g_local_index.size();

  CHECK_EQUAL(1, balancer.GetLocalIndex(0));
  CHECK_EQUAL(3, balancer.GetLocalIndex(2));
  CHECK_EQUAL(5, balancer.GetLocalIndex(4));
  CHECK_EQUAL(7, balancer.GetLocalIndex(6));
  CHECK_EQUAL(9, balancer.GetLocalIndex(8));
  CHECK_EQUAL(11, balancer.GetLocalIndex(10));
  // Original data not overwritten
  CHECK_EQUAL(25, balancer.GetLocalIndex(24));
  CHECK_EQUAL(27, balancer.GetLocalIndex(26));

  // mGlobalIndex.size()
  p_data = balancer.pGetBlock(block_index, block_size);
  auto global_index_size {g_global_index.size()};
  auto p_global_index_size {reinterpret_cast<bool*>(&global_index_size)};
  for (gsl::index i = 0; i < block_size; ++i)
      p_data[i] = p_global_index_size[i];
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(g_global_index.size(),
      *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  for (gsl::index i = 0; i < g_global_index.size(); ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size,
        /*isLoad=*/true);
    auto it_global_index {g_global_index.begin()};
    std::advance(it_global_index, i);
    auto p_it_global_index {reinterpret_cast<bool*>(&(*it_global_index))};
    for (gsl::index j = 0; j < block_size; ++j)
        p_data[j] = p_it_global_index[j];
    CHECK_EQUAL(sizeof(gsl::index), block_size);
  }
  block_index += g_global_index.size();

  CHECK_EQUAL(12, balancer.GetGlobalIndex(0));
  CHECK_EQUAL(13, balancer.GetGlobalIndex(1));
  CHECK_EQUAL(14, balancer.GetGlobalIndex(2));
  CHECK_EQUAL(15, balancer.GetGlobalIndex(3));
  CHECK_EQUAL(16, balancer.GetGlobalIndex(4));
  CHECK_EQUAL(17, balancer.GetGlobalIndex(5));
  // Original data overwritten
//  CHECK_EQUAL(28, balancer.GetGlobalIndex(6));
//  CHECK_EQUAL(29, balancer.GetGlobalIndex(7));

  // mRank.size()
  p_data = balancer.pGetBlock(block_index, block_size);
  auto rank_map_size {g_rank_map.size()};
  auto p_rank_map_size {reinterpret_cast<bool*>(&rank_map_size)};
  for (gsl::index i = 0; i < block_size; ++i)
      p_data[i] = p_rank_map_size[i];
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(g_rank_map.size(), *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  for (gsl::index i = 0; i < g_rank_map.size() + 1; ++i) {
    p_data = balancer.pGetBlock(block_index + i, block_size,
        /*isLoad=*/true);
    if (i < g_rank_map.size()) {
      auto it_rank_map {g_rank_map.begin()};
      std::advance(it_rank_map, i);
      auto p_it_rank_map {reinterpret_cast<bool*>(&(*it_rank_map))};
      for (gsl::index j = 0; j < block_size; ++j)
          p_data[j] = p_it_rank_map[j];
      CHECK_EQUAL(sizeof(std::pair<gsl::index, std::size_t>), block_size);
    }
  }
  block_index += g_rank_map.size();

  CHECK_EQUAL(0, balancer.GetRank(18));
  CHECK_EQUAL(1, balancer.GetRank(19));
  CHECK_EQUAL(2, balancer.GetRank(20));
  CHECK_EQUAL(0, balancer.GetRank(21));
  CHECK_EQUAL(1, balancer.GetRank(22));
  CHECK_EQUAL(2, balancer.GetRank(23));
  // Original data not overwritten
  CHECK_EQUAL(0, balancer.GetRank(30));
  CHECK_EQUAL(1, balancer.GetRank(31));

  num_block = 4 + local_index.size() + rank_map.size() +
      g_global_index.size() + g_local_index.size() + g_rank_map.size();
  CHECK_EQUAL(num_block, balancer.GetNumBlock());
}
}
}  // namespace iblbm
