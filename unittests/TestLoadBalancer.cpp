#include <cstddef>
#include <iostream>
#include <map>

#include "gsl/gsl"
#include "UnitTest++/UnitTest++.h"
#include "LoadBalancer.hpp"

namespace iblbm
{
SUITE(TestParallel)
{
TEST(TestLoadBalancer_Constructor_Empty)
{
  auto size {12u};
  LoadBalancer<double> balancer(size);

  CHECK_EQUAL(size, balancer.GetSize());
  CHECK_EQUAL(4u, balancer.GetNumBlock());

  bool* p_data {nullptr};
  std::size_t block_size {};

  // mSize
  p_data = balancer.GetBlock(0, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(size, *reinterpret_cast<std::size_t*>(p_data));

  // mLocalIndex.size()
  p_data = balancer.GetBlock(1, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(0, *reinterpret_cast<std::size_t*>(p_data));

  // mGlobalIndex.size()
  p_data = balancer.GetBlock(2, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(0, *reinterpret_cast<std::size_t*>(p_data));

  // mRank.size()
  p_data = balancer.GetBlock(3, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(0, *reinterpret_cast<std::size_t*>(p_data));

  // We managed to enter the inner if block in RegisterMap due to how the
  // conditional is handled.
  p_data = balancer.GetBlock(4, block_size);
  CHECK_EQUAL(sizeof(std::pair<gsl::index, std::size_t>), block_size);
  CHECK(p_data == nullptr);

  CHECK_EQUAL(4 * sizeof(std::size_t), balancer.GetSerializableSize());
}

TEST(TestLoadBalancer_Constructor_Load)
{
  auto size {1u};
  std::map<gsl::index, gsl::index> local_index {{0, 1}, {2, 3}, {4, 5},
      {6, 7}, {8, 9}, {10, 11}};
  std::vector<gsl::index> global_index {12, 13, 14, 15, 16, 17};
  std::map<gsl::index, std::size_t> rank_map {{18, 19}, {20, 21}, {22, 23},
      {24, 25}, {26, 27}, {28, 29}};

  gsl::index block_index {};
  std::size_t block_size {};
  bool* p_data {nullptr};
  LoadBalancer<double> balancer(size, local_index, global_index, rank_map);

  // mSize
  p_data = balancer.GetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(size, *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  // mLocalIndex.size()
  p_data = balancer.GetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(local_index.size(), *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  // mLocalIndex content
  for (gsl::index i = 0; i < local_index.size(); ++i) {
    p_data = balancer.GetBlock(block_index + i, block_size);
    CHECK_EQUAL(sizeof(std::pair<gsl::index, gsl::index>), block_size);
    auto exp_it {local_index.begin()};
    std::advance(exp_it, i);
    auto act_it {reinterpret_cast<std::pair<gsl::index, gsl::index>*>(
        p_data)};
    CHECK_EQUAL(exp_it->first, act_it->first);
    CHECK_EQUAL(exp_it->second, act_it->second);
  }
  block_index += local_index.size();

  // mGlobalIndex.size()
  p_data = balancer.GetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(global_index.size(), *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  // mGlobalIndex content
  for (gsl::index i = 0; i < global_index.size(); ++i) {
    p_data = balancer.GetBlock(block_index + i, block_size);
    CHECK_EQUAL(sizeof(gsl::index), block_size);
    CHECK_EQUAL(global_index[i], *reinterpret_cast<gsl::index*>(p_data));
  }
  block_index += global_index.size();

  // mRank.size()
  p_data = balancer.GetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(rank_map.size(), *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  // mRank content
  for (gsl::index i = 0; i < rank_map.size(); ++i) {
    p_data = balancer.GetBlock(block_index + i, block_size);
    CHECK_EQUAL(sizeof(std::pair<gsl::index, gsl::index>), block_size);
    auto exp_it {rank_map.begin()};
    std::advance(exp_it, i);
    auto act_it {reinterpret_cast<std::pair<gsl::index, std::size_t>*>(
        p_data)};
    CHECK_EQUAL(exp_it->first, act_it->first);
    CHECK_EQUAL(exp_it->second, act_it->second);
  }
  block_index += rank_map.size();

  // We managed to enter the inner if-block in RegisterMap due to how the
  // conditional is handled.
  p_data = balancer.GetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::pair<gsl::index, std::size_t>), block_size);
  CHECK(p_data == nullptr);

  auto num_block {4 + global_index.size() + local_index.size() +
      rank_map.size()};
  CHECK_EQUAL(num_block, balancer.GetNumBlock());

  auto serializable_size {4 * sizeof(std::size_t) + local_index.size() *
      sizeof(std::pair<gsl::index, gsl::index>) + global_index.size() *
      sizeof(gsl::index) + rank_map.size() *
      sizeof(std::pair<gsl::index, std::size_t>)};
  CHECK_EQUAL(serializable_size, balancer.GetSerializableSize());
}
}
}  // namespace iblbm
