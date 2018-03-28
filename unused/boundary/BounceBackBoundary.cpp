#include "BounceBackBoundary.hpp"

#include <memory>

#include "AbstractBoundaryCondition.hpp"
#include "IblbmEnum.hpp"
#include "Node.hpp"

namespace iblbm
{
BounceBackBoundary::BounceBackBoundary(
    AbstractLatticeModel& rLatticeModel
  , AbstractCollisionModel* pCollisionModel/*=nullptr*/)
  : AbstractBoundaryCondition(/*is_before_stream=*/true,
        /*is_during_stream=*/!pCollisionModel, rLatticeModel),
    mpCollisionModel(pCollisionModel)
{}

void BounceBackBoundary::UpdateNodes(
    std::vector<std::vector<double>>& rDF
  , bool isModifyStream)
{
  if (isModifyStream) {
    ;
  }
  else {
    if (mpCollisionModel) {
      // Reflect the vectors in node for fullway bounce back node
      for (auto node : mNodes) {
        const auto n = node.mIndex;
        const auto node_df = rDF[n];
        rDF[n][E] = node_df[W];
        rDF[n][N] = node_df[S];
        rDF[n][W] = node_df[E];
        rDF[n][S] = node_df[N];
        rDF[n][NE] = node_df[SW];
        rDF[n][NW] = node_df[SE];
        rDF[n][SW] = node_df[NE];
        rDF[n][SE] = node_df[NW];
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
  const auto nx = mrLatticeModel.GetNx();
  const auto n = y * nx + x;
  mNodes.push_back(Node(n));
  if (mpCollisionModel) mpCollisionModel->AddNodeToSkip(n);
}

const std::vector<Node>& BounceBackBoundary::rGetNodes() const
{
  return mNodes;
}
}  // namespace iblbm
