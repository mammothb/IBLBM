#include "SuperGeometry2D.hpp"

namespace iblbm
{
template<typename T>
SuperGeometry2D<T>::SuperGeometry2D(
    CuboidGeometry2D<T>& rCuboidGeometry
  , LoadBalancer<T>& rLoadBalancer
  , std::size_t overlap/*=2*/)
  : SuperStructure2D<T>{rCuboidGeometry, rLoadBalancer, overlap},
    mStatistics{this},
    mOstream{std::cout, "SuperGeometry2D"}
{
  this->mCommunicator.InitializeNeighborhood();
  this->mCommunicator.AddCells(this->mOverlap);
  this->mCommunicator.Initialize();
  this->mNeedCommunication = true;

  for (gsl::index local_idx {0};
      local_idx < this->rGetLoadBalancer().GetSize(); ++local_idx) {
    auto global_idx {this->rGetLoadBalancer().GetGlobalIndex(local_idx)};
    Cuboid2D<T> cuboid {rCuboidGeometry.rGetCuboid(global_idx), overlap};
    BlockGeometry2D<T> block_geometry {cuboid, global_idx};
    mExtendedBlockGeometries.push_back(block_geometry);
  }

  for (gsl::index local_idx {0};
      local_idx < this->rGetLoadBalancer().GetSize(); ++local_idx) {
    auto global_idx {this->rGetLoadBalancer().GetGlobalIndex(local_idx)};
    auto nx {rCuboidGeometry.rGetCuboid(global_idx).GetNx()};
    auto ny {rCuboidGeometry.rGetCuboid(global_idx).GetNy()};
    BlockGeometryView2D<T> block_geometry_view {
        mExtendedBlockGeometries[local_idx],
        static_cast<gsl::index>(overlap), static_cast<gsl::index>(overlap),
        static_cast<gsl::index>(overlap + nx - 1),
        static_cast<gsl::index>(overlap + ny - 1)};
    mBlockGeometries.push_back(block_geometry_view);
  }
  mStatistics.rGetStatus() = true;
}

template<typename T>
SuperGeometry2D<T>::SuperGeometry2D(const SuperGeometry2D<T>& rRhs)
  : SuperStructure2D<T>{rRhs.mrCuboidGeometry, rRhs.mrLoadBalancer,
        rRhs.mOverlap},
    mExtendedBlockGeometries{rRhs.mExtendedBlockGeometries},
    mBlockGeometries{rRhs.mBlockGeometries},
    mStatistics{this},
    mOstream{std::cout, "SuperGeometry2D"}
{
  this->mCommunicator.InitializeNeighborhood();
  this->mCommunicator.AddCells(this->mOverlap);
  this->mCommunicator.Initialize();
  this->mNeedCommunication = true;

  mStatistics.rGetStatus() = true;
}

template<typename T>
SuperGeometry2D<T>& SuperGeometry2D<T>::operator=(
    const SuperGeometry2D<T>& rRhs)
{
  this->mCommunicator.InitializeNeighborhood();
  this->mCommunicator.AddCells(this->mOverlap);
  this->mCommunicator.Initialize();
  this->mNeedCommunication = true;

  this->mrCuboidGeometry = rRhs.mrCuboidGeometry;
  this->mrLoadBalancer = rRhs.mrLoadBalancer;
  this->mOverlap = rRhs.mOverlap;

  mExtendedBlockGeometries = rRhs.mExtendedBlockGeometries;
  mBlockGeometries = rRhs.mBlockGeometries;
  mStatistics = SuperGeometryStatistics2D<T>(this);

  return *this;
}

template<typename T>
bool* SuperGeometry2D<T>::operator()(
    gsl::index localCuboidIndex
  , gsl::index xIndex
  , gsl::index yIndex
  , gsl::index /*dataIndex*/)
{
  return (bool*)&rGetExtendedBlockGeometry(localCuboidIndex)
      .rGetMaterial(xIndex + this->mOverlap, yIndex + this->mOverlap);
}

template<typename T>
void SuperGeometry2D<T>::UpdateStatistics(bool isVerbose/*=false*/)
{
  if (this->mNeedCommunication) {
    this->Communicate(isVerbose);
    rGetStatisticsStatus() = true;
  }
  mStatistics.Update(isVerbose);
  for (gsl::index i {0}; i < mExtendedBlockGeometries.size(); ++i)
      mExtendedBlockGeometries[i].rGetStatistics().Update(isVerbose);
}

template<typename T>
std::size_t SuperGeometry2D<T>::Clean(bool isVerbose/*=true*/)
{
  this->Communicate();
  std::size_t counter {0};
  for (gsl::index i {0}; i < mExtendedBlockGeometries.size(); ++i)
      counter += mExtendedBlockGeometries[i].Clean(false);
#ifdef PARALLEL_MODE_MPI
  MpiManager::Instance().ReduceAndBcast(counter, MPI_SUM);
#endif

  if (isVerbose) {
    mOstream << "cleaned "<< counter << " outer boundary voxel(s)" <<
        std::endl;
  }
  if (counter > 0) {
    mStatistics.rGetStatus() = true;
    this->mNeedCommunication = true;
  }
  return counter;
}

template<typename T>
std::size_t SuperGeometry2D<T>::OuterClean(bool isVerbose/*=true*/)
{
  this->Communicate();
  std::size_t counter {0};
  for (gsl::index i {0}; i < mExtendedBlockGeometries.size(); ++i)
      counter += mExtendedBlockGeometries[i].OuterClean(false);
#ifdef PARALLEL_MODE_MPI
  MpiManager::Instance().ReduceAndBcast(counter, MPI_SUM);
#endif

  if (isVerbose) {
    mOstream << "cleaned "<< counter << " outer fluid voxel(s)" << std::endl;
  }
  if (counter > 0) {
    mStatistics.rGetStatus() = true;
    this->mNeedCommunication = true;
  }
  return counter;
}

template<typename T>
std::size_t SuperGeometry2D<T>::InnerClean(bool isVerbose/*=true*/)
{
  this->Communicate();
  std::size_t counter {0};
  for (gsl::index i {0}; i < mExtendedBlockGeometries.size(); ++i)
      counter += mExtendedBlockGeometries[i].InnerClean(false);
#ifdef PARALLEL_MODE_MPI
  MpiManager::Instance().Barrier();
  MpiManager::Instance().ReduceAndBcast(counter, MPI_SUM);
#endif

  if (isVerbose) {
    mOstream << "cleaned "<< counter << " inner boundary voxel(s)" <<
        std::endl;
  }
  if (counter > 0) {
    mStatistics.rGetStatus() = true;
    this->mNeedCommunication = true;
  }
  return counter;
}

template<typename T>
std::size_t SuperGeometry2D<T>::InnerClean(
    int boundaryMaterial
  , bool isVerbose/*=true*/)
{
  this->Communicate();
  std::size_t counter {0};
  for (gsl::index i {0}; i < mExtendedBlockGeometries.size(); ++i) {
    counter += mExtendedBlockGeometries[i].InnerClean(boundaryMaterial,
        false);
  }
#ifdef PARALLEL_MODE_MPI
  MpiManager::Instance().ReduceAndBcast(counter, MPI_SUM);
#endif

  if (isVerbose) {
    mOstream << "cleaned "<< counter << " inner boundary voxel(s) of "
        "Type " << boundaryMaterial << std::endl;
  }
  if (counter > 0) {
    mStatistics.rGetStatus() = true;
    this->mNeedCommunication = true;
  }
  return counter;
}

template<typename T>
bool SuperGeometry2D<T>::CheckForErrors(bool isVerbose/*=true*/)
{
  this->Communicate();
  bool error {false};
  for (gsl::index i {0}; i < mBlockGeometries.size(); ++i) {
    if (mBlockGeometries[i].CheckForErrors(false)) {
      error = true;
      break;
    }
  }
  if (isVerbose) {
    if (error) {
      mOstream << "error!" << std::endl;
    }
    else {
      mOstream << "the model is correct!" << std::endl;
    }
  }
  return error;
}

template<typename T>
void SuperGeometry2D<T>::Rename(
    int fromMaterial
  , int toMaterial)
{
  this->Communicate();
  for (gsl::index i {0}; i < mExtendedBlockGeometries.size(); ++i)
      mExtendedBlockGeometries[i].Rename(fromMaterial, toMaterial);
  mStatistics.rGetStatus() = true;
  this->mNeedCommunication = true;
}

template<typename T>
void SuperGeometry2D<T>::Rename(
    int fromMaterial
  , int toMaterial
  , AbstractIndicatorFunctor2D<T>& rCondition)
{
  this->Communicate();
  // call Identity to prevent deleting
  IndicatorIdentity2D<T> tmp {rCondition};

  for (gsl::index i {0}; i < mExtendedBlockGeometries.size(); ++i) {
    mExtendedBlockGeometries[i].Rename(fromMaterial, toMaterial, rCondition);
  }
  mStatistics.rGetStatus() = true;
}

template<typename T>
void SuperGeometry2D<T>::Rename(
    int fromMaterial
  , int toMaterial
  , unsigned xOffset
  , unsigned yOffset)
{
  if (xOffset <= this->mOverlap && yOffset <= this->mOverlap) {
    this->Communicate();
    for (gsl::index i {0}; i < mBlockGeometries.size(); ++i) {
      mBlockGeometries[i].Rename(fromMaterial, toMaterial, xOffset,
          yOffset);
    }
    mStatistics.rGetStatus() = true;
    this->mNeedCommunication = true;
  }
  else {
    mOstream << "Error: rename only implemented for offset <= overlap" <<
        std::endl;
  }
}

template<typename T>
void SuperGeometry2D<T>::Rename(
    int fromMaterial
  , int toMaterial
  , int testMaterial
  , const std::vector<int>& rTestDirection)
{
  if (rTestDirection[0] * rTestDirection[0] <=
      this->mOverlap * this->mOverlap &&
      rTestDirection[1] * rTestDirection[1] <=
      this->mOverlap * this->mOverlap) {
    this->Communicate();
    for (gsl::index i {0}; i < mBlockGeometries.size(); ++i) {
      mBlockGeometries[i].Rename(fromMaterial, toMaterial, testMaterial,
          rTestDirection);
    }
    mStatistics.rGetStatus() = true;
    this->mNeedCommunication = true;
  }
  else {
    mOstream << "Error: rename only implemented for "
        "|testDirection[i]| <= overlap" << std::endl;
  }
}

template<typename T>
void SuperGeometry2D<T>::Rename(
    int fromBoundaryMaterial
  , int toBoundaryMaterial
  , int fluidMaterial
  , AbstractIndicatorFunctor2D<T>& rCondition)
{
  if (this->mOverlap >= 2) {
    this->Communicate();
    // call Identity to prevent deleting
    IndicatorIdentity2D<T> tmp {rCondition};
    Rename(fromBoundaryMaterial, toBoundaryMaterial, rCondition);
    auto test_direction {this->rGetStatistics().ComputeDiscreteNormal(
        toBoundaryMaterial)};

    this->Communicate();
    for (gsl::index i {0}; i < mBlockGeometries.size(); ++i) {
      mBlockGeometries[i].Rename(fromBoundaryMaterial, toBoundaryMaterial,
          fluidMaterial, rCondition, test_direction);
    }
    mStatistics.rGetStatus() = true;
    this->mNeedCommunication = true;
  }
  else {
    mOstream << "Error: rename only implemented for overlap >= 2" <<
        std::endl;
  }
}

template<typename T>
std::size_t SuperGeometry2D<T>::GetDataSize() const
{
  return 1;
}

template<typename T>
std::size_t SuperGeometry2D<T>::GetSizeofDataType() const
{
  return sizeof(std::size_t);
}

template<typename T>
int& SuperGeometry2D<T>::rGetMaterial(
    gsl::index globalCuboidIndex
  , gsl::index xIndex
  , gsl::index yIndex)
{
  if (this->rGetLoadBalancer().GetRank(globalCuboidIndex) !=
      MpiManager::Instance().GetRank()) {
    TERMINATE("Write access to data which is not available in the any block "
        "geometry");
  }
  this->mNeedCommunication = true;
  mStatistics.rGetStatus() = true;
  return mExtendedBlockGeometries[this->rGetLoadBalancer().GetLocalIndex(
      globalCuboidIndex)].rGetMaterial(xIndex + this->mOverlap, yIndex +
          this->mOverlap);
}

template<typename T>
const int& SuperGeometry2D<T>::rGetMaterial(
    gsl::index globalCuboidIndex
  , gsl::index xIndex
  , gsl::index yIndex) const
{
  if (this->rGetLoadBalancer().GetRank(globalCuboidIndex) !=
      MpiManager::Instance().GetRank()) {
    TERMINATE("Write access to data which is not available in the any block "
        "geometry");
  }
  return mExtendedBlockGeometries[this->rGetLoadBalancer().GetLocalIndex(
      globalCuboidIndex)].rGetMaterial(xIndex + this->mOverlap, yIndex +
          this->mOverlap);
}

template<typename T>
int& SuperGeometry2D<T>::rGetMaterial(
    gsl::index globalCuboidIndex
  , const std::vector<gsl::index> rLatticeR)
{
  return rGetMaterial(globalCuboidIndex, rLatticeR[0], rLatticeR[1]);
}

template<typename T>
const int& SuperGeometry2D<T>::rGetMaterial(
    gsl::index globalCuboidIndex
  , const std::vector<gsl::index> rLatticeR) const
{
  return rGetMaterial(globalCuboidIndex, rLatticeR[0], rLatticeR[1]);
}

template<typename T>
int SuperGeometry2D<T>::GetMaterialAndCommunicate(
    gsl::index globalCuboidIndex
  , const std::vector<gsl::index> rLatticeR) const
{
  return GetMaterialAndCommunicate(globalCuboidIndex, rLatticeR[0],
      rLatticeR[1]);
}

template<typename T>
int SuperGeometry2D<T>::GetMaterialAndCommunicate(
    gsl::index globalCuboidIndex
  , gsl::index xIndex
  , gsl::index yIndex) const
{
  int material {0};
  if (this->rGetLoadBalancer().GetRank(globalCuboidIndex) ==
      MpiManager::Instance().GetRank()) {
    material = mExtendedBlockGeometries[this->rGetLoadBalancer()
        .GetLocalIndex(globalCuboidIndex)].rGetMaterial(xIndex +
            this->mOverlap, yIndex + this->mOverlap);
  }
#ifdef IBLBM_PARALLEL_MPI
  MpiManager::Instance().Bcast(&material, 1,
      this->rGetLoadBalancer().GetRank(globalCuboidIndex));
#endif
  return material;
}

template<typename T>
Vector2D<T> SuperGeometry2D<T>::GetPhysR(
    gsl::index globalCuboidIndex
  , gsl::index xIndex
  , gsl::index yIndex) const
{
  return this->mrCuboidGeometry.GetPhysR(globalCuboidIndex, xIndex, yIndex);
}

template<typename T>
Vector2D<T> SuperGeometry2D<T>::GetPhysR(
    gsl::index globalCuboidIndex
  , const std::vector<gsl::index>& rLatticeR) const
{
  return this->mrCuboidGeometry.GetPhysR(globalCuboidIndex, rLatticeR);
}

template<typename T>
void SuperGeometry2D<T>::GetPhysR(
    gsl::index globalCuboidIndex
  , const gsl::index latticeR[2]
  , T physR[2]) const
{
  this->mrCuboidGeometry.GetPhysR(globalCuboidIndex, latticeR, physR);
}

template<typename T>
void SuperGeometry2D<T>::GetPhysR(
    const gsl::index globalCuboidIndex
  , const gsl::index xIndex
  , const gsl::index yIndex
  , T physR[2]) const
{
  this->mrCuboidGeometry.GetPhysR(globalCuboidIndex, xIndex, yIndex, physR);
}

template<typename T>
BlockGeometry2D<T>& SuperGeometry2D<T>::rGetExtendedBlockGeometry(
    gsl::index localCuboidIndex)
{
  mStatistics.rGetStatus() = true;
  return mExtendedBlockGeometries[localCuboidIndex];
}

template<typename T>
const BlockGeometry2D<T>& SuperGeometry2D<T>::rGetExtendedBlockGeometry(
    gsl::index localCuboidIndex) const
{
  return mExtendedBlockGeometries[localCuboidIndex];
}

template<typename T>
BlockGeometryView2D<T>& SuperGeometry2D<T>::rGetBlockGeometry(
    gsl::index localCuboidIndex)
{
  mStatistics.rGetStatus() = true;
  return mBlockGeometries[localCuboidIndex];
}

template<typename T>
const BlockGeometryView2D<T>& SuperGeometry2D<T>::rGetBlockGeometry(
    gsl::index localCuboidIndex) const
{
  return mBlockGeometries[localCuboidIndex];
}

template<typename T>
SuperGeometryStatistics2D<T>& SuperGeometry2D<T>::rGetStatistics()
{
  if (this->mNeedCommunication) {
    this->Communicate();
    rGetStatisticsStatus() = true;
  }
  return mStatistics;
}

template<typename T>
bool& SuperGeometry2D<T>::rGetStatisticsStatus()
{
  return mStatistics.rGetStatus();
}

template<typename T>
const bool& SuperGeometry2D<T>::rGetStatisticsStatus() const
{
  return mStatistics.rGetStatus();
}

template<typename T>
void SuperGeometry2D<T>::Print()
{
  this->mrCuboidGeometry.Print();
  rGetStatistics().Print();
}


// explicit instantiation
template class SuperGeometry2D<double>;
}  // namespace iblbm
