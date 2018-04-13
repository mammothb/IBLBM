#include "UnitTest++/UnitTest++.h"
#include "BlockStructure2D.hpp"

namespace iblbm
{
SUITE(TestCore)
{
TEST(TestBlockStructure2D_Constructor)
{
  BlockStructure2D block_1 {1, 2};
  CHECK_EQUAL(1, block_1.GetNx());
  CHECK_EQUAL(2, block_1.GetNy());

  BlockStructure2D block_2 {3, 4};
  CHECK_EQUAL(3, block_2.GetNx());
  CHECK_EQUAL(4, block_2.GetNy());

  BlockStructure2D block_3 {5, 6};
  CHECK_EQUAL(5, block_3.GetNx());
  CHECK_EQUAL(6, block_3.GetNy());

  BlockStructure2D block_4 {7, 8};
  CHECK_EQUAL(7, block_4.GetNx());
  CHECK_EQUAL(8, block_4.GetNy());
}
}
}  // namespace iblbm
