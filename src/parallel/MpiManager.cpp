#ifdef IBLBM_PARALLEL_MPI
#include "MpiManager.hpp"

namespace iblbm
{
MpiManager& MpiManager::Instance()
{
  static MpiManager instance;
  return instance;
}

MpiManager::MpiManager()
  : mRank{0},
    mNumProcessors{0},
    mIsOk{false},
    mOstream{std::cout, "MpiManager"}
{}

MpiManager::~MpiManager()
{
  // Only terminate if it was successfully initialized
  if (mIsOk) {
    MPI_Finalize();
    mIsOk = false;
  }
}

void MpiManager::Initialize(
    int* pArgc
  , char*** pArgv)
{
  int my_rank;
  int num_proc;
  auto return_code_1 = MPI_Init(pArgc, pArgv);
  auto return_code_2 = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  auto return_code_3 = MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
  mIsOk = (return_code_1 == 0 && return_code_2 == 0 && return_code_3 == 0);
  mRank = static_cast<std::size_t>(my_rank);
  mNumProcessors = static_cast<std::size_t>(num_proc);
  mOstream << "Successfully initialized, NUM_PROCESSES = " << GetSize() <<
      std::endl;
}

std::size_t MpiManager::GetRank() const
{
  return mRank;
}

std::size_t MpiManager::GetSize() const
{
  return mNumProcessors;
}

bool MpiManager::AmMaster() const
{
  return mRank == MASTER_RANK;
}

double MpiManager::GetTime() const
{
  return mIsOk ? MPI_Wtime() : 0;
}

void MpiManager::Barrier(MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Barrier(comm);
}
}  // namespace iblbm

#endif  // IBLBM_PARALLEL_MPI
