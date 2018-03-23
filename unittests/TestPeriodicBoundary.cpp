#include "CollisionNsf.hpp"
#include "Lattice2D.hpp"
#include "PeriodicBoundary.hpp"
#include "Stream2D.hpp"
#include "UnitTestCustomUtilities.hpp"
#include "UnitTest++.h"

namespace iblbm
{
SUITE(TestBoundaryCondition)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
static const auto g_num_dirs = 9u;
static const auto g_nx = 6u;
static const auto g_ny = 8u;
static const auto g_is_modify_stream = true;
static const auto g_k_visco = 0.2;
// Initial values
static const auto g_initial_density = 1.2;
static const std::vector<double> g_initial_velocity = {3.4, 5.6};

TEST(TestPeriodicBoundary_Constructor)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);
  PeriodicBoundary periodic(lattice, collision);

  CHECK_EQUAL(true, periodic.IsBeforeStream());
  CHECK_EQUAL(true, periodic.IsDuringStream());
}

TEST(TestPeriodicBoundary_HorizontalStreaming)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);
  Stream2D stream(lattice);
  PeriodicBoundary periodic(lattice, collision);

  for (auto y = 0u; y < g_ny; ++y) {
    periodic.AddNode(0u, y);
    periodic.AddNode(g_nx - 1, y);
  }
  for (auto x = 1u; x < g_nx - 1; ++x) {
    periodic.AddNode(x, 0u);
    periodic.AddNode(x, g_ny - 1);
  }

  std::vector<double> first_node = {1, 2, 1, 0, 1, 2, 0, 0, 2};
  std::vector<double> second_node = {4, 5, 4, 3, 4, 5, 3, 3, 5};
  std::vector<std::vector<double>> nodes = {first_node, second_node};
  std::vector<double> first_result = {1, 5, 1, 3, 1, 5, 3, 3, 5};
  std::vector<double> second_result = {4, 2, 4, 0, 4, 2, 0, 0, 2};
  std::vector<std::vector<double>> results = {first_result, second_result};
  std::vector<std::vector<double>> df(g_nx * g_ny);

  for (auto n = 0u; n < df.size(); ++n) df[n] = nodes[n % 2];
  periodic.UpdateNodes(df, !g_is_modify_stream);
  df = stream.Stream(df);
  periodic.UpdateNodes(df, g_is_modify_stream);

  for (auto n = 0u; n < df.size(); ++n)
      CHECK(testutil::CheckCloseVector(df[n], results[n % 2], g_zero_tol));
}

TEST(TestPeriodicBoundary_VerticalStreaming)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);
  Stream2D stream(lattice);
  PeriodicBoundary periodic(lattice, collision);

  for (auto y = 0u; y < g_ny; ++y) {
    periodic.AddNode(0u, y);
    periodic.AddNode(g_nx - 1, y);
  }
  for (auto x = 1u; x < g_nx - 1; ++x) {
    periodic.AddNode(x, 0u);
    periodic.AddNode(x, g_ny - 1);
  }

  std::vector<double> first_node = {1, 1, 0, 1, 2, 0, 0, 2, 2};
  std::vector<double> second_node = {4, 4, 3, 4, 5, 3, 3, 5, 5};
  std::vector<std::vector<double>> nodes = {first_node, second_node};
  std::vector<double> first_result = {1, 1, 3, 1, 5, 3, 3, 5, 5};
  std::vector<double> second_result = {4, 4, 0, 4, 2, 0, 0, 2, 2};
  std::vector<std::vector<double>> results = {first_result, second_result};
  std::vector<std::vector<double>> df(g_nx * g_ny);

  for (auto n = 0u; n < df.size(); ++n) df[n] = nodes[(n / g_nx) % 2];
  periodic.UpdateNodes(df, !g_is_modify_stream);
  df = stream.Stream(df);
  periodic.UpdateNodes(df, g_is_modify_stream);

  for (auto n = 0u; n < df.size(); ++n) {
    CHECK(testutil::CheckCloseVector(df[n], results[(n / g_nx) % 2],
        g_zero_tol));
  }
}

TEST(TestPeriodicBoundary_DiagonalStreamingNESW)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);
  Stream2D stream(lattice);
  PeriodicBoundary periodic(lattice, collision);

  for (auto y = 0u; y < g_ny; ++y) {
    periodic.AddNode(0u, y);
    periodic.AddNode(g_nx - 1, y);
  }
  for (auto x = 1u; x < g_nx - 1; ++x) {
    periodic.AddNode(x, 0u);
    periodic.AddNode(x, g_ny - 1);
  }

  std::vector<double> first_node(g_num_dirs, 0);
  std::vector<double> second_node(g_num_dirs, 1);
  std::vector<double> third_node(g_num_dirs, 2);
  std::vector<std::vector<double>> nodes = {first_node, second_node,
      third_node};
  std::vector<double> result_ne = {1, 2, 0};
  std::vector<double> result_sw = {2, 0, 1};
  std::vector<std::vector<double>> df(g_nx * g_ny);

  for (auto n = 0u; n < df.size(); ++n) df[n] = nodes[(n + n / g_nx) % 3];
  periodic.UpdateNodes(df, !g_is_modify_stream);
  df = stream.Stream(df);
  periodic.UpdateNodes(df, g_is_modify_stream);

  for (auto n = 0u; n < df.size(); ++n) {
    if (n / g_nx == 0) {
      CHECK_CLOSE(df[n][0], df[n][NE], g_zero_tol);
    }
    else {
      CHECK_CLOSE(result_ne[static_cast<unsigned>(df[n][0])], df[n][NE],
          g_zero_tol);
    }
    if (n / g_nx == g_ny - 1) {
      CHECK_CLOSE(df[n][0], df[n][SW], g_zero_tol);
    }
    else {
      CHECK_CLOSE(result_sw[static_cast<unsigned>(df[n][0])], df[n][SW],
          g_zero_tol);
    }
  }
}

TEST(TestPeriodicBoundary_DiagonalStreamingNWSE)
{
  Lattice2D lattice(g_nx, g_ny, g_initial_velocity);
  CollisionNsf collision(lattice, g_initial_density);
  Stream2D stream(lattice);
  PeriodicBoundary periodic(lattice, collision);

  for (auto y = 0u; y < g_ny; ++y) {
    periodic.AddNode(0u, y);
    periodic.AddNode(g_nx - 1, y);
  }
  for (auto x = 1u; x < g_nx - 1; ++x) {
    periodic.AddNode(x, 0u);
    periodic.AddNode(x, g_ny - 1);
  }

  std::vector<double> first_node(g_num_dirs, 0);
  std::vector<double> second_node(g_num_dirs, 1);
  std::vector<double> third_node(g_num_dirs, 2);
  std::vector<std::vector<double>> nodes = {first_node, second_node,
      third_node};
  std::vector<double> result_nw = {2, 0, 1};
  std::vector<double> result_se = {1, 2, 0};
  std::vector<std::vector<double>> df(g_nx * g_ny);

  for (auto n = 0u; n < df.size(); ++n) df[n] = nodes[(n - n / g_nx) % 3];
  periodic.UpdateNodes(df, !g_is_modify_stream);
  df = stream.Stream(df);
  periodic.UpdateNodes(df, g_is_modify_stream);

  for (auto n = 0u; n < df.size(); ++n) {
    if (n / g_nx == 0) {
      CHECK_CLOSE(df[n][0], df[n][NW], g_zero_tol);
    }
    else {
      CHECK_CLOSE(result_nw[static_cast<unsigned>(df[n][0])], df[n][NW],
          g_zero_tol);
    }
    if (n / g_nx == g_ny - 1) {
      CHECK_CLOSE(df[n][0], df[n][SE], g_zero_tol);
    }
    else {
      CHECK_CLOSE(result_se[static_cast<unsigned>(df[n][0])], df[n][SE],
          g_zero_tol);
    }
  }
}
}
}  // namespace iblbm
