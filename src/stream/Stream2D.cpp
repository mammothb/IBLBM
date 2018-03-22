#include "Stream2D.hpp"

#include "IblbmEnum.hpp"

namespace iblbm
{
Stream2D::Stream2D(AbstractLatticeModel& rLatticeModel)
  : AbstractStreamModel(rLatticeModel)
{}

std::vector<std::vector<double>> Stream2D::Stream(
    const std::vector<std::vector<double>>& rDF)
{
  const auto nx = mrLatticeModel.GetNx();
  const auto ny = mrLatticeModel.GetNy();
  const auto r_dist = mrLatticeModel.rGetDistance();
  auto ret_df(rDF);
  for (auto n = 0u; n < nx * ny; ++n) {
    const auto is_left = n % nx == 0;
    const auto is_right = n % nx == nx - 1;
    const auto is_lower = n / nx == 0;
    const auto is_upper = n / nx == ny - 1;

    if (!is_left) ret_df[n][E] = rDF[n - r_dist[E]][E];
    if (!is_lower) ret_df[n][N] = rDF[n  - r_dist[N]][N];
    if (!is_right) ret_df[n][W] = rDF[n - r_dist[W]][W];
    if (!is_upper) ret_df[n][S] = rDF[n - r_dist[S]][S];
    if (!(is_lower || is_left)) ret_df[n][NE] = rDF[n - r_dist[NE]][NE];
    if (!(is_lower || is_right)) ret_df[n][NW] = rDF[n - r_dist[NW]][NW];
    if (!(is_upper || is_right)) ret_df[n][SW] = rDF[n - r_dist[SW]][SW];
    if (!(is_upper || is_left)) ret_df[n][SE] = rDF[n - r_dist[SE]][SE];
  }
  return ret_df;
}
}  // namespace iblbm
