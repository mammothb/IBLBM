#include "BounceBackBoundary.hpp"

#include <memory>

#include "AbstractBoundaryCondition.hpp"
#include "IblbmEnum.hpp"
#include "Node.hpp"

namespace iblbm
{
BounceBackBoundary::BounceBackBoundary(
    AbstractLatticeModel& r_lm
  , AbstractCollisionModel* p_cm/*=nullptr*/)
  : AbstractBoundaryCondition(/*is_before_stream=*/true,
        /*is_during_stream=*/!p_cm, r_lm),
    p_cm_(p_cm)
{}

void BounceBackBoundary::UpdateNodes(
    std::vector<std::vector<double>>& r_df
  , bool is_modify_stream)
{
  if (is_modify_stream) {
    ;
  }
  else {
    if (p_cm_) {
      // Reflect the vectors in node for fullway bounce back node
      for (auto node : nodes_) {
        const auto n = node.n;
        const auto node_df = r_df[n];
        r_df[n][E] = node_df[W];
        r_df[n][N] = node_df[S];
        r_df[n][W] = node_df[E];
        r_df[n][S] = node_df[N];
        r_df[n][NE] = node_df[SW];
        r_df[n][NW] = node_df[SE];
        r_df[n][SW] = node_df[NE];
        r_df[n][SE] = node_df[NW];
      }
    }
    else {
      ;
    }
  }
}

void BounceBackBoundary::AddNode(
    std::size_t x
  , std::size_t y)
{
  const auto nx = r_lm_.GetNx();
  const auto n = y * nx + x;
  nodes_.push_back(Node(n));
  if (p_cm_) p_cm_->AddNodeToSkip(n);
}

const std::vector<Node>& BounceBackBoundary::rGetNodes() const
{
  return nodes_;
}
}  // namespace iblbm
