#include "PeriodicBoundary.hpp"

#include <cassert>
#include <stdexcept>

#include "IblbmEnum.hpp"

namespace iblbm
{
PeriodicBoundary::PeriodicBoundary(
    AbstractLatticeModel& rLatticeModel
  , AbstractCollisionModel& rCollisionModel)
  : AbstractBoundaryCondition(/*is_before_stream=*/true,
        /*is_during_stream=*/true, rLatticeModel),
    mrCollisionModel(rCollisionModel)
{}

void PeriodicBoundary::UpdateNodes(
    std::vector<std::vector<double>>& rDF
  , bool isModifyStream)
{
  for (auto& r_node : mNodes) {
    if (r_node.mIsCorner) {
      PeriodicBoundary::UpdateCorner(rDF, r_node, isModifyStream);
    }
    else {
      PeriodicBoundary::UpdateEdge(rDF, r_node, isModifyStream);
    }
  }
}

void PeriodicBoundary::UpdateCorner(
    std::vector<std::vector<double>>& rDF
  , PeriodicNode& rNode
  , bool isModifyStream)
{
  const auto nc = mrLatticeModel.GetNumberOfDirections();
  const auto nx = mrLatticeModel.GetNx();
  const auto ny = mrLatticeModel.GetNy();
  const auto width = nx - 1;
  const auto height = (ny - 1) * nx;
  const auto n = rNode.mIndex;
  if (isModifyStream) {
    switch (rNode.mEdge) {
    case Edge::LOWER_LEFT:
      rDF[n][E] = rNode.mDF[E];
      rDF[n][N] = rNode.mDF[N];
      rDF[n][NE] = rNode.mDF[NE];
      rDF[n][NW] = rNode.mDF[NW];
      rDF[n][SE] = rNode.mDF[SE];
      break;
    case Edge::LOWER_RIGHT:
      rDF[n][N] = rNode.mDF[N];
      rDF[n][W] = rNode.mDF[W];
      rDF[n][NE] = rNode.mDF[NE];
      rDF[n][NW] = rNode.mDF[NW];
      rDF[n][SW] = rNode.mDF[SW];
      break;
    case Edge::UPPER_LEFT:
      rDF[n][E] = rNode.mDF[E];
      rDF[n][S] = rNode.mDF[S];
      rDF[n][NE] = rNode.mDF[NE];
      rDF[n][SW] = rNode.mDF[SW];
      rDF[n][SE] = rNode.mDF[SE];
      break;
    case Edge::UPPER_RIGHT:
      rDF[n][W] = rNode.mDF[W];
      rDF[n][S] = rNode.mDF[S];
      rDF[n][NW] = rNode.mDF[NW];
      rDF[n][SW] = rNode.mDF[SW];
      rDF[n][SE] = rNode.mDF[SE];
      break;
    case Edge::NOT_EDGE:
    case Edge::RIGHT:
    case Edge::UPPER:
    case Edge::LEFT:
    case Edge::LOWER:
    default:
      throw std::runtime_error("Not a side");
    }
  }
  else {
    rNode.mDF.assign(nc, 0.0);
    switch (rNode.mEdge) {
    case Edge::LOWER_LEFT:
      rNode.mDF[E] = rDF[n + width][E];
      rNode.mDF[N] = rDF[n + height][N];
      rNode.mDF[NE] = rDF[n + width + height][NE];
      rNode.mDF[NW] = rDF[n + 1 + height][NW];
      rNode.mDF[SE] = rDF[n + width + nx][SE];
      break;
    case Edge::LOWER_RIGHT:
      rNode.mDF[N] = rDF[n + height][N];
      rNode.mDF[W] = rDF[n - width][W];
      rNode.mDF[NE] = rDF[n - 1 + height][NE];
      rNode.mDF[NW] = rDF[n - width + height][NW];
      rNode.mDF[SW] = rDF[n - width + nx][SW];
      break;
    case Edge::UPPER_LEFT:
      rNode.mDF[E] = rDF[n + width][E];
      rNode.mDF[S] = rDF[n - height][S];
      rNode.mDF[NE] = rDF[n + width - nx][NE];
      rNode.mDF[SW] = rDF[n + 1 - height][SW];
      rNode.mDF[SE] = rDF[n + width - height][SE];
      break;
    case Edge::UPPER_RIGHT:
      rNode.mDF[W] = rDF[n - width][W];
      rNode.mDF[S] = rDF[n - height][S];
      rNode.mDF[NW] = rDF[n - width - nx][NW];
      rNode.mDF[SW] = rDF[n - width - height][SW];
      rNode.mDF[SE] = rDF[n - 1 - height][SE];
      break;
    case Edge::NOT_EDGE:
    case Edge::RIGHT:
    case Edge::UPPER:
    case Edge::LEFT:
    case Edge::LOWER:
    default:
      throw std::runtime_error("Not a side");
    }
  }
}

void PeriodicBoundary::UpdateEdge(
    std::vector<std::vector<double>>& rDF
  , PeriodicNode& rNode
  , bool isModifyStream)
{
  const auto nx = mrLatticeModel.GetNx();
  const auto ny = mrLatticeModel.GetNy();
  const auto width = nx - 1;
  const auto height = (ny - 1) * nx;
  const auto n = rNode.mIndex;
  if (isModifyStream) {
    switch (rNode.mEdge) {
    case Edge::RIGHT:
      rDF[n][W] = rNode.mDF[W];
      rDF[n][NW] = rNode.mDF[NW];
      rDF[n][SW] = rNode.mDF[SW];
      break;
    case Edge::UPPER:
      rDF[n][S] = rNode.mDF[S];
      rDF[n][SW] = rNode.mDF[SW];
      rDF[n][SE] = rNode.mDF[SE];
      break;
    case Edge::LEFT:
      rDF[n][E] = rNode.mDF[E];
      rDF[n][NE] = rNode.mDF[NE];
      rDF[n][SE] = rNode.mDF[SE];
      break;
    case Edge::LOWER:
      rDF[n][N] = rNode.mDF[N];
      rDF[n][NE] = rNode.mDF[NE];
      rDF[n][NW] = rNode.mDF[NW];
      break;
    case Edge::NOT_EDGE:
    case Edge::UPPER_RIGHT:
    case Edge::UPPER_LEFT:
    case Edge::LOWER_LEFT:
    case Edge::LOWER_RIGHT:
    default:
      throw std::runtime_error("Not a side");
    }
  }
  else {
    const auto nc = mrLatticeModel.GetNumberOfDirections();
    rNode.mDF.assign(nc, 0.0);
    switch (rNode.mEdge) {
    case Edge::RIGHT:
      rNode.mDF[W] = rDF[n - width][W];
      rNode.mDF[NW] = rDF[n - width - nx][NW];
      rNode.mDF[SW] = rDF[n - width + nx][SW];
      break;
    case Edge::UPPER:
      rNode.mDF[S] = rDF[n - height][S];
      rNode.mDF[SW] = rDF[n + 1 - height][SW];
      rNode.mDF[SE] = rDF[n - 1 - height][SE];
      break;
    case Edge::LEFT:
      rNode.mDF[E] = rDF[n + width][E];
      rNode.mDF[NE] = rDF[n + width - nx][NE];
      rNode.mDF[SE] = rDF[n + width + nx][SE];
      break;
    case Edge::LOWER:
      rNode.mDF[N] = rDF[n + height][N];
      rNode.mDF[NE] = rDF[n - 1 + height][NE];
      rNode.mDF[NW] = rDF[n + 1 + height][NW];
      break;
    case Edge::NOT_EDGE:
    case Edge::UPPER_RIGHT:
    case Edge::UPPER_LEFT:
    case Edge::LOWER_LEFT:
    case Edge::LOWER_RIGHT:
    default:
      throw std::runtime_error("Not a side");
    }
  }
}

void PeriodicBoundary::AddNode(
    std::size_t x
  , std::size_t y)
{
  const auto nx = mrLatticeModel.GetNx();
  const auto n = y * nx + x;
  const auto edge = mrLatticeModel.DetermineOrientation(n);
  // Periodic boundary only work for rNodes at the edges
  assert(edge != Edge::NOT_EDGE);
  const auto is_corner = edge >= Edge::UPPER_RIGHT &&
      edge <= Edge::LOWER_RIGHT;
  mNodes.push_back(PeriodicNode(n, edge, is_corner));
}

const std::vector<PeriodicNode>& PeriodicBoundary::rGetNodes() const
{
  return mNodes;
}
}  // namespace iblbm
