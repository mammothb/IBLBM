#ifndef SRC_BOUNDARY_PERIODICBOUNDARY_HPP_
#define SRC_BOUNDARY_PERIODICBOUNDARY_HPP_

#include "AbstractBoundaryCondition.hpp"
#include "AbstractCollisionModel.hpp"
#include "IblbmEnum.hpp"
#include "Node.hpp"

namespace iblbm
{
struct PeriodicNode : Node
{
  Edge mEdge;
  bool mIsCorner;

  PeriodicNode(
      std::size_t index
    , Edge edge
    , bool isCorner)
    : Node(index),
      mEdge(edge),
      mIsCorner(isCorner)
  {}

  ~PeriodicNode() = default;
};

class PeriodicBoundary : public AbstractBoundaryCondition
{
 public:
  PeriodicBoundary(
      AbstractLatticeModel& rLatticeModel
    , AbstractCollisionModel& rCollisionModel);

  ~PeriodicBoundary() = default;

  void UpdateNodes(
      std::vector<std::vector<double>>& rDF
    , bool isModifyStream);

  void UpdateCorner(
      std::vector<std::vector<double>>& rDF
    , PeriodicNode& rNode
    , bool isModifyStream);

  void UpdateEdge(
      std::vector<std::vector<double>>& rDF
    , PeriodicNode& rNode
    , bool isModifyStream);

  void AddNode(
      std::size_t x
    , std::size_t y);

  const std::vector<PeriodicNode>& rGetNodes() const;

 private:
  AbstractCollisionModel& mrCollisionModel;

  std::vector<PeriodicNode> mNodes;
};

}  // namespace iblbm

#endif  // SRC_BOUNDARY_PERIODICBOUNDARY_HPP_
