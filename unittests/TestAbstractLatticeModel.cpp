#include <iostream>

#include "AbstractLatticeModel.hpp"
#include "UnitTest++.h"

namespace iblbm
{
SUITE(TestLatticeModel)
{
TEST(TestAbstractLatticeModel_Constructor)
{
  auto nx = 2u;
  auto ny = 4u;
  AbstractLatticeModel model(2u, 9u, nx, ny);

  CHECK_EQUAL(nx, model.GetNx());
  CHECK_EQUAL(ny, model.GetNy());
}
}
}  // namespace iblbm
