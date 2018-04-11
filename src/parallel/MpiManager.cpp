#ifdef IBLBM_PARALLEL_MPI
#include "MpiManager.hpp"

namespace iblbm
{
#ifndef NDEBUG
// Uncomment this to trace calls to MpiManager::Instance().Barrier
// #define DEBUG_BARRIERS
#endif

#ifdef DEBUG_BARRIERS
static unsigned mNumBarriers = 0u;
#endif

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

bool MpiManager::IsOk() const
{
  return mIsOk;
}

bool MpiManager::AmMaster() const
{
  return mRank == MASTER_RANK;
}

double MpiManager::GetTime() const
{
  return mIsOk ? MPI_Wtime() : 0;
}

void MpiManager::Barrier(
    const std::string callerId/*=""*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (mIsOk) {
#ifdef DEBUG_BARRIERS
    // "Before" is alphabetically before "Post" so that one can sort the
    // output on process/event/barrier
    std::cout << "DEBUG: proc " << mRank << ": Before " << "Barrier " <<
        mNumBarriers << " \""<< callerId <<  "\"." << std::endl <<
        std::flush;
#endif
    MPI_Barrier(comm);
#ifdef DEBUG_BARRIERS
    std::cout << "DEBUG: proc " << mRank << ": Post " << "Barrier " <<
        mNumBarriers++ << " \""<< callerId <<  "\"." << std::endl <<
        std::flush;
#endif
  }
}
}  // namespace iblbm

#endif  // IBLBM_PARALLEL_MPI
