#include <iostream>
#include <limits>

#include "UnitTest++/UnitTest++.h"
#include "Cuboid2D.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
SUITE(TestGeometry)
{
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestCuboid2D_Constructor_Default)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {5.6};
  auto nx {7u};
  auto ny {8u};

  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);

  auto exp_phys_perimeter {2.0 * delta_R * nx + 2.0 * delta_R * ny};
  auto exp_lattice_perimenter {2 * nx + 2 * ny - 4};
  auto exp_phys_volume {delta_R * nx * delta_R * ny};
  auto exp_lattice_volume {nx * ny};

  CHECK_CLOSE(x_pos, cuboid.GetGlobalXPosition(), g_zero_tol);
  CHECK_CLOSE(y_pos, cuboid.GetGlobalYPosition(), g_zero_tol);
  CHECK_CLOSE(x_pos, cuboid.GetOrigin()[0], g_zero_tol);
  CHECK_CLOSE(y_pos, cuboid.GetOrigin()[1], g_zero_tol);
  CHECK_EQUAL(nx, cuboid.GetNx());
  CHECK_EQUAL(ny, cuboid.GetNy());
  CHECK_EQUAL(nx, cuboid.GetLatticeExtent()[0]);
  CHECK_EQUAL(ny, cuboid.GetLatticeExtent()[1]);
  CHECK_EQUAL(std::numeric_limits<std::size_t>::max(), cuboid.GetWeight());
  CHECK_CLOSE(exp_phys_perimeter, cuboid.GetPhysPerimeter(), g_loose_tol);
  CHECK_EQUAL(exp_lattice_perimenter, cuboid.GetLatticePerimeter());
  CHECK_CLOSE(exp_phys_volume, cuboid.GetPhysVolume(), g_loose_tol);
  CHECK_EQUAL(exp_lattice_volume, cuboid.GetLatticeVolume());
}

TEST(TestCuboid2D_Divide_RowAndColumn_Divisible)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);

  auto num_col {5u};
  auto num_row {12u};
  std::vector<Cuboid2D<double>> cuboid_vec;
  cuboid.Divide(num_col, num_row, cuboid_vec);

  std::vector<double> exp_x_pos;
  std::vector<double> exp_y_pos;
  auto child_x_pos {x_pos};
  for (gsl::index x = 0; x < num_col; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + num_col - 1 - x) / num_col};
    for (gsl::index y = 0; y < num_row; ++y) {
      auto ny_child {(ny + num_row - 1 - y) / num_row};
      exp_x_pos.push_back(child_x_pos);
      exp_y_pos.push_back(child_y_pos);
      child_y_pos += ny_child * delta_R;
    }
    child_x_pos += nx_child * delta_R;
  }

  CHECK_EQUAL(num_col * num_row, cuboid_vec.size());
  for (gsl::index i = 0; i < cuboid_vec.size(); ++i) {
    CHECK_CLOSE(exp_x_pos[i], cuboid_vec[i].GetGlobalXPosition(),
        g_loose_tol);
  }
}

TEST(TestCuboid2D_Divide_RowAndColumn_Indivisible)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {23u};
  auto ny {67u};
  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);

  auto num_col {7u};
  auto num_row {11u};
  std::vector<Cuboid2D<double>> cuboid_vec;
  cuboid.Divide(num_col, num_row, cuboid_vec);

  std::vector<double> exp_x_pos;
  std::vector<double> exp_y_pos;
  auto child_x_pos {x_pos};
  for (gsl::index x = 0; x < num_col; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + num_col - 1 - x) / num_col};
    for (gsl::index y = 0; y < num_row; ++y) {
      auto ny_child {(ny + num_row - 1 - y) / num_row};
      exp_x_pos.push_back(child_x_pos);
      exp_y_pos.push_back(child_y_pos);
      child_y_pos += ny_child * delta_R;
    }
    child_x_pos += nx_child * delta_R;
  }

  CHECK_EQUAL(num_col * num_row, cuboid_vec.size());
  for (gsl::index i = 0; i < cuboid_vec.size(); ++i) {
    CHECK_CLOSE(exp_x_pos[i], cuboid_vec[i].GetGlobalXPosition(),
        g_loose_tol);
  }
}

TEST(TestCuboid2D_Divide_Number_Divisible)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);

  auto num {8};
  std::vector<Cuboid2D<double>> cuboid_vec;
  cuboid.Divide(num, cuboid_vec);

  std::size_t num_col {0};
  std::size_t num_row {0};

  // Determine the best layout for child cuboids
  const auto best_ratio {static_cast<double>(nx) / static_cast<double>(ny)};
  auto best_diff {std::numeric_limits<double>::max()};
  for (gsl::index col = 1; col <= num; ++col) {
    const auto row {num / col};
    if (col * row <= num) {
      const auto curr_diff {std::abs(best_ratio - static_cast<double>(col) /
          static_cast<double>(row))};
      if (curr_diff <= best_diff) {
        best_diff = curr_diff;
        num_col = col;
        num_row = row;
      }
    }
  }

  // Calculated expected node positions
  std::vector<double> exp_x_pos;
  std::vector<double> exp_y_pos;
  auto child_x_pos {x_pos};
  for (gsl::index x = 0; x < num_col; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + num_col - 1 - x) / num_col};
    for (gsl::index y = 0; y < num_row; ++y) {
      auto ny_child {(ny + num_row - 1 - y) / num_row};
      exp_x_pos.push_back(child_x_pos);
      exp_y_pos.push_back(child_y_pos);
      child_y_pos += ny_child * delta_R;
    }
    child_x_pos += nx_child * delta_R;
  }

  CHECK_EQUAL(num, cuboid_vec.size());
  for (gsl::index i = 0; i < cuboid_vec.size(); ++i) {
    CHECK_CLOSE(exp_x_pos[i], cuboid_vec[i].GetGlobalXPosition(),
        g_loose_tol);
  }
}

TEST(TestCuboid2D_Divide_Number_Indivisible_InsertColumn)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);

  auto num {15};
  std::vector<Cuboid2D<double>> cuboid_vec;
  cuboid.Divide(num, cuboid_vec);

  std::size_t num_col {0};
  std::size_t num_row {0};

  // Determine the best layout for child cuboids
  const auto best_ratio {static_cast<double>(nx) / static_cast<double>(ny)};
  auto best_diff {std::numeric_limits<double>::max()};
  for (gsl::index col = 1; col <= num; ++col) {
    const auto row {num / col};
    if (col * row <= num) {
      const auto curr_diff {std::abs(best_ratio - static_cast<double>(col) /
          static_cast<double>(row))};
      if (curr_diff <= best_diff) {
        best_diff = curr_diff;
        num_col = col;
        num_row = row;
      }
    }
  }
  const auto remainder {num - num_col * num_row};
  const auto num_same {num_col * (num_row - remainder)};
  // Number of nodes allocated to the "same" child cuboids
  const auto vol_same {static_cast<double>(nx) * static_cast<double>(ny) *
      num_same / static_cast<double>(num)};
  const auto ny_same {static_cast<std::size_t>(vol_same /
      static_cast<double>(nx))};

  // Calculated expected node positions
  std::vector<double> exp_x_pos;
  std::vector<double> exp_y_pos;
  auto child_x_pos {x_pos};
  for (gsl::index x = 0; x < num_col; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + num_col - 1 - x) / num_col};
    for (gsl::index y = 0; y < num_row - remainder; ++y) {
      auto ny_child {(ny + num_row - remainder - y) / (num_row - remainder)};
      exp_x_pos.push_back(child_x_pos);
      exp_y_pos.push_back(child_y_pos);
      child_y_pos += ny_child * delta_R;
    }
    child_x_pos += nx_child * delta_R;
  }

  // Calculate positions for the remainder nodes
  child_x_pos = x_pos;
  for (gsl::index x = 0; x < num_col + 1; ++x) {
    auto child_y_pos {y_pos + delta_R * ny_same};
    auto nx_child {(nx + num_col - x) / (num_col + 1)};
    for (gsl::index y = 0; y < remainder; ++y) {
      auto ny_child {(ny + remainder - 1 - y) / remainder};
      exp_x_pos.push_back(child_x_pos);
      exp_y_pos.push_back(child_y_pos);
      child_y_pos += ny_child * delta_R;
    }
    child_x_pos += nx_child * delta_R;
  }

  CHECK_EQUAL(num, cuboid_vec.size());
  for (gsl::index i = 0; i < cuboid_vec.size(); ++i) {
    CHECK_CLOSE(exp_x_pos[i], cuboid_vec[i].GetGlobalXPosition(),
        g_loose_tol);
  }
}

TEST(TestCuboid2D_Divide_Number_Indivisible_InsertRow)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);

  auto num {13};
  std::vector<Cuboid2D<double>> cuboid_vec;
  cuboid.Divide(num, cuboid_vec);

  std::size_t num_col {0};
  std::size_t num_row {0};

  // Determine the best layout for child cuboids
  const auto best_ratio {static_cast<double>(nx) / static_cast<double>(ny)};
  auto best_diff {std::numeric_limits<double>::max()};
  for (gsl::index col = 1; col <= num; ++col) {
    const auto row {num / col};
    if (col * row <= num) {
      const auto curr_diff {std::abs(best_ratio - static_cast<double>(col) /
          static_cast<double>(row))};
      if (curr_diff <= best_diff) {
        best_diff = curr_diff;
        num_col = col;
        num_row = row;
      }
    }
  }
  const auto remainder {num - num_col * num_row};
  const auto num_same {num_row * (num_col - remainder)};
  // Number of nodes allocated to the "same" child cuboids
  const auto vol_same {static_cast<double>(nx) * static_cast<double>(ny) *
      num_same / static_cast<double>(num)};
  const auto nx_same {static_cast<std::size_t>(vol_same /
        static_cast<double>(ny) + 0.9999)};

  // Calculated expected node positions
  std::vector<double> exp_x_pos;
  std::vector<double> exp_y_pos;
  auto child_x_pos {x_pos};
  for (gsl::index x = 0; x < num_col - remainder; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + num_col - remainder - x) / (num_col - remainder)};
    for (gsl::index y = 0; y < num_row; ++y) {
      auto ny_child {(ny + num_row - 1 - y) / num_row};
      exp_x_pos.push_back(child_x_pos);
      exp_y_pos.push_back(child_y_pos);
      child_y_pos += ny_child * delta_R;
    }
    child_x_pos += nx_child * delta_R;
  }

  // Calculate positions for the remainder nodes
  child_x_pos = x_pos + delta_R * nx_same;
  for (gsl::index x = 0; x < remainder; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + remainder - x) / remainder};
    for (gsl::index y = 0; y < num_row + 1; ++y) {
      auto ny_child {(ny + num_row + 1 - y) / (num_row + 1)};
      exp_x_pos.push_back(child_x_pos);
      exp_y_pos.push_back(child_y_pos);
      child_y_pos += ny_child * delta_R;
    }
    child_x_pos += nx_child * delta_R;
  }

  CHECK_EQUAL(num, cuboid_vec.size());
  for (gsl::index i = 0; i < cuboid_vec.size(); ++i) {
    CHECK_CLOSE(exp_x_pos[i], cuboid_vec[i].GetGlobalXPosition(),
        g_loose_tol);
  }
}

TEST(TestCuboid2D_Resize)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);

  auto x_index {5};
  auto y_index {6};
  nx = 70;
  ny = 80;
  cuboid.Resize(x_index, y_index, nx, ny);

  // Expected values
  auto exp_x_pos {x_pos + delta_R * x_index};
  auto exp_y_pos {y_pos + delta_R * y_index};

  CHECK_CLOSE(exp_x_pos, cuboid.GetGlobalXPosition(), g_loose_tol);
  CHECK_CLOSE(exp_y_pos, cuboid.GetGlobalYPosition(), g_loose_tol);
  CHECK_EQUAL(nx, cuboid.GetNx());
  CHECK_EQUAL(ny, cuboid.GetNy());
}

TEST(TestCuboid2D_GetPhysR_Index)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid(x_pos, y_pos, delta_R, nx, ny);

  for (gsl::index x = 0; x < nx; ++x) {
    for (gsl::index y = 0; y < ny; ++y) {
      auto phys_x {x_pos + delta_R * x};
      auto phys_y {y_pos + delta_R * y};
      CHECK_CLOSE(phys_x, cuboid.GetPhysR(x, y)[0], g_loose_tol);
      CHECK_CLOSE(phys_y, cuboid.GetPhysR(x, y)[1], g_loose_tol);
    }
  }
}
}
}  // namespace iblbm
