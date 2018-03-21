#include "Stream2D.hpp"

#include "IblbmEnum.hpp"

namespace iblbm
{
Stream2D::Stream2D(AbstractLatticeModel& r_lm)
  : AbstractStreamModel(r_lm)
{}

std::vector<std::vector<double>> Stream2D::Stream(
    const std::vector<std::vector<double>>& r_df)
{
  const auto nx = r_lm_.GetNx();
  const auto ny = r_lm_.GetNy();
  const auto r_dist = r_lm_.rGetDistance();
  auto ret_df(r_df);
  for (auto n = 0u; n < nx * ny; ++n) {
    const auto is_left = n % nx == 0;
    const auto is_right = n % nx == nx - 1;
    const auto is_lower = n / nx == 0;
    const auto is_upper = n / nx == ny - 1;

    if (!is_left) ret_df[n][E] = r_df[n - r_dist[E]][E];
    if (!is_lower) ret_df[n][N] = r_df[n  - r_dist[N]][N];
    if (!is_right) ret_df[n][W] = r_df[n - r_dist[W]][W];
    if (!is_upper) ret_df[n][S] = r_df[n - r_dist[S]][S];
    if (!(is_lower || is_left)) ret_df[n][NE] = r_df[n - r_dist[NE]][NE];
    if (!(is_lower || is_right)) ret_df[n][NW] = r_df[n - r_dist[NW]][NW];
    if (!(is_upper || is_right)) ret_df[n][SW] = r_df[n - r_dist[SW]][SW];
    if (!(is_upper || is_left)) ret_df[n][SE] = r_df[n - r_dist[SE]][SE];
  }
  return ret_df;
}
}  // namespace iblbm
