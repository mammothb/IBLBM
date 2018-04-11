#include "HeuristicLoadBalancer.hpp"

namespace iblbm
{
template<typename T>
HeuristicLoadBalancer<T>::HeuristicLoadBalancer()
  : mpCuboidGeometry2D{nullptr},
    mRatioFullEmpty(1.0)
{}

template<typename T>
HeuristicLoadBalancer<T>::HeuristicLoadBalancer(
    CuboidGeometry2D<T>& rCuboidGeometry2D
  , const double ratioFullEmpty/*=3.7*/
  , const double emptyCellWeight/*=0.0*/)
  : mRatioFullEmpty{ratioFullEmpty}
{
  Initialize(rCuboidGeometry2D, ratioFullEmpty, emptyCellWeight);
}

template<typename T>
HeuristicLoadBalancer<T>::~HeuristicLoadBalancer()
{}

template<typename T>
void HeuristicLoadBalancer<T>::Initialize(
    CuboidGeometry2D<T>& rCuboidGeometry2D
  , const double ratioFullEmpty/*=3.7*/
  , const double emptyCellWeight/*=0.0*/)
{
  mRatioFullEmpty = ratioFullEmpty;
  this->mGlobalIndex.clear();
  mpCuboidGeometry2D = &rCuboidGeometry2D;
  auto rank {MpiManager::Instance().GetRank()};
  auto size {MpiManager::Instance().GetSize()};
  auto nc {mpCuboidGeometry2D->GetNumberOfCuboids()};

  // If we're running on a single process, assignment all cuboids to this
  // load balancer
  if (size == 1) {
    for (gsl::index i {0}; i < nc; ++i) {
      this->mGlobalIndex.push_back(i);
      this->mLocalIndex[i] = i;
      this->mRank[i] = rank;
    }
    this->mSize = nc;
    return;
  }

  std::vector<gsl::index> cuboid_to_thread(nc);
  std::vector<gsl::index> partition_result(nc);
  std::vector<int> node_weights(nc);
  if (MpiManager::Instance().AmMaster()) {
    for (gsl::index i {0}; i < nc; ++i) {
      auto num_full_cell {mpCuboidGeometry2D->rGetCuboid(i).GetWeight()};
      node_weights[i] = static_cast<int>(emptyCellWeight *
          (mpCuboidGeometry2D->rGetCuboid(i).GetLatticeVolume() -
              num_full_cell)) + static_cast<int>(ratioFullEmpty *
              num_full_cell);
    }

    // Flags to determine if the cuboid has been assigned to a process
    std::vector<bool> is_assigned(nc, false);
    // Sum of weights of cuboids assigned to each process
    std::vector<int> process_load(size, 0);
    auto max_weight {-1};
    gsl::index max_weight_idx {-1};
    // Try to evenly split the load among processes
    do {
      max_weight = -1;
      max_weight_idx = -1;
      for (gsl::index i {0}; i < nc; ++i) {
        if (!is_assigned[i] && node_weights[i] > max_weight) {
          max_weight = node_weights[i];
          max_weight_idx = i;
        }
      }
      if (max_weight_idx != -1) {
        auto min_load {process_load[0]};
        gsl::index min_load_idx {0};
        for (gsl::index p {1}; p < size; ++p) {
          if (process_load[p] < min_load) {
            min_load = process_load[p];
            min_load_idx = p;
          }
        }
        is_assigned[max_weight_idx] = true;
        process_load[min_load_idx] += max_weight;
        partition_result[max_weight_idx] = min_load_idx;
      }
    } while (max_weight != -1);

//    std::cout << "node_weights" << std::endl;
//    for (int i = 0; i < nc; i++)  {
//      std::cout << "[" << i << "]="<< node_weights[i] << std::endl;
//    }
//
//    for (int i = 0; i < size; i++) {
//      std::cout << "process_load[" << i << "]=" << process_load[i] << std::endl;
//    }
//
//    std::cout << "node_weights" << std::endl;
//    for (int i = 0; i < nc; i++)  {
//      std::cout << node_weights[i] << std::endl;
//    }
//
//    std::cout << "nc " << nc << " size " << size << std::endl;

    // Assign cuboid and set up global-to-local index mapping for master
    // process
    std::size_t num_master_cuboid {0};
    for (gsl::index i {0}; i < nc; ++i) {
      if (partition_result[i] == 0) {
        this->mLocalIndex[i] = num_master_cuboid;
        this->mGlobalIndex.push_back(i);
        ++num_master_cuboid;
      }
      this->mRank[i] = partition_result[i];
      cuboid_to_thread[i] = partition_result[i];
    }
    this->mSize = num_master_cuboid;
    std::cout << "partition_result cuboid_to_thread" << std::endl;
    for (int i = 0; i < nc; i++)  {
      std::cout << partition_result[i] << " " << cuboid_to_thread[i] << std::endl;
    }
  }
#ifdef IBLBM_PARALLEL_MPI
//  if (MpiManager::Instance().AmMaster()) {
//    // Send all threads their respective cuboids
//    mMpiNonBlockingHelper.free();
//    mMpiNonBlockingHelper.allocate(size-1);
//    for (gsl::index i {1}; i < size; ++i) {
//      MpiManager::Instance().iSend(&cuboid_to_thread.front(), nc, i,
//          &mMpiNonblockingHelper.get_mpiRequest()[i - 1], 0);
//    }
//    MpiManager::Instance().WaitAll(mMpiNonBlockingHelper);
//  }
//  else {
//    int *tmpCuboids = new int[nC];
//    singleton::mpi().receive(tmpCuboids, nC, 0, 0);
//    int count = 0;
//    for (int i = 0; i < nC; ++i) {
//      if (tmpCuboids[i] == rank) {
//        this->_glob.push_back(i);
//        this->_loc[i] = count;
//        count++;
//      };
//      this->_rank[i] = tmpCuboids[i];
//    }
//    delete[] tmpCuboids;
//    this->_size = count;
//  }
#endif  // IBLBM_PARALLEL_MPI
}

// explicit instantiation
template class HeuristicLoadBalancer<double>;
}  // namespace iblbm
