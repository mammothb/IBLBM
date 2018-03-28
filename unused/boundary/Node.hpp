#ifndef SRC_BOUDARY_NODE_HPP_
#define SRC_BOUDARY_NODE_HPP_

namespace iblbm
{
struct Node
{
  std::size_t mIndex;
  std::vector<double> mDF;

  Node(std::size_t index)
    : mIndex(index)
  {}

  virtual ~Node() = default;
};
}  // namespace

#endif  // SRC_BOUDARY_NODE_HPP_
