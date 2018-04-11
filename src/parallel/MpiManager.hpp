#ifndef SRC_PARALLEL_MPIMANAGER_HPP_
#define SRC_PARALLEL_MPIMANAGER_HPP_

#ifdef IBLBM_PARALLEL_MPI
#include "mpi.h"
#include <vector>

#else

// Define some macros to replace MPI related function and variable, to
// prevent compile error in Exception.cpp when we are not using Parallel
// build
#define MPI_Comm int
#define MPI_COMM_WORLD 0
#define MPI_Abort(comm, errorcode)

#endif  // IBLBM_PARALLEL_MPI

#include <string>

#include "Exception.hpp"
#include "OstreamManager.hpp"

namespace iblbm
{
#ifdef IBLBM_PARALLEL_MPI
class MpiManager
{
  friend class TestMpiManager;
 public:
  /** As a convention, we consider processor 0 the master process. */
  static const std::size_t MASTER_RANK = 0;

  /** \return instance of MpiManager */
  static MpiManager& Instance();

  /** Initialize MpiManager. Sets mIsOk if initialization is successful */
  void Initialize(
      int* pArgc
    , char*** pArgv);

  /** \return process rank/ID */
  std::size_t GetRank() const;

  /** \return number of processes */
  std::size_t GetSize() const;

  /** \return flag to indicate if MPI is successfully initialized */
  bool IsOk() const;

  /** \return whether the current process is the master process */
  bool AmMaster() const;

  /** \return Universal MPI-time in seconds */
  double GetTime() const;

  /**
   * If MPI is set up, perform a barrier synchronisation.
   * If not, it's a noop.
   *
   * \param callerId only used in debug mode; printed before & after the
   *        barrier call
   * \param comm object used by MPI to determine which process are involved
   *        in a communication
   */
  void Barrier(
      const std::string callerId = ""
    , MPI_Comm comm = MPI_COMM_WORLD);

 private:
  /** Constructor */
  MpiManager();
  /** Destructor */
  ~MpiManager();

  /** Rank of the process */
  std::size_t mRank;
  /** Total number of processes */
  std::size_t mNumProcessors;
  /** Flag to indicate if MPI is successfully initialized */
  bool mIsOk;
  /** Special output stream manager */
  mutable OstreamManager mOstream;
};

#else

class MpiManager
{
  friend class TestMpiManager;
 public:
  /** As a convention, we consider processor 0 the master process. */
  static const std::size_t MASTER_RANK = 0;

  /** \return instance of MpiManager */
  static MpiManager& Instance()
  {
    static MpiManager instance;
    return instance;
  }

  MpiManager()
    : mRank{0},
      mNumProcessors{1}
  {}

  /** Initialize MpiManager. Does nothing */
  void Initialize(
      int* /*pArgc*/
    , char*** /*pArgv*/)
  {}

  /** \return process rank/ID */
  std::size_t GetRank() const
  {
    return mRank;
  }

  /** \return number of processes */
  std::size_t GetSize() const
  {
    return mNumProcessors;
  }

  /** \return flag to indicate if MPI is successfully initialized */
  bool IsOk() const
  {
    return false;
  }

  /** \return whether the current process is the master process */
  bool AmMaster() const
  {
    return mRank == MASTER_RANK;
  }

  void Barrier(
      const std::string callerId = ""
    , MPI_Comm comm = MPI_COMM_WORLD)
  {
    IBLBM_UNUSED(callerId);
    IBLBM_UNUSED(comm);
  }

 private:
  /** Rank of the process */
  std::size_t mRank;
  /** Total number of processes */
  std::size_t mNumProcessors;
};
#endif  // IBLBM_PARALLEL_MPI
}  // namespace iblbm

#endif  // SRC_PARALLEL_MPIMANAGER_HPP_
