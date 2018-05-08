#ifdef IBLBM_PARALLEL_MPI
#include "MpiManager.hpp"

#include "gsl/gsl"

namespace iblbm
{
#ifndef NDEBUG
// Uncomment this to trace calls to MpiManager::Instance().Barrier
// #define DEBUG_BARRIERS
#endif

#ifdef DEBUG_BARRIERS
static unsigned mNumBarriers {0u};
#endif

/***************************************************************************
 * MpiNonblockingHelper
 ***************************************************************************/
MpiNonblockingHelper::MpiNonblockingHelper()
  : mSize{0},
    mpMpiRequest{nullptr},
    mpMpiStatus{nullptr}
{}

MpiNonblockingHelper::~MpiNonblockingHelper()
{
  Free();
}

MpiNonblockingHelper::MpiNonblockingHelper(const MpiNonblockingHelper& rRhs)
  : mSize{rRhs.mSize}
{
  if (mSize != 0) {
    Allocate(mSize);
    for (gsl::index i {0}; i < mSize; ++i) {
      mpMpiRequest[i] = rRhs.mpMpiRequest[i];
      mpMpiStatus[i]  = rRhs.mpMpiStatus[i];
    }
  }
}

MpiNonblockingHelper& MpiNonblockingHelper::operator=(
    const MpiNonblockingHelper& rRhs)
{
  mSize = rRhs.mSize;
  if (mSize != 0) {
    Allocate(mSize);
    for (gsl::index i {0}; i < mSize; ++i) {
      mpMpiRequest[i] = rRhs.mpMpiRequest[i];
      mpMpiStatus[i]  = rRhs.mpMpiStatus[i];
    }
  }
  return *this;
}

void MpiNonblockingHelper::Swap(MpiNonblockingHelper& rNonblockingHelper)
{
  std::swap(mSize, rNonblockingHelper.mSize);
  std::swap(mpMpiRequest, rNonblockingHelper.mpMpiRequest);
  std::swap(mpMpiStatus, rNonblockingHelper.mpMpiStatus);
}

void MpiNonblockingHelper::Allocate(std::size_t size)
{
  Free();
  mSize = size;
  mpMpiRequest = new MPI_Request[size];
  mpMpiStatus = new MPI_Status[size];
}

void MpiNonblockingHelper::Free()
{
  if (mSize != 0) {
    delete[] mpMpiRequest;
    delete[] mpMpiStatus;
    mSize = 0;
  }
}

const std::size_t& MpiNonblockingHelper::rGetSize() const
{
  return mSize;
}

/** \return Read and write access mpMpiRequest */
MPI_Request* MpiNonblockingHelper::pGetMpiRequest() const
{
  return mpMpiRequest;
}

/** \return Read and write access mpMpiStatus */
MPI_Status* MpiNonblockingHelper::pGetMpiStatus() const
{
  return mpMpiStatus;
}

/***************************************************************************
 * MpiManager
 ***************************************************************************/
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

// LCOV_EXCL_START
double MpiManager::GetTime() const
{
  return mIsOk ? MPI_Wtime() : 0;
}
// LCOV_EXCL_STOP

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

template<>
void MpiManager::Send<bool>(
    bool* pBuffer
  , int count
  , int destination
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Send(static_cast<void*>(pBuffer), count, MPI_BYTE, destination, tag,
      comm);
}

template<>
void MpiManager::Send<double>(
    double* pBuffer
  , int count
  , int destination
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Send(static_cast<void*>(pBuffer), count, MPI_DOUBLE, destination, tag,
      comm);
}

template<>
void MpiManager::Isend<bool>(
    bool* pBuffer
  , int count
  , int destination
  , MPI_Request* pRequest
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Isend(static_cast<void*>(pBuffer), count, MPI_BYTE, destination, tag,
      comm, pRequest);
}

template<>
void MpiManager::Isend<double>(
    double* pBuffer
  , int count
  , int destination
  , MPI_Request* pRequest
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Isend(static_cast<void*>(pBuffer), count, MPI_DOUBLE, destination, tag,
      comm, pRequest);
}

template<>
void MpiManager::Isend<gsl::index>(
    gsl::index* pBuffer
  , int count
  , int destination
  , MPI_Request* pRequest
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Isend(static_cast<void*>(pBuffer), count, MPI_LONG, destination, tag,
      comm, pRequest);
}

template<>
void MpiManager::Isend<std::size_t>(
    std::size_t* pBuffer
  , int count
  , int destination
  , MPI_Request* pRequest
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Isend(static_cast<void*>(pBuffer), count, MPI_UNSIGNED_LONG_LONG,
      destination, tag, comm, pRequest);
}

template<>
void MpiManager::Receive<bool>(
    bool* pBuffer
  , int count
  , int source
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Status status;
  MPI_Recv(static_cast<void*>(pBuffer), count, MPI_BYTE, source, tag, comm,
      &status);
}

template<>
void MpiManager::Receive<double>(
    double* pBuffer
  , int count
  , int source
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Status status;
  MPI_Recv(static_cast<void*>(pBuffer), count, MPI_DOUBLE, source, tag, comm,
      &status);
}

template<>
void MpiManager::Receive<std::size_t>(
    std::size_t* pBuffer
  , int count
  , int source
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Status status;
  MPI_Recv(static_cast<void*>(pBuffer), count, MPI_UNSIGNED_LONG_LONG,
      source, tag, comm, &status);
}

template<>
void MpiManager::Receive<gsl::index>(
    gsl::index* pBuffer
  , int count
  , int source
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Status status;
  MPI_Recv(static_cast<void*>(pBuffer), count, MPI_LONG, source, tag, comm,
      &status);
}

template<>
void MpiManager::SendReceive<int>(
    int* pSendBuffer
  , int* pRecvBuffer
  , int count
  , int destination
  , int source
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Status status;
  MPI_Sendrecv(static_cast<void*>(pSendBuffer), count, MPI_INT, destination,
      tag, static_cast<void*>(pRecvBuffer), count, MPI_INT, source, tag,
      comm, &status);
}

template<>
void MpiManager::SendReceive<double>(
    double* pSendBuffer
  , double* pRecvBuffer
  , int count
  , int destination
  , int source
  , int tag/*=0*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Status status;
  MPI_Sendrecv(static_cast<void*>(pSendBuffer), count, MPI_DOUBLE,
      destination, tag, static_cast<void*>(pRecvBuffer), count, MPI_DOUBLE,
      source, tag, comm, &status);
}

template<>
void MpiManager::Bcast<int>(
    int* pBuffer
  , int count
  , int root/*=MASTER_RANK*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Bcast(static_cast<void*>(pBuffer), count, MPI_INT, root, comm);
}

template<>
void MpiManager::Reduce<int>(
    int& rSendVal
  , int& rRecvVal
  , MPI_Op op
  , int root/*=MASTER_RANK*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  MPI_Reduce(static_cast<void*>(&rSendVal), static_cast<void*>(&rRecvVal),
      /*count=*/1, MPI_INT, op, root, comm);
}

template<>
void MpiManager::ReduceAndBcast<int>(
    int& rReduceVal
  , MPI_Op op
  , int root/*=MASTER_RANK*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  int recv_val;
  MPI_Reduce(&rReduceVal, &recv_val, /*count=*/1, MPI_INT, op, root, comm);
  rReduceVal = recv_val;
  MPI_Bcast(&rReduceVal, /*count=*/1, MPI_INT, root, comm);
}

template<>
void MpiManager::ReduceAndBcast<double>(
    double& rReduceVal
  , MPI_Op op
  , int root/*=MASTER_RANK*/
  , MPI_Comm comm/*=MPI_COMM_WORLD*/)
{
  if (!mIsOk) return;
  double recv_val;
  MPI_Reduce(&rReduceVal, &recv_val, /*count=*/1, MPI_DOUBLE, op, root, comm);
  rReduceVal = recv_val;
  MPI_Bcast(&rReduceVal, /*count=*/1, MPI_DOUBLE, root, comm);
}

void MpiManager::WaitAll(MpiNonblockingHelper& rHelper)
{
  if (!mIsOk) return;
  MPI_Waitall(rHelper.rGetSize(), rHelper.pGetMpiRequest(),
      rHelper.pGetMpiStatus());
}
}  // namespace iblbm

#endif  // IBLBM_PARALLEL_MPI
