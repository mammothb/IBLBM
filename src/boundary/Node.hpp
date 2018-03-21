#ifndef SRC_BOUDARY_NODE_HPP_
#define SRC_BOUDARY_NODE_HPP_

namespace iblbm
{
struct Node
{
  std::size_t n;
  std::vector<double> df;

  Node(std::size_t n_pos)
    : n(n_pos)
  {}

  virtual ~Node() = default;
};
}  // namespace

#endif  // SRC_BOUDARY_NODE_HPP_
