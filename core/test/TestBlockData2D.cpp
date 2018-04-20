#include "gsl/gsl"
#include "UnitTest++/UnitTest++.h"
#include "BlockData2D.hpp"
#include "Cuboid2D.hpp"

namespace iblbm
{
SUITE(TestCore)
{
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestBlockData2D_Constructor_Empty)
{
  BlockData2D<double, double> block;
  // allocating array of zero length with new[] is legal
  CHECK(block.pGetRawData() != nullptr);
  CHECK_EQUAL(0, block.GetDataSize());
  CHECK_EQUAL(0, block.GetDimension());
  CHECK_EQUAL(4, block.GetNumBlock());
  CHECK_EQUAL(3 * sizeof(std::size_t), block.GetSerializableSize());
}

TEST(TestBlockData2D_Constructor_Cuboid_Default)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockData2D<double, double> block {cuboid};

  auto data_size {nx * ny * 1};
  CHECK(block.pGetRawData() != nullptr);
  CHECK_EQUAL(data_size, block.GetDataSize());
  CHECK_EQUAL(1, block.GetDimension());
  CHECK_EQUAL(4, block.GetNumBlock());
  CHECK_EQUAL(3 * sizeof(std::size_t) + data_size * sizeof(double),
      block.GetSerializableSize());
//  CHECK_CLOSE(0.0, block.GetMax(), g_zero_tol);
//  CHECK_CLOSE(0.0, block.GetMin(), g_zero_tol);
  int i = 0;
  int* raw_data = new int[nx * ny];
  int*** field = new int**[nx];
  for (gsl::index x {0}; x < nx; ++x) {
    field[x] = new int*[ny];
    for (gsl::index y {0}; y < ny; ++y) {
      field[x][y] = raw_data + x * ny + y;
      field[x][y][0] = i;
      ++i;
    }
  }
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      std::cout << field[x][y][0] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << *std::max_element(raw_data, raw_data + nx * ny) << std::endl;
  std::cout << *std::min_element(raw_data, raw_data + nx * ny) << std::endl;
//  for (gsl::index x {0}; x < nx; ++x) {
//    for (gsl::index y {0}; y < ny; ++y) {
//      auto idx {x * ny + y};
//      CHECK_CLOSE(0.0, block.rGetData(x, y), g_zero_tol);
//      CHECK_CLOSE(0.0, block[idx], g_zero_tol);
//    }
//  }
}
}
}  // namespace iblbm
