#ifdef IBLBM_PARALLEL_MPI
#include "gsl/gsl"
#include "UnitTest++/UnitTest++.h"
#include "MpiManager.hpp"

namespace iblbm
{
SUITE(TestParallel)
{
TEST(TestMpiNonblockingHelper_Constructor_Default)
{
  MpiNonblockingHelper helper;
  CHECK_EQUAL(0, helper.rGetSize());
  CHECK(helper.pGetMpiRequest() == nullptr);
  CHECK(helper.pGetMpiStatus() == nullptr);
}

TEST(TestMpiNonblockingHelper_Allocate_Free)
{
  std::size_t size {MpiManager::Instance().GetSize()};
  MpiNonblockingHelper helper;
  helper.Allocate(size);

  CHECK_EQUAL(size, helper.rGetSize());
  CHECK(helper.pGetMpiRequest() != nullptr);
  CHECK(helper.pGetMpiStatus() != nullptr);

  helper.Free();

  CHECK_EQUAL(0, helper.rGetSize());
  // Since we only delete but not setting pointers to nullptr
  CHECK(helper.pGetMpiRequest() != nullptr);
  CHECK(helper.pGetMpiStatus() != nullptr);
}
}
}  // namespace iblbm
#endif  // IBLBM_PARALLEL_MPI
