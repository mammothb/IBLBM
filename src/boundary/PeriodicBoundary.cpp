#include "PeriodicBoundary.hpp"

#include <cassert>
#include <stdexcept>

#include "IblbmEnum.hpp"

namespace iblbm
{
PeriodicBoundary::PeriodicBoundary(
    AbstractLatticeModel& r_lm
  , AbstractCollisionModel& r_cm)
  : AbstractBoundaryCondition(/*is_before_stream=*/true,
        /*is_during_stream=*/true, r_lm),
    r_cm_(r_cm)
{}

void PeriodicBoundary::UpdateNodes(
    std::vector<std::vector<double>>& r_df
  , bool is_modify_stream)
{
  for (auto& node : nodes_) {
    if (node.is_corner) {
      PeriodicBoundary::UpdateCorner(r_df, node, is_modify_stream);
    }
    else {
      PeriodicBoundary::UpdateEdge(r_df, node, is_modify_stream);
    }
  }
}

void PeriodicBoundary::UpdateCorner(
    std::vector<std::vector<double>>& r_df
  , PeriodicNode& r_node
  , bool is_modify_stream)
{
  const auto nc = r_lm_.GetNumberOfDirections();
  const auto nx = r_lm_.GetNx();
  const auto ny = r_lm_.GetNy();
  const auto width = nx - 1;
  const auto height = (ny - 1) * nx;
  const auto n = r_node.n;
  if (is_modify_stream) {
    switch (r_node.edge) {
    case Edge::LOWER_LEFT:
      r_df[n][E] = r_node.df[E];
      r_df[n][N] = r_node.df[N];
      r_df[n][NE] = r_node.df[NE];
      r_df[n][NW] = r_node.df[NW];
      r_df[n][SE] = r_node.df[SE];
      break;
    case Edge::LOWER_RIGHT:
      r_df[n][N] = r_node.df[N];
      r_df[n][W] = r_node.df[W];
      r_df[n][NE] = r_node.df[NE];
      r_df[n][NW] = r_node.df[NW];
      r_df[n][SW] = r_node.df[SW];
      break;
    case Edge::UPPER_LEFT:
      r_df[n][E] = r_node.df[E];
      r_df[n][S] = r_node.df[S];
      r_df[n][NE] = r_node.df[NE];
      r_df[n][SW] = r_node.df[SW];
      r_df[n][SE] = r_node.df[SE];
      break;
    case Edge::UPPER_RIGHT:
      r_df[n][W] = r_node.df[W];
      r_df[n][S] = r_node.df[S];
      r_df[n][NW] = r_node.df[NW];
      r_df[n][SW] = r_node.df[SW];
      r_df[n][SE] = r_node.df[SE];
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
    r_node.df.assign(nc, 0.0);
    switch (r_node.edge) {
    case Edge::LOWER_LEFT:
      r_node.df[E] = r_df[n + width][E];
      r_node.df[N] = r_df[n + height][N];
      r_node.df[NE] = r_df[n + width + height][NE];
      r_node.df[NW] = r_df[n + 1 + height][NW];
      r_node.df[SE] = r_df[n + width + nx][SE];
      break;
    case Edge::LOWER_RIGHT:
      r_node.df[N] = r_df[n + height][N];
      r_node.df[W] = r_df[n - width][W];
      r_node.df[NE] = r_df[n - 1 + height][NE];
      r_node.df[NW] = r_df[n - width + height][NW];
      r_node.df[SW] = r_df[n - width + nx][SW];
      break;
    case Edge::UPPER_LEFT:
      r_node.df[E] = r_df[n + width][E];
      r_node.df[S] = r_df[n - height][S];
      r_node.df[NE] = r_df[n + width - nx][NE];
      r_node.df[SW] = r_df[n + 1 - height][SW];
      r_node.df[SE] = r_df[n + width - height][SE];
      break;
    case Edge::UPPER_RIGHT:
      r_node.df[W] = r_df[n - width][W];
      r_node.df[S] = r_df[n - height][S];
      r_node.df[NW] = r_df[n - width - nx][NW];
      r_node.df[SW] = r_df[n - width - height][SW];
      r_node.df[SE] = r_df[n - 1 - height][SE];
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
    std::vector<std::vector<double>>& df
  , PeriodicNode& node
  , bool is_modify_stream)
{
  const auto nx = r_lm_.GetNx();
  const auto ny = r_lm_.GetNy();
  const auto width = nx - 1;
  const auto height = (ny - 1) * nx;
  const auto n = node.n;
  if (is_modify_stream) {
    switch (node.edge) {
    case Edge::RIGHT:
      df[n][W] = node.df[W];
      df[n][NW] = node.df[NW];
      df[n][SW] = node.df[SW];
      break;
    case Edge::UPPER:
      df[n][S] = node.df[S];
      df[n][SW] = node.df[SW];
      df[n][SE] = node.df[SE];
      break;
    case Edge::LEFT:
      df[n][E] = node.df[E];
      df[n][NE] = node.df[NE];
      df[n][SE] = node.df[SE];
      break;
    case Edge::LOWER:
      df[n][N] = node.df[N];
      df[n][NE] = node.df[NE];
      df[n][NW] = node.df[NW];
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
    const auto nc = r_lm_.GetNumberOfDirections();
    node.df.assign(nc, 0.0);
    switch (node.edge) {
    case Edge::RIGHT:
      node.df[W] = df[n - width][W];
      node.df[NW] = df[n - width - nx][NW];
      node.df[SW] = df[n - width + nx][SW];
      break;
    case Edge::UPPER:
      node.df[S] = df[n - height][S];
      node.df[SW] = df[n + 1 - height][SW];
      node.df[SE] = df[n - 1 - height][SE];
      break;
    case Edge::LEFT:
      node.df[E] = df[n + width][E];
      node.df[NE] = df[n + width - nx][NE];
      node.df[SE] = df[n + width + nx][SE];
      break;
    case Edge::LOWER:
      node.df[N] = df[n + height][N];
      node.df[NE] = df[n - 1 + height][NE];
      node.df[NW] = df[n + 1 + height][NW];
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
  const auto nx = r_lm_.GetNx();
  const auto n = y * nx + x;
  const auto edge = r_lm_.DetermineOrientation(n);
  // Periodic boundary only work for r_nodes at the edges
  assert(edge != Edge::NOT_EDGE);
  const auto is_corner = edge >= Edge::UPPER_RIGHT &&
      edge <= Edge::LOWER_RIGHT;
  nodes_.push_back(PeriodicNode(n, edge, is_corner));
}

const std::vector<PeriodicNode>& PeriodicBoundary::rGetNodes() const
{
  return nodes_;
}
}  // namespace iblbm
