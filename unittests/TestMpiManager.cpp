#include "UnitTest++/UnitTest++.h"
#include "MpiManager.hpp"

namespace iblbm
{
class TestMpiManager
{
 public:
#ifdef IBLBM_PARALLEL_MPI
  bool GetIsOk()
  {
    return MpiManager::Instance().mIsOk;
  }
#endif  // IBLBM_PARALLEL_MPI
};

SUITE(TestParallel)
{
TEST(TestMpiManager_Initialize)
{
  TestMpiManager tester;

#ifdef IBLBM_PARALLEL_MPI

  CHECK_EQUAL(true, tester.GetIsOk());

  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  CHECK_EQUAL(static_cast<std::size_t>(my_rank),
      MpiManager::Instance().GetRank());

  auto am_master = my_rank == 0;
  CHECK_EQUAL(am_master, MpiManager::Instance().AmMaster());

  int num_proc;
  MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
  CHECK_EQUAL(static_cast<std::size_t>(num_proc),
      MpiManager::Instance().GetSize());

#else

  CHECK_EQUAL(0, MpiManager::Instance().GetRank());
  CHECK_EQUAL(true, MpiManager::Instance().AmMaster());
  CHECK_EQUAL(1, MpiManager::Instance().GetSize());

#endif  // IBLBM_PARALLEL_MPI
}
}
}  // namespace iblbm
