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
  LoadBalancer<double> balancer(10);
  std::size_t block_size {};
  std::cout << block_size << std::endl;
  auto p_data {balancer.GetBlock(0, block_size)};
  std::cout << block_size << std::endl;
  std::cout << *p_data << std::endl;
  std::cout << balancer.GetSerializableSize() << std::endl;
  balancer.Print();
}

TEST(TestLoadBalancer_Constructor_Load)
{
  auto size {1u};
  std::vector<gsl::index> global_index {0, 1, 2, 3, 4, 5};
  std::map<gsl::index, gsl::index> local_index {{0, 0}, {1, 1}, {2, 2},
      {3, 3}, {4, 4}, {5, 5}};
  std::map<gsl::index, std::size_t> rank_map {{0, 0}, {1, 0}, {2, 0}, {3, 0},
      {4, 0}, {5, 0}};
  LoadBalancer<double> balancer(size, global_index, local_index, rank_map);
  std::size_t block_size {};
  std::cout << block_size << std::endl;
  auto p_data {balancer.GetBlock(0, block_size)};
  std::cout << block_size << std::endl;
  std::cout << *p_data << std::endl;
  std::cout << balancer.GetSerializableSize() << std::endl;
  balancer.Print();
}
}
}  // namespace iblbm

// 0x7ffdf87deb38
