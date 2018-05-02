#include <cmath>
#include <iostream>
#include <limits>

#include "UnitTest++/UnitTest++.h"
#include "Cuboid2D.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
struct CuboidVolume
{
  CuboidVolume(
      double x0
    , double y0
    , double x1
    , double y1)
    : mX0{x0},
      mY0{y0},
      mX1{x1},
      mY1{y1}
  {}

  double mX0;
  double mY0;
  double mX1;
  double mY1;
};

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

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

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
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  auto num_col {5u};
  auto num_row {12u};
  std::vector<Cuboid2D<double>> cuboid_vec;
  cuboid.Divide(num_col, num_row, cuboid_vec);

  std::vector<double> exp_x_pos;
  std::vector<double> exp_y_pos;
  auto child_x_pos {x_pos};
  for (gsl::index x {0}; x < num_col; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + num_col - 1 - x) / num_col};
    for (gsl::index y {0}; y < num_row; ++y) {
      auto ny_child {(ny + num_row - 1 - y) / num_row};
      exp_x_pos.push_back(child_x_pos);
      exp_y_pos.push_back(child_y_pos);
      child_y_pos += ny_child * delta_R;
    }
    child_x_pos += nx_child * delta_R;
  }

  CHECK_EQUAL(num_col * num_row, cuboid_vec.size());
  for (gsl::index i {0}; i < cuboid_vec.size(); ++i) {
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
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  auto num_col {7u};
  auto num_row {11u};
  std::vector<Cuboid2D<double>> cuboid_vec;
  cuboid.Divide(num_col, num_row, cuboid_vec);

  std::vector<double> exp_x_pos;
  std::vector<double> exp_y_pos;
  auto child_x_pos {x_pos};
  for (gsl::index x {0}; x < num_col; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + num_col - 1 - x) / num_col};
    for (gsl::index y {0}; y < num_row; ++y) {
      auto ny_child {(ny + num_row - 1 - y) / num_row};
      exp_x_pos.push_back(child_x_pos);
      exp_y_pos.push_back(child_y_pos);
      child_y_pos += ny_child * delta_R;
    }
    child_x_pos += nx_child * delta_R;
  }

  CHECK_EQUAL(num_col * num_row, cuboid_vec.size());
  for (gsl::index i {0}; i < cuboid_vec.size(); ++i) {
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
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  auto num {8};
  std::vector<Cuboid2D<double>> cuboid_vec;
  cuboid.Divide(num, cuboid_vec);

  std::size_t num_col {0};
  std::size_t num_row {0};

  // Determine the best layout for child cuboids
  const auto best_ratio {static_cast<double>(nx) / static_cast<double>(ny)};
  auto best_diff {std::numeric_limits<double>::max()};
  for (gsl::index col {1}; col <= num; ++col) {
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
  for (gsl::index x {0}; x < num_col; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + num_col - 1 - x) / num_col};
    for (gsl::index y {0}; y < num_row; ++y) {
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
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  auto num {15};
  std::vector<Cuboid2D<double>> cuboid_vec;
  cuboid.Divide(num, cuboid_vec);

  std::size_t num_col {0};
  std::size_t num_row {0};

  // Determine the best layout for child cuboids
  const auto best_ratio {static_cast<double>(nx) / static_cast<double>(ny)};
  auto best_diff {std::numeric_limits<double>::max()};
  for (gsl::index col {1}; col <= num; ++col) {
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
  for (gsl::index x {0}; x < num_col; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + num_col - 1 - x) / num_col};
    for (gsl::index y {0}; y < num_row - remainder; ++y) {
      auto ny_child {(ny + num_row - remainder - y) / (num_row - remainder)};
      exp_x_pos.push_back(child_x_pos);
      exp_y_pos.push_back(child_y_pos);
      child_y_pos += ny_child * delta_R;
    }
    child_x_pos += nx_child * delta_R;
  }

  // Calculate positions for the remainder nodes
  child_x_pos = x_pos;
  for (gsl::index x {0}; x < num_col + 1; ++x) {
    auto child_y_pos {y_pos + delta_R * ny_same};
    auto nx_child {(nx + num_col - x) / (num_col + 1)};
    for (gsl::index y {0}; y < remainder; ++y) {
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
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  auto num {13};
  std::vector<Cuboid2D<double>> cuboid_vec;
  cuboid.Divide(num, cuboid_vec);

  std::size_t num_col {0};
  std::size_t num_row {0};

  // Determine the best layout for child cuboids
  const auto best_ratio {static_cast<double>(nx) / static_cast<double>(ny)};
  auto best_diff {std::numeric_limits<double>::max()};
  for (gsl::index col {1}; col <= num; ++col) {
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
  for (gsl::index x {0}; x < num_col - remainder; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + num_col - remainder - x) / (num_col - remainder)};
    for (gsl::index y {0}; y < num_row; ++y) {
      auto ny_child {(ny + num_row - 1 - y) / num_row};
      exp_x_pos.push_back(child_x_pos);
      exp_y_pos.push_back(child_y_pos);
      child_y_pos += ny_child * delta_R;
    }
    child_x_pos += nx_child * delta_R;
  }

  // Calculate positions for the remainder nodes
  child_x_pos = x_pos + delta_R * nx_same;
  for (gsl::index x {0}; x < remainder; ++x) {
    auto child_y_pos {y_pos};
    auto nx_child {(nx + remainder - x) / remainder};
    for (gsl::index y {0}; y < num_row + 1; ++y) {
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
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

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

TEST(TestCuboid2D_ContainPoint_Default)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  for (gsl::index x {-1}; x < nx + 1; ++x) {
    for (gsl::index y {-1}; y < ny + 1; ++y) {
      auto phys_x {x_pos + delta_R * x};
      auto phys_y {y_pos + delta_R * y};
      CHECK(cuboid.ContainPoint(phys_x, phys_y) !=
          (x < 0 || x > nx - 1 || y < 0 || y > ny - 1));
    }
  }
}

TEST(TestCuboid2D_ContainPoint_UserDefined)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  auto overlap {1};
  for (gsl::index x {-2}; x < nx + 2; ++x) {
    for (gsl::index y {-2}; y < ny + 2; ++y) {
      auto phys_x {x_pos + delta_R * x};
      auto phys_y {y_pos + delta_R * y};
      CHECK(cuboid.ContainPoint(phys_x, phys_y, overlap) !=
          (x < 0 - overlap || x > nx - 1 + overlap ||
           y < 0 - overlap || y > ny - 1 + overlap));
    }
  }
}

TEST(TestCuboid2D_ContainPoint_WriteIndex_Default)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  for (gsl::index x {-1}; x < nx + 1; ++x) {
    for (gsl::index y {-1}; y < ny + 1; ++y) {
      auto phys_x {x_pos + delta_R * x};
      auto phys_y {y_pos + delta_R * y};
      gsl::index x_index {0};
      gsl::index y_index {0};
      CHECK(cuboid.ContainPoint(phys_x, phys_y, x_index, y_index) !=
          (x < 0 || x > nx - 1 || y < 0 || y > ny - 1));
      if (!(x < 0 || x > nx - 1 || y < 0 || y > ny - 1)) {
        CHECK_EQUAL(x, x_index);
        CHECK_EQUAL(y, y_index);
      }
    }
  }
}

TEST(TestCuboid2D_ContainPoint_WriteIndex_UserDefined)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  auto overlap {1};
  for (gsl::index x {-1}; x < nx + 1; ++x) {
    for (gsl::index y {-1}; y < ny + 1; ++y) {
      auto phys_x {x_pos + delta_R * x};
      auto phys_y {y_pos + delta_R * y};
      gsl::index x_index {0};
      gsl::index y_index {0};
      CHECK(cuboid.ContainPoint(phys_x, phys_y, x_index, y_index, overlap) !=
          (x < 0 - overlap || x > nx - 1 + overlap ||
           y < 0 - overlap || y > ny - 1 + overlap));
      if (!(x < 0 - overlap || x > nx - 1 + overlap ||
           y < 0 - overlap || y > ny - 1 + overlap)) {
        CHECK_EQUAL(x, x_index - overlap);
        CHECK_EQUAL(y, y_index - overlap);
      }
    }
  }
}

TEST(TestCuboid2D_CheckIntersection_Default)
{
  // This creates a cuboid with the volume (1.2, 3.4) and (3.2, 9.4)
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  // Test against cuboid volume at (1.4, 3.6) and (3.0, 9.2), i.e., contained
  // within current cuboid
  CuboidVolume inside {1.4, 3.6, 3.0, 9.2};
  CHECK(cuboid.CheckIntersection(inside.mX0, inside.mY0, inside.mX1,
      inside.mY1));

  // Test against cuboid volume at (1.0, 3.6) and (4.0, 9.8), i.e., contained
  // within current cuboid
  CuboidVolume outside {1.0, 3.0, 4.0, 9.8};
  CHECK(cuboid.CheckIntersection(outside.mX0, outside.mY0, outside.mX1,
      outside.mY1));

  // Intersect
  {
    // Test against cuboid volume at (1.0, 3.0) and (3.0, 9.2), i.e.,
    // intersect the lower left area of the current cuboid
    CuboidVolume lower_left {1.0, 3.0, 3.0, 9.2};
    CHECK(cuboid.CheckIntersection(lower_left.mX0, lower_left.mY0,
        lower_left.mX1, lower_left.mY1));

    // Test against cuboid volume at (1.0, 9.0) and (3.0, 9.8), i.e.,
    // intersect the upper left area of the current cuboid
    CuboidVolume upper_left {1.0, 9.0, 3.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_left.mX0, upper_left.mY0,
        upper_left.mX1, upper_left.mY1));

    // Test against cuboid volume at (3.0, 9.0) and (5.0, 9.8), i.e.,
    // intersect the upper right area of the current cuboid
    CuboidVolume upper_right {3.0, 9.0, 5.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_right.mX0, upper_right.mY0,
        upper_right.mX1, upper_right.mY1));

    // Test against cuboid volume at (3.0, 1.0) and (5.0, 8.8), i.e.,
    // intersect the upper right area of the current cuboid
    CuboidVolume lower_right {3.0, 1.0, 5.0, 8.8};
    CHECK(cuboid.CheckIntersection(lower_right.mX0, lower_right.mY0,
        lower_right.mX1, lower_right.mY1));
  }

  // Does not intersect, tests cuboid volumes which are 1 * delta_R off so we
  // can test that they intersect later when overlap = 2
  {
    // off by 1 delta_R in the x-direction
    CuboidVolume lower_left_x {0.5, 1.0, 1.1, 9.0};
    CHECK(!cuboid.CheckIntersection(lower_left_x.mX0, lower_left_x.mY0,
        lower_left_x.mX1, lower_left_x.mY1));

    // off by 1 delta_R in the y-direction
    CuboidVolume lower_left_y {1.0, 1.0, 3.0, 3.3};
    CHECK(!cuboid.CheckIntersection(lower_left_y.mX0, lower_left_y.mY0,
        lower_left_y.mX1, lower_left_y.mY1));

    CuboidVolume upper_left_x {0.5, 9.0, 1.1, 9.8};
    CHECK(!cuboid.CheckIntersection(upper_left_x.mX0, upper_left_x.mY0,
        upper_left_x.mX1, upper_left_x.mY1));

    CuboidVolume upper_left_y {1.0, 9.5, 3.0, 9.8};
    CHECK(!cuboid.CheckIntersection(upper_left_y.mX0, upper_left_y.mY0,
        upper_left_y.mX1, upper_left_y.mY1));

    CuboidVolume upper_right_x {3.3, 9.0, 5.0, 9.8};
    CHECK(!cuboid.CheckIntersection(upper_right_x.mX0, upper_right_x.mY0,
        upper_right_x.mX1, upper_right_x.mY1));

    CuboidVolume upper_right_y {3.0, 9.5, 5.0, 9.8};
    CHECK(!cuboid.CheckIntersection(upper_right_y.mX0, upper_right_y.mY0,
        upper_right_y.mX1, upper_right_y.mY1));

    CuboidVolume lower_right_x {3.3, 1.0, 5.0, 8.8};
    CHECK(!cuboid.CheckIntersection(lower_right_x.mX0, lower_right_x.mY0,
        lower_right_x.mX1, lower_right_x.mY1));

    CuboidVolume lower_right_y {3.0, 1.0, 5.0, 3.3};
    CHECK(!cuboid.CheckIntersection(lower_right_y.mX0, lower_right_y.mY0,
        lower_right_y.mX1, lower_right_y.mY1));
  }
}

TEST(TestCuboid2D_CheckIntersection_UserDefined)
{
  // This creates a cuboid with the volume (1.2, 3.4) and (3.2, 9.4)
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  auto overlap {2};
  // Test against cuboid volume at (1.4, 3.6) and (3.0, 9.2), i.e., contained
  // within current cuboid
  CuboidVolume inside {1.4, 3.6, 3.0, 9.2};
  CHECK(cuboid.CheckIntersection(inside.mX0, inside.mY0, inside.mX1,
      inside.mY1, overlap));

  // Test against cuboid volume at (1.0, 3.6) and (4.0, 9.8), i.e., contained
  // within current cuboid
  CuboidVolume outside {1.0, 3.0, 4.0, 9.8};
  CHECK(cuboid.CheckIntersection(outside.mX0, outside.mY0, outside.mX1,
      outside.mY1, overlap));

  // Intersect
  {
    // Test against cuboid volume at (1.0, 3.0) and (3.0, 9.2), i.e.,
    // intersect the lower left area of the current cuboid
    CuboidVolume lower_left {1.0, 3.0, 3.0, 9.2};
    CHECK(cuboid.CheckIntersection(lower_left.mX0, lower_left.mY0,
        lower_left.mX1, lower_left.mY1, overlap));

    // Test against cuboid volume at (1.0, 9.0) and (3.0, 9.8), i.e.,
    // intersect the upper left area of the current cuboid
    CuboidVolume upper_left {1.0, 9.0, 3.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_left.mX0, upper_left.mY0,
        upper_left.mX1, upper_left.mY1, overlap));

    // Test against cuboid volume at (3.0, 9.0) and (5.0, 9.8), i.e.,
    // intersect the upper right area of the current cuboid
    CuboidVolume upper_right {3.0, 9.0, 5.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_right.mX0, upper_right.mY0,
        upper_right.mX1, upper_right.mY1, overlap));

    // Test against cuboid volume at (3.0, 1.0) and (5.0, 8.8), i.e.,
    // intersect the upper right area of the current cuboid
    CuboidVolume lower_right {3.0, 1.0, 5.0, 8.8};
    CHECK(cuboid.CheckIntersection(lower_right.mX0, lower_right.mY0,
        lower_right.mX1, lower_right.mY1, overlap));
  }

  // Did not intersect when overlap = 0, intersects now
  {
    // off by 1 delta_R in the x-direction
    CuboidVolume lower_left_x {0.5, 1.0, 1.1, 9.0};
    CHECK(cuboid.CheckIntersection(lower_left_x.mX0, lower_left_x.mY0,
        lower_left_x.mX1, lower_left_x.mY1, overlap));

    // off by 1 delta_R in the y-direction
    CuboidVolume lower_left_y {1.0, 1.0, 3.0, 3.3};
    CHECK(cuboid.CheckIntersection(lower_left_y.mX0, lower_left_y.mY0,
        lower_left_y.mX1, lower_left_y.mY1, overlap));

    CuboidVolume upper_left_x {0.5, 9.0, 1.1, 9.8};
    CHECK(cuboid.CheckIntersection(upper_left_x.mX0, upper_left_x.mY0,
        upper_left_x.mX1, upper_left_x.mY1, overlap));

    CuboidVolume upper_left_y {1.0, 9.5, 3.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_left_y.mX0, upper_left_y.mY0,
        upper_left_y.mX1, upper_left_y.mY1, overlap));

    CuboidVolume upper_right_x {3.3, 9.0, 5.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_right_x.mX0, upper_right_x.mY0,
        upper_right_x.mX1, upper_right_x.mY1, overlap));

    CuboidVolume upper_right_y {3.0, 9.5, 5.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_right_y.mX0, upper_right_y.mY0,
        upper_right_y.mX1, upper_right_y.mY1, overlap));

    CuboidVolume lower_right_x {3.3, 1.0, 5.0, 8.8};
    CHECK(cuboid.CheckIntersection(lower_right_x.mX0, lower_right_x.mY0,
        lower_right_x.mX1, lower_right_x.mY1, overlap));

    CuboidVolume lower_right_y {3.0, 1.0, 5.0, 3.3};
    CHECK(cuboid.CheckIntersection(lower_right_y.mX0, lower_right_y.mY0,
        lower_right_y.mX1, lower_right_y.mY1, overlap));
  }

  // Does not intersect
  {
    CuboidVolume lower_left {0.5, 1.0, 3.0, 2.5};
    CHECK(!cuboid.CheckIntersection(lower_left.mX0, lower_left.mY0,
        lower_left.mX1, lower_left.mY1, overlap));

    CuboidVolume upper_left {0.5, 9.7, 1.4, 10.0};
    CHECK(!cuboid.CheckIntersection(upper_left.mX0, upper_left.mY0,
        upper_left.mX1, upper_left.mY1, overlap));

    CuboidVolume upper_right {3.7, 9.0, 5.0, 9.8};
    CHECK(!cuboid.CheckIntersection(upper_right.mX0, upper_right.mY0,
        upper_right.mX1, upper_right.mY1, overlap));

    CuboidVolume lower_right {3.7, 1.0, 5.0, 8.8};
    CHECK(!cuboid.CheckIntersection(lower_right.mX0, lower_right.mY0,
        lower_right.mX1, lower_right.mY1, overlap));
  }
}

TEST(TestCuboid2D_CheckIntersection_Point_Default)
{
  // This creates a cuboid with the volume (1.2, 3.4) and (3.2, 9.4)
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  Vector2D<double> inside {1.4, 3.6};
  CHECK(cuboid.CheckIntersection(inside[0], inside[1]));

  // outside of current cuboid by 1 delta_R at the left edge
  Vector2D<double> left {1.1, 5.0};
  CHECK(!cuboid.CheckIntersection(left[0], left[1]));

  Vector2D<double> upper {1.6, 9.5};
  CHECK(!cuboid.CheckIntersection(upper[0], upper[1]));

  Vector2D<double> right {3.3, 5.0};
  CHECK(!cuboid.CheckIntersection(right[0], right[1]));

  Vector2D<double> lower {1.6, 3.3};
  CHECK(!cuboid.CheckIntersection(lower[0], lower[1]));

  for (gsl::index x {-10}; x < nx + 10; ++x) {
    for (gsl::index y {-10}; y < ny + 10; ++y) {
      auto x_pos_0 {x_pos + static_cast<double>(x) * delta_R};
      auto y_pos_0 {y_pos + static_cast<double>(y) * delta_R};
      CHECK(cuboid.CheckIntersection(x_pos_0, y_pos_0) ==
            cuboid.ContainPoint(x_pos_0, y_pos_0));
    }
  }
}

TEST(TestCuboid2D_CheckIntersection_Point_UserDefined)
{
  // This creates a cuboid with the volume (1.2, 3.4) and (3.2, 9.4)
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  auto overlap {2};
  Vector2D<double> inside {1.4, 3.6};
  CHECK(cuboid.CheckIntersection(inside[0], inside[1], overlap));

  // Intersect
  {
    // outside of current cuboid by 1 delta_R at the left edge
    Vector2D<double> left {1.1, 5.0};
    CHECK(cuboid.CheckIntersection(left[0], left[1], overlap));

    Vector2D<double> upper {1.6, 9.5};
    CHECK(cuboid.CheckIntersection(upper[0], upper[1], overlap));

    Vector2D<double> right {3.3, 5.0};
    CHECK(cuboid.CheckIntersection(right[0], right[1], overlap));

    Vector2D<double> lower {1.6, 3.3};
    CHECK(cuboid.CheckIntersection(lower[0], lower[1], overlap));
  }

  // Intersect
  {
    // outside of current cuboid by 1 delta_R at the left edge
    Vector2D<double> left {0.9, 5.0};
    CHECK(!cuboid.CheckIntersection(left[0], left[1], overlap));

    Vector2D<double> upper {1.6, 9.8};
    CHECK(!cuboid.CheckIntersection(upper[0], upper[1], overlap));

    Vector2D<double> right {3.5, 5.0};
    CHECK(!cuboid.CheckIntersection(right[0], right[1], overlap));

    Vector2D<double> lower {1.6, 3.1};
    CHECK(!cuboid.CheckIntersection(lower[0], lower[1], overlap));
  }

  for (gsl::index x {-10}; x < nx + 10; ++x) {
    for (gsl::index y {-10}; y < ny + 10; ++y) {
      auto x_pos_0 {x_pos + static_cast<double>(x) * delta_R};
      auto y_pos_0 {y_pos + static_cast<double>(y) * delta_R};
      CHECK(cuboid.CheckIntersection(x_pos_0, y_pos_0, overlap) ==
            cuboid.ContainPoint(x_pos_0, y_pos_0, overlap));
    }
  }
}

TEST(TestCuboid2D_CheckIntersection_WriteIndex_Default)
{
  // This creates a cuboid with the volume (1.2, 3.4) and (3.2, 9.4)
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  // Test against cuboid volume at (1.4, 3.6) and (3.0, 9.2), i.e., contained
  // within current cuboid
  gsl::index x_idx_0 {0};
  gsl::index y_idx_0 {0};
  gsl::index x_idx_1 {0};
  gsl::index y_idx_1 {0};
  for (gsl::index x {1}; x < nx / 2; ++x) {
    for (gsl::index y {1}; y < ny / 2; ++y) {
      auto x_pos_0 {x_pos + static_cast<double>(x) * delta_R};
      auto y_pos_0 {y_pos + static_cast<double>(y) * delta_R};
      auto x_pos_1 {x_pos_0 + static_cast<double>(nx / 4) * delta_R};
      auto y_pos_1 {y_pos_0 + static_cast<double>(ny / 4) * delta_R};

      CHECK(cuboid.CheckIntersection(x_pos_0, y_pos_0, x_pos_1, y_pos_1,
          x_idx_0, y_idx_0, x_idx_1, y_idx_1));
      CHECK_EQUAL(x, x_idx_0);
      CHECK_EQUAL(y, y_idx_0);
      CHECK_EQUAL(x + nx / 4, x_idx_1);
      CHECK_EQUAL(y + ny / 4, y_idx_1);
    }
  }

  // Test against cuboid volume at (1.0, 3.6) and (4.0, 9.8), i.e., contained
  // within current cuboid
  CuboidVolume outside {1.0, 3.0, 4.0, 9.8};
  CHECK(cuboid.CheckIntersection(outside.mX0, outside.mY0, outside.mX1,
      outside.mY1, x_idx_0, y_idx_0, x_idx_1, y_idx_1));
  CHECK_EQUAL(0, x_idx_0);
  CHECK_EQUAL(0, y_idx_0);
  CHECK_EQUAL(nx - 1, x_idx_1);
  CHECK_EQUAL(ny - 1, y_idx_1);

  // Intersect
  {
    // Test against cuboid volume at (1.0, 3.0) and (3.0, 9.2), i.e.,
    // intersect the lower left area of the current cuboid
    CuboidVolume lower_left {1.0, 3.0, 3.0, 9.2};
    CHECK(cuboid.CheckIntersection(lower_left.mX0, lower_left.mY0,
        lower_left.mX1, lower_left.mY1, x_idx_0, y_idx_0, x_idx_1, y_idx_1));

    CHECK_EQUAL(0, x_idx_0);
    CHECK_EQUAL(0, y_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((lower_left.mX1 - x_pos) / delta_R +
        0.5), x_idx_1);
    CHECK_EQUAL(static_cast<gsl::index>((lower_left.mY1 - y_pos) / delta_R +
        0.5), y_idx_1);

    // Test against cuboid volume at (1.0, 9.0) and (3.0, 9.8), i.e.,
    // intersect the upper left area of the current cuboid
    CuboidVolume upper_left {1.0, 9.0, 3.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_left.mX0, upper_left.mY0,
        upper_left.mX1, upper_left.mY1, x_idx_0, y_idx_0, x_idx_1, y_idx_1));
    CHECK_EQUAL(0, x_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_left.mY0 - y_pos) / delta_R +
        0.5), y_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_left.mX1 - x_pos) / delta_R),
        x_idx_1);
    CHECK_EQUAL(ny - 1, y_idx_1);

    // Test against cuboid volume at (3.0, 9.0) and (5.0, 9.8), i.e.,
    // intersect the upper right area of the current cuboid
    CuboidVolume upper_right {3.0, 9.0, 5.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_right.mX0, upper_right.mY0,
        upper_right.mX1, upper_right.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1));
    CHECK_EQUAL(static_cast<gsl::index>((upper_right.mX0 - x_pos) /
        delta_R + 0.5), x_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_right.mY0 - y_pos) /
        delta_R + 0.5), y_idx_0);
    CHECK_EQUAL(nx - 1, x_idx_1);
    CHECK_EQUAL(ny - 1, y_idx_1);

    // Test against cuboid volume at (3.0, 1.0) and (5.0, 8.8), i.e.,
    // intersect the upper right area of the current cuboid
    CuboidVolume lower_right {3.0, 1.0, 5.0, 8.8};
    CHECK(cuboid.CheckIntersection(lower_right.mX0, lower_right.mY0,
        lower_right.mX1, lower_right.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1));
    CHECK_EQUAL(static_cast<gsl::index>((lower_right.mX0 - x_pos) /
        delta_R + 0.5), x_idx_0);
    CHECK_EQUAL(0, y_idx_0);
    CHECK_EQUAL(nx - 1, x_idx_1);
    CHECK_EQUAL(static_cast<gsl::index>((lower_right.mY1 - y_pos) /
        delta_R + 0.5), y_idx_1);
  }

  // Does not intersect, tests cuboid volumes which are 1 * delta_R off so we
  // can test that they intersect later when overlap = 2
  {
    // off by 1 delta_R in the x-direction
    CuboidVolume lower_left_x {0.5, 1.0, 1.1, 9.0};
    CHECK(!cuboid.CheckIntersection(lower_left_x.mX0, lower_left_x.mY0,
        lower_left_x.mX1, lower_left_x.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1));
    CHECK_EQUAL(1, x_idx_0);
    CHECK_EQUAL(1, y_idx_0);
    CHECK_EQUAL(0, x_idx_1);
    CHECK_EQUAL(0, y_idx_1);

    // off by 1 delta_R in the y-direction
    CuboidVolume lower_left_y {1.0, 1.0, 3.0, 3.3};
    CHECK(!cuboid.CheckIntersection(lower_left_y.mX0, lower_left_y.mY0,
        lower_left_y.mX1, lower_left_y.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1));
    CHECK_EQUAL(1, x_idx_0);
    CHECK_EQUAL(1, y_idx_0);
    CHECK_EQUAL(0, x_idx_1);
    CHECK_EQUAL(0, y_idx_1);

    CuboidVolume upper_left_x {0.5, 9.0, 1.1, 9.8};
    CHECK(!cuboid.CheckIntersection(upper_left_x.mX0, upper_left_x.mY0,
        upper_left_x.mX1, upper_left_x.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1));
    CHECK_EQUAL(1, x_idx_0);
    CHECK_EQUAL(1, y_idx_0);
    CHECK_EQUAL(0, x_idx_1);
    CHECK_EQUAL(0, y_idx_1);

    CuboidVolume upper_left_y {1.0, 9.5, 3.0, 9.8};
    CHECK(!cuboid.CheckIntersection(upper_left_y.mX0, upper_left_y.mY0,
        upper_left_y.mX1, upper_left_y.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1));
    CHECK_EQUAL(1, x_idx_0);
    CHECK_EQUAL(1, y_idx_0);
    CHECK_EQUAL(0, x_idx_1);
    CHECK_EQUAL(0, y_idx_1);

    CuboidVolume upper_right_x {3.3, 9.0, 5.0, 9.8};
    CHECK(!cuboid.CheckIntersection(upper_right_x.mX0, upper_right_x.mY0,
        upper_right_x.mX1, upper_right_x.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1));
    CHECK_EQUAL(1, x_idx_0);
    CHECK_EQUAL(1, y_idx_0);
    CHECK_EQUAL(0, x_idx_1);
    CHECK_EQUAL(0, y_idx_1);

    CuboidVolume upper_right_y {3.0, 9.5, 5.0, 9.8};
    CHECK(!cuboid.CheckIntersection(upper_right_y.mX0, upper_right_y.mY0,
        upper_right_y.mX1, upper_right_y.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1));
    CHECK_EQUAL(1, x_idx_0);
    CHECK_EQUAL(1, y_idx_0);
    CHECK_EQUAL(0, x_idx_1);
    CHECK_EQUAL(0, y_idx_1);

    CuboidVolume lower_right_x {3.3, 1.0, 5.0, 8.8};
    CHECK(!cuboid.CheckIntersection(lower_right_x.mX0, lower_right_x.mY0,
        lower_right_x.mX1, lower_right_x.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1));
    CHECK_EQUAL(1, x_idx_0);
    CHECK_EQUAL(1, y_idx_0);
    CHECK_EQUAL(0, x_idx_1);
    CHECK_EQUAL(0, y_idx_1);

    CuboidVolume lower_right_y {3.0, 1.0, 5.0, 3.3};
    CHECK(!cuboid.CheckIntersection(lower_right_y.mX0, lower_right_y.mY0,
        lower_right_y.mX1, lower_right_y.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1));
    CHECK_EQUAL(1, x_idx_0);
    CHECK_EQUAL(1, y_idx_0);
    CHECK_EQUAL(0, x_idx_1);
    CHECK_EQUAL(0, y_idx_1);
  }
}

TEST(TestCuboid2D_CheckIntersection_WriteIndex_UserDefined)
{
  // This creates a cuboid with the volume (1.2, 3.4) and (3.2, 9.4)
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  auto overlap {2};
  // Test against cuboid volume at (1.4, 3.6) and (3.0, 9.2), i.e., contained
  // within current cuboid
  gsl::index x_idx_0 {0};
  gsl::index y_idx_0 {0};
  gsl::index x_idx_1 {0};
  gsl::index y_idx_1 {0};
  for (gsl::index x {1}; x < nx / 2; ++x) {
    for (gsl::index y {1}; y < ny / 2; ++y) {
      auto x_pos_0 {x_pos + static_cast<double>(x) * delta_R};
      auto y_pos_0 {y_pos + static_cast<double>(y) * delta_R};
      auto x_pos_1 {x_pos_0 + static_cast<double>(nx / 4) * delta_R};
      auto y_pos_1 {y_pos_0 + static_cast<double>(ny / 4) * delta_R};

      CHECK(cuboid.CheckIntersection(x_pos_0, y_pos_0, x_pos_1, y_pos_1,
          x_idx_0, y_idx_0, x_idx_1, y_idx_1, overlap));
      CHECK_EQUAL(x + overlap, x_idx_0);
      CHECK_EQUAL(y + overlap, y_idx_0);
      CHECK_EQUAL(x + overlap + nx / 4, x_idx_1);
      CHECK_EQUAL(y + overlap + ny / 4, y_idx_1);
    }
  }

  // Test against cuboid volume at (1.0, 3.6) and (4.0, 9.8), i.e., contained
  // within current cuboid
  CuboidVolume outside {1.0, 3.0, 4.0, 9.8};
  CHECK(cuboid.CheckIntersection(outside.mX0, outside.mY0, outside.mX1,
      outside.mY1, x_idx_0, y_idx_0, x_idx_1, y_idx_1, overlap));
  CHECK_EQUAL(0, x_idx_0);
  CHECK_EQUAL(0, y_idx_0);
  CHECK_EQUAL(nx - 1 + 2 * overlap, x_idx_1);
  CHECK_EQUAL(ny - 1 + 2 * overlap, y_idx_1);

  // Intersect
  {
    // Test against cuboid volume at (1.0, 3.0) and (3.0, 9.2), i.e.,
    // intersect the lower left area of the current cuboid
    CuboidVolume lower_left {1.0, 3.0, 3.0, 9.2};
    CHECK(cuboid.CheckIntersection(lower_left.mX0, lower_left.mY0,
        lower_left.mX1, lower_left.mY1, x_idx_0, y_idx_0, x_idx_1, y_idx_1,
        overlap));

    CHECK_EQUAL(0, x_idx_0);
    CHECK_EQUAL(0, y_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((lower_left.mX1 - x_pos) / delta_R +
        overlap + 0.5), x_idx_1);
    CHECK_EQUAL(static_cast<gsl::index>((lower_left.mY1 - y_pos) / delta_R +
        overlap + 0.5), y_idx_1);

    // Test against cuboid volume at (1.0, 9.0) and (3.0, 9.8), i.e.,
    // intersect the upper left area of the current cuboid
    CuboidVolume upper_left {1.0, 9.0, 3.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_left.mX0, upper_left.mY0,
        upper_left.mX1, upper_left.mY1, x_idx_0, y_idx_0, x_idx_1, y_idx_1,
        overlap));
    CHECK_EQUAL(0, x_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_left.mY0 - y_pos) / delta_R +
        overlap + 0.5), y_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_left.mX1 - x_pos) / delta_R +
        overlap + 0.5), x_idx_1);
    CHECK_EQUAL(ny - 1 + 2 * overlap, y_idx_1);

    // Test against cuboid volume at (3.0, 9.0) and (5.0, 9.8), i.e.,
    // intersect the upper right area of the current cuboid
    CuboidVolume upper_right {3.0, 9.0, 5.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_right.mX0, upper_right.mY0,
        upper_right.mX1, upper_right.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1, overlap));
    CHECK_EQUAL(static_cast<gsl::index>((upper_right.mX0 - x_pos) /
        delta_R + overlap + 0.5), x_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_right.mY0 - y_pos) /
        delta_R + overlap +  0.5), y_idx_0);
    CHECK_EQUAL(nx - 1 + 2 * overlap, x_idx_1);
    CHECK_EQUAL(ny - 1 + 2 * overlap, y_idx_1);

    // Test against cuboid volume at (3.0, 1.0) and (5.0, 8.8), i.e.,
    // intersect the upper right area of the current cuboid
    CuboidVolume lower_right {3.0, 1.0, 5.0, 8.8};
    CHECK(cuboid.CheckIntersection(lower_right.mX0, lower_right.mY0,
        lower_right.mX1, lower_right.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1, overlap));
    CHECK_EQUAL(static_cast<gsl::index>((lower_right.mX0 - x_pos) /
        delta_R + overlap + 0.5), x_idx_0);
    CHECK_EQUAL(0, y_idx_0);
    CHECK_EQUAL(nx - 1 + 2 * overlap, x_idx_1);
    CHECK_EQUAL(static_cast<gsl::index>((lower_right.mY1 - y_pos) /
        delta_R + overlap + 0.5), y_idx_1);
  }

  // Does not intersect, tests cuboid volumes which are 1 * delta_R off so we
  // can test that they intersect later when overlap = 2
  {
    // off by 1 delta_R in the x-direction
    CuboidVolume lower_left_x {0.5, 1.0, 1.1, 9.0};
    CHECK(cuboid.CheckIntersection(lower_left_x.mX0, lower_left_x.mY0,
        lower_left_x.mX1, lower_left_x.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1, overlap));
    CHECK_EQUAL(0, x_idx_0);
    CHECK_EQUAL(0, y_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((lower_left_x.mX1 - x_pos) /
        delta_R + overlap + 0.5), x_idx_1);
    CHECK_EQUAL(static_cast<gsl::index>((lower_left_x.mY1 - y_pos) /
        delta_R + overlap + 0.5), y_idx_1);

    // off by 1 delta_R in the y-direction
    CuboidVolume lower_left_y {1.0, 1.0, 3.0, 3.3};
    CHECK(cuboid.CheckIntersection(lower_left_y.mX0, lower_left_y.mY0,
        lower_left_y.mX1, lower_left_y.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1, overlap));
    CHECK_EQUAL(0, x_idx_0);
    CHECK_EQUAL(0, y_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((lower_left_y.mX1 - x_pos) /
        delta_R + overlap + 0.5), x_idx_1);
    CHECK_EQUAL(static_cast<gsl::index>((lower_left_y.mY1 - y_pos) /
        delta_R + overlap + 0.5), y_idx_1);

    CuboidVolume upper_left_x {0.5, 9.0, 1.1, 9.8};
    CHECK(cuboid.CheckIntersection(upper_left_x.mX0, upper_left_x.mY0,
        upper_left_x.mX1, upper_left_x.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1, overlap));
    CHECK_EQUAL(0, x_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_left_x.mY0 - y_pos) / delta_R +
        overlap + 0.5), y_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_left_x.mX1 - x_pos) / delta_R +
        overlap + 0.5), x_idx_1);
    CHECK_EQUAL(ny - 1 + 2 * overlap, y_idx_1);

    CuboidVolume upper_left_y {1.0, 9.5, 3.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_left_y.mX0, upper_left_y.mY0,
        upper_left_y.mX1, upper_left_y.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1, overlap));
    CHECK_EQUAL(0, x_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_left_y.mY0 - y_pos) / delta_R +
        overlap + 0.5), y_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_left_y.mX1 - x_pos) / delta_R +
        overlap + 0.5), x_idx_1);
    CHECK_EQUAL(ny - 1 + 2 * overlap, y_idx_1);

    CuboidVolume upper_right_x {3.3, 9.0, 5.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_right_x.mX0, upper_right_x.mY0,
        upper_right_x.mX1, upper_right_x.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1, overlap));
    CHECK_EQUAL(static_cast<gsl::index>((upper_right_x.mX0 - x_pos) /
        delta_R + overlap + 0.5), x_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_right_x.mY0 - y_pos) /
        delta_R + overlap +  0.5), y_idx_0);
    CHECK_EQUAL(nx - 1 + 2 * overlap, x_idx_1);
    CHECK_EQUAL(ny - 1 + 2 * overlap, y_idx_1);

    CuboidVolume upper_right_y {3.0, 9.5, 5.0, 9.8};
    CHECK(cuboid.CheckIntersection(upper_right_y.mX0, upper_right_y.mY0,
        upper_right_y.mX1, upper_right_y.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1, overlap));
    CHECK_EQUAL(static_cast<gsl::index>((upper_right_y.mX0 - x_pos) /
        delta_R + overlap + 0.5), x_idx_0);
    CHECK_EQUAL(static_cast<gsl::index>((upper_right_y.mY0 - y_pos) /
        delta_R + overlap +  0.5), y_idx_0);
    CHECK_EQUAL(nx - 1 + 2 * overlap, x_idx_1);
    CHECK_EQUAL(ny - 1 + 2 * overlap, y_idx_1);

    CuboidVolume lower_right_x {3.3, 1.0, 5.0, 8.8};
    CHECK(cuboid.CheckIntersection(lower_right_x.mX0, lower_right_x.mY0,
        lower_right_x.mX1, lower_right_x.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1, overlap));
    CHECK_EQUAL(static_cast<gsl::index>((lower_right_x.mX0 - x_pos) /
        delta_R + overlap + 0.5), x_idx_0);
    CHECK_EQUAL(0, y_idx_0);
    CHECK_EQUAL(nx - 1 + 2 * overlap, x_idx_1);
    CHECK_EQUAL(static_cast<gsl::index>((lower_right_x.mY1 - y_pos) /
        delta_R + overlap + 0.5), y_idx_1);

    CuboidVolume lower_right_y {3.0, 1.0, 5.0, 3.3};
    CHECK(cuboid.CheckIntersection(lower_right_y.mX0, lower_right_y.mY0,
        lower_right_y.mX1, lower_right_y.mY1, x_idx_0, y_idx_0, x_idx_1,
        y_idx_1, overlap));
    CHECK_EQUAL(static_cast<gsl::index>((lower_right_y.mX0 - x_pos) /
        delta_R + overlap + 0.5), x_idx_0);
    CHECK_EQUAL(0, y_idx_0);
    CHECK_EQUAL(nx - 1 + 2 * overlap, x_idx_1);
    CHECK_EQUAL(static_cast<gsl::index>((lower_right_y.mY1 - y_pos) /
        delta_R + overlap + 0.5), y_idx_1);
  }
}

TEST(TestCuboid2D_RefineToLevel)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  auto level {3};
  auto level_diff {static_cast<int>(std::pow(2, level))};
  cuboid.RefineToLevel(level);
  CHECK_CLOSE(delta_R / static_cast<double>(level_diff), cuboid.GetDeltaR(),
      g_loose_tol);
  CHECK_EQUAL(nx * level_diff, cuboid.GetNx());
  CHECK_EQUAL(ny * level_diff, cuboid.GetNy());
  CHECK_EQUAL(level, cuboid.GetRefinementLevel());

  // Check that refining down doesn't work
  cuboid.RefineToLevel(0);
  CHECK_CLOSE(delta_R / static_cast<double>(level_diff), cuboid.GetDeltaR(),
      g_loose_tol);
  CHECK_EQUAL(nx * level_diff, cuboid.GetNx());
  CHECK_EQUAL(ny * level_diff, cuboid.GetNy());
  CHECK_EQUAL(level, cuboid.GetRefinementLevel());
}

TEST(TestCuboid2D_RefineIncrease)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  cuboid.RefineIncrease();
  CHECK_CLOSE(delta_R / 2.0, cuboid.GetDeltaR(), g_loose_tol);
  CHECK_EQUAL(nx * 2, cuboid.GetNx());
  CHECK_EQUAL(ny * 2, cuboid.GetNy());
  CHECK_EQUAL(1, cuboid.GetRefinementLevel());

  cuboid.RefineIncrease();
  CHECK_CLOSE(delta_R / 4.0, cuboid.GetDeltaR(), g_loose_tol);
  CHECK_EQUAL(nx * 4, cuboid.GetNx());
  CHECK_EQUAL(ny * 4, cuboid.GetNy());
  CHECK_EQUAL(2, cuboid.GetRefinementLevel());
}

TEST(TestCuboid2D_RefineDecrease)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  cuboid.RefineToLevel(2);
  cuboid.RefineDecrease();
  CHECK_CLOSE(delta_R / 2.0, cuboid.GetDeltaR(), g_loose_tol);
  CHECK_EQUAL(nx * 2, cuboid.GetNx());
  CHECK_EQUAL(ny * 2, cuboid.GetNy());
  CHECK_EQUAL(1, cuboid.GetRefinementLevel());

  cuboid.RefineDecrease();
  CHECK_CLOSE(delta_R, cuboid.GetDeltaR(), g_loose_tol);
  CHECK_EQUAL(nx, cuboid.GetNx());
  CHECK_EQUAL(ny, cuboid.GetNy());
  CHECK_EQUAL(0, cuboid.GetRefinementLevel());

  // Check that we can't go below refinement 0
  cuboid.RefineDecrease();
  CHECK_CLOSE(delta_R, cuboid.GetDeltaR(), g_loose_tol);
  CHECK_EQUAL(nx, cuboid.GetNx());
  CHECK_EQUAL(ny, cuboid.GetNy());
  CHECK_EQUAL(0, cuboid.GetRefinementLevel());
}

TEST(TestCuboid2D_GetPhysR_Array)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      auto phys_x {x_pos + delta_R * x};
      auto phys_y {y_pos + delta_R * y};

      gsl::index lattice_R[2] {x, y};
      double phys_R[2];
      cuboid.GetPhysR(lattice_R, phys_R);

      CHECK_CLOSE(phys_x, phys_R[0], g_loose_tol);
      CHECK_CLOSE(phys_y, phys_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboid2D_GetPhysR_Index)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      auto phys_x {x_pos + delta_R * x};
      auto phys_y {y_pos + delta_R * y};
      CHECK_CLOSE(phys_x, cuboid.GetPhysR(x, y)[0], g_loose_tol);
      CHECK_CLOSE(phys_y, cuboid.GetPhysR(x, y)[1], g_loose_tol);
    }
  }
}

TEST(TestCuboid2D_GetLatticeR_Array)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      auto phys_x {x_pos + delta_R * x};
      auto phys_y {y_pos + delta_R * y};

      gsl::index lattice_R[2];
      double phys_R[2] {phys_x, phys_y};
      cuboid.GetLatticeR(phys_R, lattice_R);

      CHECK_CLOSE(x, lattice_R[0], g_loose_tol);
      CHECK_CLOSE(y, lattice_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboid2D_GetLatticeR_Vector)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      auto phys_x {x_pos + delta_R * x};
      auto phys_y {y_pos + delta_R * y};

      gsl::index lattice_R[2];
      Vector2D<double> phys_R {phys_x, phys_y};
      cuboid.GetLatticeR(phys_R, lattice_R);

      CHECK_CLOSE(x, lattice_R[0], g_loose_tol);
      CHECK_CLOSE(y, lattice_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboid2D_GetFloorLatticeR_Array)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      auto phys_x {x_pos + delta_R * x};
      auto phys_y {y_pos + delta_R * y};

      gsl::index lattice_R[2];
      double phys_R[2] {phys_x, phys_y};
      cuboid.GetFloorLatticeR(phys_R, lattice_R);
      auto lattice_x {static_cast<gsl::index>(floor((phys_x - x_pos) /
          delta_R))};
      auto lattice_y {static_cast<gsl::index>(floor((phys_y - y_pos) /
          delta_R))};

      CHECK_CLOSE(lattice_x, lattice_R[0], g_loose_tol);
      CHECK_CLOSE(lattice_y, lattice_R[1], g_loose_tol);
    }
  }
}

TEST(TestCuboid2D_GetFloorLatticeR_StdVector)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {20u};
  auto ny {60u};
  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      auto phys_x {x_pos + delta_R * x};
      auto phys_y {y_pos + delta_R * y};

      std::vector<gsl::index> lattice_R(2);
      std::vector<double> phys_R {phys_x, phys_y};
      cuboid.GetFloorLatticeR(phys_R, lattice_R);
      auto lattice_x {static_cast<gsl::index>(floor((phys_x - x_pos) /
          delta_R))};
      auto lattice_y {static_cast<gsl::index>(floor((phys_y - y_pos) /
          delta_R))};

      CHECK_CLOSE(lattice_x, lattice_R[0], g_loose_tol);
      CHECK_CLOSE(lattice_y, lattice_R[1], g_loose_tol);
    }
  }
}
}
}  // namespace iblbm
