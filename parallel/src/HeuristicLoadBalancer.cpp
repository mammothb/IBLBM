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
  auto nc {mpCuboidGeometry2D->GetNumCuboid()};

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

  // Vector to track the cuboid's assigned thread number
  std::vector<gsl::index> cuboid_to_thread(nc);
  std::vector<std::size_t> node_weights(nc);
  if (MpiManager::Instance().AmMaster()) {
    for (gsl::index i {0}; i < nc; ++i) {
      auto num_full_cell {mpCuboidGeometry2D->rGetCuboid(i).GetWeight()};
      node_weights[i] = emptyCellWeight *
          (mpCuboidGeometry2D->rGetCuboid(i).GetLatticeVolume() -
              num_full_cell) + ratioFullEmpty * num_full_cell;
    }

    // Flags to determine if the cuboid has been assigned to a process
    std::vector<bool> is_assigned(nc, false);
    // Sum of weights of cuboids assigned to each process
    std::vector<int> process_load(size, 0);

    std::size_t num_assigned {0};
    while (num_assigned < nc) {
      auto is_unset {true};
      std::size_t max_weight {0};
      gsl::index max_weight_idx {-1};
      // Find the max weight among unassigned nodes
      for (gsl::index i {0}; i < nc; ++i) {
        if (!is_assigned[i] && (node_weights[i] > max_weight || is_unset)) {
          max_weight = node_weights[i];
          max_weight_idx = i;
          is_unset = false;
        }
      }
      if (max_weight_idx != -1) {
        auto min_load {process_load[0]};
        gsl::index min_load_idx {0};
        // Find the thread with the least load to take in the cuboid
        for (gsl::index p {1}; p < size; ++p) {
          if (process_load[p] < min_load) {
            min_load = process_load[p];
            min_load_idx = p;
          }
        }
        is_assigned[max_weight_idx] = true;
        process_load[min_load_idx] += max_weight;
        cuboid_to_thread[max_weight_idx] = min_load_idx;
        ++num_assigned;
      }
    };

    // Assign cuboid and set up global-to-local index mapping for master
    // process
    std::size_t num_assigned_cuboid {0};
    for (gsl::index i {0}; i < nc; ++i) {
      if (cuboid_to_thread[i] == 0) {
        this->mLocalIndex[i] = num_assigned_cuboid;
        this->mGlobalIndex.push_back(i);
        ++num_assigned_cuboid;
      }
      this->mRank[i] = cuboid_to_thread[i];
    }
    this->mSize = num_assigned_cuboid;
  }
#ifdef IBLBM_PARALLEL_MPI
  if (MpiManager::Instance().AmMaster()) {
    // Send all threads their respective cuboids
    mNonblockingHelper.Free();
    mNonblockingHelper.Allocate(size - 1);
    for (gsl::index i {1}; i < size; ++i) {
      MpiManager::Instance().Isend(&cuboid_to_thread.front(), nc, i,
          &mNonblockingHelper.pGetMpiRequest()[i - 1]);
    }
    MpiManager::Instance().WaitAll(mNonblockingHelper);
  }
  else {
    auto* p_tmp_cuboids {new gsl::index[nc]};
    MpiManager::Instance().Receive(p_tmp_cuboids, nc, 0, 0);
    std::size_t num_assigned_cuboid {0};
    for (gsl::index i {0}; i < nc; ++i) {
      if (p_tmp_cuboids[i] == rank) {
        this->mLocalIndex[i] = num_assigned_cuboid;
        this->mGlobalIndex.push_back(i);
        ++num_assigned_cuboid;
      };
      this->mRank[i] = p_tmp_cuboids[i];
    }
    delete[] p_tmp_cuboids;
    this->mSize = num_assigned_cuboid;
  }
#endif  // IBLBM_PARALLEL_MPI
}

template<typename T>
void HeuristicLoadBalancer<T>::Swap(HeuristicLoadBalancer<T>& rLoadBalancer)
{
  LoadBalancer<T>::Swap(rLoadBalancer);
#ifdef IBLBM_PARALLEL_MPI
  mNonblockingHelper.Swap(rLoadBalancer.mNonblockingHelper);
#endif  // IBLBM_PARALLEL_MPI
  std::swap(mpCuboidGeometry2D, rLoadBalancer.mpCuboidGeometry2D);
}

// explicit instantiation
template class HeuristicLoadBalancer<double>;
}  // namespace iblbm
