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
  Edge edge;
  bool is_corner;

  PeriodicNode(
      std::size_t n_pos
    , Edge edge_no
    , bool corner)
    : Node(n_pos),
      edge(edge_no),
      is_corner(corner)
  {}

  ~PeriodicNode() = default;
};

class PeriodicBoundary : public AbstractBoundaryCondition
{
 public:
  PeriodicBoundary(
      AbstractLatticeModel& r_lm
    , AbstractCollisionModel& r_cm);

  ~PeriodicBoundary() = default;

  void UpdateNodes(
      std::vector<std::vector<double>>& r_df
    , bool is_modify_stream);

  void UpdateCorner(
      std::vector<std::vector<double>>& r_df
    , PeriodicNode& r_node
    , bool is_modify_stream);

  void UpdateEdge(
      std::vector<std::vector<double>>& r_df
    , PeriodicNode& r_node
    , bool is_modify_stream);

  void AddNode(
      std::size_t x
    , std::size_t y);

  const std::vector<PeriodicNode>& rGetNodes() const;

 private:
  AbstractCollisionModel& r_cm_;

  std::vector<PeriodicNode> nodes_;
};

}  // namespace iblbm

#endif  // SRC_BOUNDARY_PERIODICBOUNDARY_HPP_
