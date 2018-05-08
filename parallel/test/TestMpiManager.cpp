#include "gsl/gsl"
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
// Global variables for testing
static const auto g_loose_tol {1e-6};
static const auto g_zero_tol {1e-20};

TEST(TestMpiManager_Initialize)
{

#ifdef IBLBM_PARALLEL_MPI
  TestMpiManager tester;
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

#ifdef IBLBM_PARALLEL_MPI
TEST(TestMpiManager_Send_Bool)
{
  typedef bool T;

  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  if (MpiManager::Instance().AmMaster()) {
    for (gsl::index i {1}; i < MpiManager::Instance().GetSize(); ++i) {
      T buffer {i % 2 == 0};
      MpiManager::Instance().Send(&buffer, 1, i);
    }
  }
  else {
    T buffer {};
    MpiManager::Instance().Receive(&buffer, 1, 0);
    CHECK_EQUAL(MpiManager::Instance().GetRank() % 2 == 0, buffer);
  }
}

TEST(TestMpiManager_Send_Double)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  if (MpiManager::Instance().AmMaster()) {
    for (gsl::index i {1}; i < MpiManager::Instance().GetSize(); ++i) {
      auto buffer {static_cast<double>(i)};
      MpiManager::Instance().Send(&buffer, 1, i);
    }
  }
  else {
    double buffer {};
    MpiManager::Instance().Receive(&buffer, 1, 0);
    CHECK_CLOSE(static_cast<double>(MpiManager::Instance().GetRank()),
        buffer, g_zero_tol);
  }
}

TEST(TestMpiManager_Isend_Bool)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  MpiNonblockingHelper helper;
  helper.Allocate(MpiManager::Instance().GetSize() - 1);

  if (MpiManager::Instance().AmMaster()) {
    for (gsl::index i {1}; i < MpiManager::Instance().GetSize(); ++i) {
      auto buffer {i % 2 == 0};
      MpiManager::Instance().Isend(&buffer, 1, i,
          &helper.pGetMpiRequest()[i - 1]);
    }
    MpiManager::Instance().WaitAll(helper);
  }
  else {
    bool buffer {};
    MpiManager::Instance().Receive(&buffer, 1, 0);
    CHECK_EQUAL(MpiManager::Instance().GetRank() % 2 == 0, buffer);
  }
}

TEST(TestMpiManager_Isend_Double)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  MpiNonblockingHelper helper;
  helper.Allocate(MpiManager::Instance().GetSize() - 1);

  if (MpiManager::Instance().AmMaster()) {
    for (gsl::index i {1}; i < MpiManager::Instance().GetSize(); ++i) {
      auto buffer {static_cast<double>(i)};
      MpiManager::Instance().Isend(&buffer, 1, i,
          &helper.pGetMpiRequest()[i - 1]);
    }
    MpiManager::Instance().WaitAll(helper);
  }
  else {
    double buffer {};
    MpiManager::Instance().Receive(&buffer, 1, 0);
    CHECK_CLOSE(static_cast<double>(MpiManager::Instance().GetRank()),
        buffer, g_zero_tol);
  }
}

TEST(TestMpiManager_Isend_GslIndex)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  MpiNonblockingHelper helper;
  helper.Allocate(MpiManager::Instance().GetSize() - 1);

  if (MpiManager::Instance().AmMaster()) {
    for (gsl::index i {1}; i < MpiManager::Instance().GetSize(); ++i) {
      gsl::index buffer {i};
      MpiManager::Instance().Isend(&buffer, 1, i,
          &helper.pGetMpiRequest()[i - 1]);
    }
    MpiManager::Instance().WaitAll(helper);
  }
  else {
    gsl::index buffer {};
    MpiManager::Instance().Receive(&buffer, 1, 0);
    CHECK_EQUAL(static_cast<gsl::index>(MpiManager::Instance().GetRank()),
        buffer);
  }
}

TEST(TestMpiManager_Isend_StdSizet)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  MpiNonblockingHelper helper;
  helper.Allocate(MpiManager::Instance().GetSize() - 1);

  if (MpiManager::Instance().AmMaster()) {
    for (gsl::index i {1}; i < MpiManager::Instance().GetSize(); ++i) {
      std::size_t buffer {static_cast<std::size_t>(i)};
      MpiManager::Instance().Isend(&buffer, 1, i,
          &helper.pGetMpiRequest()[i - 1]);
    }
    MpiManager::Instance().WaitAll(helper);
  }
  else {
    std::size_t buffer {};
    MpiManager::Instance().Receive(&buffer, 1, 0);
    CHECK_EQUAL(MpiManager::Instance().GetRank(), buffer);
  }
}

TEST(TestMpiManager_SendReceive_Int)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  auto my_rank {MpiManager::Instance().GetRank()};
  std::size_t num_material {2};
  int materials[num_material];
  int materials_in_buffer[num_material];
  // Set material array value to process rank, so we can check source and
  // destination
  for (gsl::index i {0}; i < num_material; ++i)
      materials[i] = MpiManager::Instance().GetRank();

  for (gsl::index i {1}; i < MpiManager::Instance().GetSize(); ++i) {
    MpiManager::Instance().SendReceive(materials, materials_in_buffer,
        num_material, (my_rank + i) % MpiManager::Instance().GetSize(),
        (my_rank - i) % MpiManager::Instance().GetSize());
    for (gsl::index j {0}; j < num_material; ++j) {
      CHECK_EQUAL((my_rank - i) % MpiManager::Instance().GetSize(),
          materials_in_buffer[j]);
      // Make sure we have not sent to ourselves
      CHECK(materials_in_buffer[j] != my_rank);
    }
  }
}

TEST(TestMpiManager_SendReceive_Double)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  auto my_rank {MpiManager::Instance().GetRank()};
  std::size_t num_material {2};
  double materials[num_material];
  double materials_in_buffer[num_material];
  // Set material array value to process rank, so we can check source and
  // destination
  for (gsl::index i {0}; i < num_material; ++i)
      materials[i] = MpiManager::Instance().GetRank();

  for (gsl::index i {1}; i < MpiManager::Instance().GetSize(); ++i) {
    MpiManager::Instance().SendReceive(materials, materials_in_buffer,
        num_material, (my_rank + i) % MpiManager::Instance().GetSize(),
        (my_rank - i) % MpiManager::Instance().GetSize());
    for (gsl::index j {0}; j < num_material; ++j) {
      CHECK_CLOSE((my_rank - i) % MpiManager::Instance().GetSize(),
          materials_in_buffer[j], g_zero_tol);
      // Make sure we have not sent to ourselves
      CHECK(materials_in_buffer[j] != my_rank);
    }
  }
}

TEST(TestMpiManager_BCast_Int_Default)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  int buffer {0};
  CHECK_EQUAL(0, buffer);
  if (MpiManager::Instance().AmMaster()) buffer = 2;
  MpiManager::Instance().Bcast(&buffer, 1);
  if (!MpiManager::Instance().AmMaster()) CHECK_EQUAL(2, buffer);
}

TEST(TestMpiManager_BCast_Int_UserDefined)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  int buffer {0};
  CHECK_EQUAL(0, buffer);
  if (MpiManager::Instance().GetRank() == 1) buffer = 2;
  MpiManager::Instance().Bcast(&buffer, 1, 1);
  if (MpiManager::Instance().GetRank() != 1) CHECK_EQUAL(2, buffer);
}

TEST(TestMpiManager_Reduce_Int_Default)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  int send_val {static_cast<int>(MpiManager::Instance().GetRank())};
  int recv_val {0};
  MpiManager::Instance().Reduce(send_val, recv_val, MPI_SUM);
  if (MpiManager::Instance().AmMaster()) {
    int sum {0};
    for (gsl::index i {0}; i < MpiManager::Instance().GetSize(); ++i) {
        sum += i;
    }
    CHECK_EQUAL(sum, recv_val);
  }
  else {
    CHECK_EQUAL(0, recv_val);
  }
}

TEST(TestMpiManager_Reduce_Int_UserDefined)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  auto root {2};
  int send_val {static_cast<int>(MpiManager::Instance().GetRank())};
  int recv_val {0};
  MpiManager::Instance().Reduce(send_val, recv_val, MPI_SUM, root);
  if (MpiManager::Instance().GetRank() == root) {
    int sum {0};
    for (gsl::index i {0}; i < MpiManager::Instance().GetSize(); ++i)
        sum += i;
    CHECK_EQUAL(sum, recv_val);
  }
  else {
    CHECK_EQUAL(0, recv_val);
  }
}

TEST(TestMpiManager_ReduceAndBcast_Int_Default)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  int send_val {static_cast<int>(MpiManager::Instance().GetRank())};
  int sum {0};
  for (gsl::index i {0}; i < MpiManager::Instance().GetSize(); ++i)
      sum += i;
  MpiManager::Instance().ReduceAndBcast(send_val, MPI_SUM);
  CHECK_EQUAL(sum, send_val);
}

TEST(TestMpiManager_ReduceAndBcast_Int_UserDefined)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  auto root {2};
  int send_val {static_cast<int>(MpiManager::Instance().GetRank())};
  int sum {0};
  for (gsl::index i {0}; i < MpiManager::Instance().GetSize(); ++i)
      sum += i;
  MpiManager::Instance().ReduceAndBcast(send_val, MPI_SUM, root);
  CHECK_EQUAL(sum, send_val);
}

TEST(TestMpiManager_ReduceAndBcast_Double_Default)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  double send_val {static_cast<double>(MpiManager::Instance().GetRank())};
  double sum {0};
  for (gsl::index i {0}; i < MpiManager::Instance().GetSize(); ++i)
      sum += i;
  MpiManager::Instance().ReduceAndBcast(send_val, MPI_SUM);
  CHECK_CLOSE(sum, send_val, g_zero_tol);
}

TEST(TestMpiManager_ReduceAndBcast_Double_UserDefined)
{
  TestMpiManager tester;
  CHECK_EQUAL(true, tester.GetIsOk());

  auto root {2};
  double send_val {static_cast<double>(MpiManager::Instance().GetRank())};
  double sum {0};
  for (gsl::index i {0}; i < MpiManager::Instance().GetSize(); ++i)
      sum += i;
  MpiManager::Instance().ReduceAndBcast(send_val, MPI_SUM, root);
  CHECK_CLOSE(sum, send_val, g_zero_tol);
}
#endif  // IBLBM_PARALLEL_MPI
}
}  // namespace iblbm
