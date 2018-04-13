#include "UnitTest++/UnitTest++.h"
#include "BlockData2D.hpp"

namespace iblbm
{
SUITE(TestCore)
{
TEST(TestBlockData2D_Constructor)
{
  BlockData2D<double, double> block_1;
}
}
}  // namespace iblbm
