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
  CHECK_CLOSE(0.0, block.GetMax(), g_zero_tol);
  CHECK_CLOSE(0.0, block.GetMin(), g_zero_tol);
}

TEST(TestBlockData2D_Constructor_Cuboid_UserDefined)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};
  auto nd {9u};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockData2D<double, double> block {cuboid, nd};

  auto data_size {nx * ny * nd};
  CHECK(block.pGetRawData() != nullptr);
  CHECK_EQUAL(data_size, block.GetDataSize());
  CHECK_EQUAL(nd, block.GetDimension());
  CHECK_EQUAL(4, block.GetNumBlock());
  CHECK_EQUAL(3 * sizeof(std::size_t) + data_size * sizeof(double),
      block.GetSerializableSize());
  CHECK_CLOSE(0.0, block.GetMax(), g_zero_tol);
  CHECK_CLOSE(0.0, block.GetMin(), g_zero_tol);
}

TEST(TestBlockData2D_Constructor_Index_Default)
{
  auto nx {7u};
  auto ny {8u};

  BlockData2D<double, double> block {nx, ny};

  auto data_size {nx * ny * 1};
  CHECK(block.pGetRawData() != nullptr);
  CHECK_EQUAL(data_size, block.GetDataSize());
  CHECK_EQUAL(1, block.GetDimension());
  CHECK_EQUAL(4, block.GetNumBlock());
  CHECK_EQUAL(3 * sizeof(std::size_t) + data_size * sizeof(double),
      block.GetSerializableSize());
  CHECK_CLOSE(0.0, block.GetMax(), g_zero_tol);
  CHECK_CLOSE(0.0, block.GetMin(), g_zero_tol);
}

TEST(TestBlockData2D_Constructor_Index_UserDefined)
{
  auto nx {7u};
  auto ny {8u};
  auto nd {9u};

  BlockData2D<double, double> block {nx, ny, nd};

  auto data_size {nx * ny * nd};
  CHECK(block.pGetRawData() != nullptr);
  CHECK_EQUAL(data_size, block.GetDataSize());
  CHECK_EQUAL(nd, block.GetDimension());
  CHECK_EQUAL(4, block.GetNumBlock());
  CHECK_EQUAL(3 * sizeof(std::size_t) + data_size * sizeof(double),
      block.GetSerializableSize());
  CHECK_CLOSE(0.0, block.GetMax(), g_zero_tol);
  CHECK_CLOSE(0.0, block.GetMin(), g_zero_tol);
}

TEST(TestBlockData2D_Constructor_Copy)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};
  auto nd {9u};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockData2D<double, double> block {cuboid, nd};
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto val {static_cast<double>(x + y + d)};
        block.rGetData(x, y, d) = val;
      }
    }
  }
  BlockData2D<double, double> block_2 {block};

  auto data_size {nx * ny * nd};
  auto* p_raw_data {block_2.pGetRawData()};
  CHECK(p_raw_data != nullptr);
  CHECK_EQUAL(data_size, block_2.GetDataSize());
  CHECK_EQUAL(nd, block_2.GetDimension());
  CHECK_EQUAL(4, block_2.GetNumBlock());
  CHECK_EQUAL(3 * sizeof(std::size_t) + data_size * sizeof(double),
      block_2.GetSerializableSize());
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto idx {(x * ny + y) * nd + d};
        auto val {static_cast<double>(x + y + d)};
        CHECK_CLOSE(val, block_2.rGetData(x, y, d), g_zero_tol);
        CHECK_CLOSE(val, std::as_const(block_2).rGetData(x, y, d),
            g_zero_tol);
        CHECK_CLOSE(val, block_2[idx], g_zero_tol);
        CHECK_CLOSE(val, std::as_const(block_2)[idx], g_zero_tol);
        CHECK_CLOSE(val, p_raw_data[idx], g_zero_tol);
      }
    }
  }
  CHECK_CLOSE(static_cast<double>(nx - 1 + ny - 1 + nd - 1), block_2.GetMax(),
      g_zero_tol);
  CHECK_CLOSE(0.0, block_2.GetMin(), g_zero_tol);
}

TEST(TestBlockData2D_CopyAssignment)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};
  auto nd {9u};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockData2D<double, double> block {cuboid, nd};
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto val {static_cast<double>(x + y + d)};
        block.rGetData(x, y, d) = val;
      }
    }
  }
  BlockData2D<double, double> block_2 {cuboid, nd};
  block_2 = block;

  auto data_size {nx * ny * nd};
  auto* p_raw_data {block_2.pGetRawData()};
  CHECK(p_raw_data != nullptr);
  CHECK_EQUAL(data_size, block_2.GetDataSize());
  CHECK_EQUAL(nd, block_2.GetDimension());
  CHECK_EQUAL(4, block_2.GetNumBlock());
  CHECK_EQUAL(3 * sizeof(std::size_t) + data_size * sizeof(double),
      block_2.GetSerializableSize());
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto idx {(x * ny + y) * nd + d};
        auto val {static_cast<double>(x + y + d)};
        CHECK_CLOSE(val, block_2.rGetData(x, y, d), g_zero_tol);
        CHECK_CLOSE(val, std::as_const(block_2).rGetData(x, y, d),
            g_zero_tol);
        CHECK_CLOSE(val, block_2[idx], g_zero_tol);
        CHECK_CLOSE(val, std::as_const(block_2)[idx], g_zero_tol);
        CHECK_CLOSE(val, p_raw_data[idx], g_zero_tol);
      }
    }
  }
  CHECK_CLOSE(static_cast<double>(nx - 1 + ny - 1 + nd - 1), block_2.GetMax(),
      g_zero_tol);
  CHECK_CLOSE(0.0, block_2.GetMin(), g_zero_tol);
}

TEST(TestBlockData2D_Constructor_Move)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};
  auto nd {9u};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockData2D<double, double> block {cuboid, nd};
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto val {static_cast<double>(x + y + d)};
        block.rGetData(x, y, d) = val;
      }
    }
  }
  BlockData2D<double, double> block_2 {std::move(block)};

  auto data_size {nx * ny * nd};
  auto* p_raw_data {block_2.pGetRawData()};
  CHECK(p_raw_data != nullptr);
  CHECK_EQUAL(data_size, block_2.GetDataSize());
  CHECK_EQUAL(nd, block_2.GetDimension());
  CHECK_EQUAL(4, block_2.GetNumBlock());
  CHECK_EQUAL(3 * sizeof(std::size_t) + data_size * sizeof(double),
      block_2.GetSerializableSize());
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto idx {(x * ny + y) * nd + d};
        auto val {static_cast<double>(x + y + d)};
        CHECK_CLOSE(val, block_2.rGetData(x, y, d), g_zero_tol);
        CHECK_CLOSE(val, std::as_const(block_2).rGetData(x, y, d),
            g_zero_tol);
        CHECK_CLOSE(val, block_2[idx], g_zero_tol);
        CHECK_CLOSE(val, std::as_const(block_2)[idx], g_zero_tol);
        CHECK_CLOSE(val, p_raw_data[idx], g_zero_tol);
      }
    }
  }
  CHECK_CLOSE(static_cast<double>(nx - 1 + ny - 1 + nd - 1), block_2.GetMax(),
      g_zero_tol);
  CHECK_CLOSE(0.0, block_2.GetMin(), g_zero_tol);
}

TEST(TestBlockData2D_Reset)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};
  auto nd {9u};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockData2D<double, double> block {cuboid, nd};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto val {static_cast<double>(x + y + d)};
        block.rGetData(x, y, d) = val;
      }
    }
  }
  auto* p_raw_data {block.pGetRawData()};
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto idx {(x * ny + y) * nd + d};
        auto val {static_cast<double>(x + y + d)};
        CHECK_CLOSE(val, block.rGetData(x, y, d), g_zero_tol);
        CHECK_CLOSE(val, std::as_const(block).rGetData(x, y, d), g_zero_tol);
        CHECK_CLOSE(val, block[idx], g_zero_tol);
        CHECK_CLOSE(val, std::as_const(block)[idx], g_zero_tol);
        CHECK_CLOSE(val, p_raw_data[idx], g_zero_tol);
      }
    }
  }
  CHECK_CLOSE(static_cast<double>(nx - 1 + ny - 1 + nd - 1), block.GetMax(),
      g_zero_tol);
  CHECK_CLOSE(0.0, block.GetMin(), g_zero_tol);

  block.Reset();
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto idx {(x * ny + y) * nd + d};
        CHECK_CLOSE(0.0, block.rGetData(x, y, d), g_zero_tol);
        CHECK_CLOSE(0.0, std::as_const(block).rGetData(x, y, d), g_zero_tol);
        CHECK_CLOSE(0.0, block[idx], g_zero_tol);
        CHECK_CLOSE(0.0, std::as_const(block)[idx], g_zero_tol);
        CHECK_CLOSE(0.0, p_raw_data[idx], g_zero_tol);
      }
    }
  }
  CHECK_CLOSE(0.0, block.GetMax(), g_zero_tol);
  CHECK_CLOSE(0.0, block.GetMin(), g_zero_tol);
}

TEST(TestBlockData2D_rGetData)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};
  auto nd {9u};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockData2D<double, double> block {cuboid, nd};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto val {static_cast<double>(x + y + d)};
        block.rGetData(x, y, d) = val;
      }
    }
  }
  auto* p_raw_data {block.pGetRawData()};
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto idx {(x * ny + y) * nd + d};
        auto val {static_cast<double>(x + y + d)};
        CHECK_CLOSE(val, block.rGetData(x, y, d), g_zero_tol);
        CHECK_CLOSE(val, std::as_const(block).rGetData(x, y, d), g_zero_tol);
        CHECK_CLOSE(val, block[idx], g_zero_tol);
        CHECK_CLOSE(val, std::as_const(block)[idx], g_zero_tol);
        CHECK_CLOSE(val, p_raw_data[idx], g_zero_tol);
      }
    }
  }
  CHECK_CLOSE(static_cast<double>(nx - 1 + ny - 1 + nd - 1), block.GetMax(),
      g_zero_tol);
  CHECK_CLOSE(0.0, block.GetMin(), g_zero_tol);
}

TEST(TestBlockData2D_BracketOperator)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};
  auto nd {9u};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockData2D<double, double> block {cuboid, nd};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto idx {(x * ny + y) * nd + d};
        auto val {static_cast<double>(x + y + d)};
        block[idx] = val;
      }
    }
  }
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto idx {(x * ny + y) * nd + d};
        auto val {static_cast<double>(x + y + d)};
        CHECK_CLOSE(val, block[idx], g_zero_tol);
        CHECK_CLOSE(val, std::as_const(block)[idx], g_zero_tol);
      }
    }
  }
  CHECK_CLOSE(static_cast<double>(nx - 1 + ny - 1 + nd - 1), block.GetMax(),
      g_zero_tol);
  CHECK_CLOSE(0.0, block.GetMin(), g_zero_tol);
}

TEST(TestBlockData2D_ParenthesisOperator)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};
  auto nd {9u};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockData2D<double, double> block {cuboid, nd};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto idx {(x * ny + y) * nd + d};
        auto val {static_cast<double>(x + y + d)};
        block[idx] = val;
      }
    }
  }
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto val {static_cast<double>(x + y + d)};
        CHECK_CLOSE(val, *reinterpret_cast<double*>(block(x, y, d)),
            g_zero_tol);
      }
    }
  }
}

TEST(TestBlockData2D_pGetBlock)
{
  auto x_pos {1.2};
  auto y_pos {3.4};
  auto delta_R {0.1};
  auto nx {7u};
  auto ny {8u};
  auto nd {9u};

  Cuboid2D<double> cuboid {x_pos, y_pos, delta_R, nx, ny};
  BlockData2D<double, double> block {cuboid, nd};

  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto idx {(x * ny + y) * nd + d};
        auto val {static_cast<double>(x + y + d)};
        block[idx] = val;
      }
    }
  }

  gsl::index block_index {0};
  std::size_t block_size {0};
  bool* p_data {nullptr};

  p_data = block.pGetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(nd, *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  p_data = block.pGetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(nx, *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  p_data = block.pGetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(std::size_t), block_size);
  CHECK_EQUAL(ny, *reinterpret_cast<std::size_t*>(p_data));
  ++block_index;

  p_data = block.pGetBlock(block_index, block_size);
  CHECK_EQUAL(sizeof(double) * nx * ny * nd, block_size);
  for (gsl::index x {0}; x < nx; ++x) {
    for (gsl::index y {0}; y < ny; ++y) {
      for (gsl::index d {0}; d < nd; ++d) {
        auto idx {(x * ny + y) * nd + d};
        auto val {static_cast<double>(x + y + d)};
        CHECK_CLOSE(val, reinterpret_cast<double*>(p_data)[idx], g_zero_tol);
      }
    }
  }
}
}
}  // namespace iblbm
