#ifndef SRC_PARALLEL_MPIMANAGER_HPP_
#define SRC_PARALLEL_MPIMANAGER_HPP_

#ifdef IBLBM_PARALLEL_MPI
#include "mpi.hpp"
#include <vector>
#endif  // IBLBM_PARALLEL_MPI

#include <string>

#include "OstreamManager.hpp"

namespace iblbm
{
namespace singleton
{

}  // namespace singleton
}  // namespace iblbm

#endif // SRC_PARALLEL_MPIMANAGER_HPP_
