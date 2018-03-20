#include "IblbmEnum.hpp"
#include "IblbmUtilities.hpp"
#include "Lattice2D.hpp"
#include "Stream2D.hpp"
#include "UnitTest++.h"

namespace iblbm
{
SUITE(TestStreamodel)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
static const auto g_num_dirs = 9u;
static const auto g_nx = 6u;
static const auto g_ny = 8u;

// Initial values
static const std::vector<double> g_initial_velocity = {3.4, 5.6};

TEST(TestStream2D_HorizontalStreaming)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  Stream2D stream(lattice);
  std::vector<double> first_node = {1, 2, 1, 0, 1, 2, 0, 0, 2};
  std::vector<double> second_node = {4, 5, 4, 3, 4, 5, 3, 3, 5};
  std::vector<std::vector<double>> nodes = {first_node, second_node};
  std::vector<double> first_result = {1, 5, 1, 3, 1, 5, 3, 3, 5};
  std::vector<double> second_result = {4, 2, 4, 0, 4, 2, 0, 0, 2};
  std::vector<std::vector<double>> results = {first_result, second_result};
  std::vector<std::vector<double>> df(g_nx * g_ny);

  for (auto n = 0u; n < df.size(); ++n) df[n] = nodes[n % 2];
  df = stream.Stream(df);
  for (auto n = 0u; n < df.size(); ++n) {
    const auto is_left = n % g_nx == 0;
    const auto is_right = n % g_nx == g_nx - 1;
    const auto is_bottom = n / g_nx == 0;
    const auto is_top = n / g_nx == g_ny - 1;
    // We're only concerned with stream the nodes within the walls
    if (!is_left && !is_right && !is_bottom && !is_top) {
      CHECK(util::CheckCloseVector(df[n], results[n % 2], g_zero_tol));
    }
  }
}

TEST(TestStream2D_VerticalStreaming)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  Stream2D stream(lattice);
  std::vector<double> first_node = {1, 1, 0, 1, 2, 0, 0, 2, 2};
  std::vector<double> second_node = {4, 4, 3, 4, 5, 3, 3, 5, 5};
  std::vector<std::vector<double>> nodes = {first_node, second_node};
  std::vector<double> first_result = {1, 1, 3, 1, 5, 3, 3, 5, 5};
  std::vector<double> second_result = {4, 4, 0, 4, 2, 0, 0, 2, 2};
  std::vector<std::vector<double>> results = {first_result, second_result};
  std::vector<std::vector<double>> df(g_nx * g_ny);

  for (auto n = 0u; n < df.size(); ++n) df[n] = nodes[(n / g_nx) % 2];
  df = stream.Stream(df);
  for (auto n = 0u; n < df.size(); ++n) {
    const auto is_left = n % g_nx == 0;
    const auto is_right = n % g_nx == g_nx - 1;
    const auto is_bottom = n / g_nx == 0;
    const auto is_top = n / g_nx == g_ny - 1;
    // We're only concerned with stream the nodes within the walls
    if (!is_left && !is_right && !is_bottom && !is_top) {
      CHECK(util::CheckCloseVector(df[n], results[(n / g_nx) % 2],
          g_zero_tol));
    }
  }
}

TEST(TestStream2D_DiagonalStreamingNESW)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  Stream2D stream(lattice);
  std::vector<double> first_node(g_num_dirs, 0);
  std::vector<double> second_node(g_num_dirs, 1);
  std::vector<double> third_node(g_num_dirs, 2);
  std::vector<std::vector<double>> nodes = {first_node, second_node,
      third_node};
  std::vector<double> result_ne = {1, 2, 0};
  std::vector<double> result_sw = {2, 0, 1};
  std::vector<std::vector<double>> df(g_nx * g_ny);

  for (auto n = 0u; n < df.size(); ++n) df[n] = nodes[(n + n / g_nx) % 3];
  df = stream.Stream(df);
  for (auto n = 0u; n < df.size(); ++n) {
    const auto is_left = n % g_nx == 0;
    const auto is_right = n % g_nx == g_nx - 1;
    const auto is_bottom = n / g_nx == 0;
    const auto is_top = n / g_nx == g_ny - 1;
    // We're only concerned with stream the nodes within the walls
    if (!is_left && !is_right && !is_bottom && !is_top) {
      CHECK_CLOSE(result_ne[static_cast<unsigned>(df[n][0])], df[n][NE],
          g_zero_tol);
      CHECK_CLOSE(result_sw[static_cast<unsigned>(df[n][0])], df[n][SW],
          g_zero_tol);
    }
  }
}

TEST(TestStream2D_DiagonalStreamingNWSE)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  Stream2D stream(lattice);
  std::vector<double> first_node(g_num_dirs, 0);
  std::vector<double> second_node(g_num_dirs, 1);
  std::vector<double> third_node(g_num_dirs, 2);
  std::vector<std::vector<double>> nodes = {first_node, second_node,
      third_node};
  std::vector<double> result_nw = {2, 0, 1};
  std::vector<double> result_se = {1, 2, 0};
  std::vector<std::vector<double>> df(g_nx * g_ny);

  for (auto n = 0u; n < df.size(); ++n) df[n] = nodes[(n - n / g_nx) % 3];
  df = stream.Stream(df);
  for (auto n = 0u; n < df.size(); ++n) {
    const auto is_left = n % g_nx == 0;
    const auto is_right = n % g_nx == g_nx - 1;
    const auto is_bottom = n / g_nx == 0;
    const auto is_top = n / g_nx == g_ny - 1;
    // We're only concerned with stream the nodes within the walls
    if (!is_left && !is_right && !is_bottom && !is_top) {
      CHECK_CLOSE(result_nw[static_cast<unsigned>(df[n][0])], df[n][NW],
          g_zero_tol);
      CHECK_CLOSE(result_se[static_cast<unsigned>(df[n][0])], df[n][SE],
          g_zero_tol);
    }
  }
}
}
}  // namespace iblbm
