#include "SuperGeometryStatistics2D.hpp"

#include <cmath>
#include <iostream>

#include "SuperGeometry2D.hpp"

namespace iblbm
{
template<typename T>
SuperGeometryStatistics2D<T>::SuperGeometryStatistics2D(
    SuperGeometry2D<T>* pSuperGeometry)
  : mpSuperGeometry{pSuperGeometry},
    mNeedUpdate{true},
    mOverlap{pSuperGeometry->GetOverlap()},
    mOstream{std::cout, "SuperGeometryStatistics2D"}
{}

template<typename T>
SuperGeometryStatistics2D<T>::SuperGeometryStatistics2D(
    const SuperGeometryStatistics2D<T>& rRhs)
  : mpSuperGeometry{rRhs.mpSuperGeometry},
    mNeedUpdate{true},
    mOverlap{rRhs.mOverlap},
    mOstream{std::cout, "SuperGeometryStatistics2D"}
{}

template<typename T>
SuperGeometryStatistics2D<T>& SuperGeometryStatistics2D<T>::operator=(
    const SuperGeometryStatistics2D<T>& rRhs)
{
  mpSuperGeometry = rRhs.mpSuperGeometry;
  mNeedUpdate = true;
  mOverlap = rRhs.mOverlap;
  return *this;
}

template<typename T>
void SuperGeometryStatistics2D<T>::Update(bool isVerbose/*=false*/)
{
#ifdef IBLBM_PARALLEL_MPI
  auto really_need_update_global {0};
  if (mNeedUpdate) really_need_update_global = 1;
  // Sum across all process and determine if any process needs update
  MpiManager::Instance().ReduceAndBcast(really_need_update_global, MPI_SUM);
  if (really_need_update_global > 0) mNeedUpdate = true;
#endif  // IBLBM_PARALLEL_MPI
  // Check if update is really needed by going through the block geometries
  // on each process
  if (mNeedUpdate) {
    auto really_need_update {0};
    for (gsl::index local_idx {0};
        local_idx < mpSuperGeometry->rGetLoadBalancer().GetSize();
        ++local_idx) {
      mpSuperGeometry->rGetBlockGeometry(local_idx).rGetStatistics().Update(
          false);
      ++really_need_update;
    }
#ifdef IBLBM_PARALLEL_MPI
    MpiManager::Instance().ReduceAndBcast(really_need_update, MPI_SUM);
#endif  // IBLBM_PARALLEL_MPI
    if (really_need_update == 0) {
      mNeedUpdate = false;
      return;
    }
  }
}

template<typename T>
bool& SuperGeometryStatistics2D<T>::rGetStatus()
{
  return mNeedUpdate;
}

template<typename T>
const bool& SuperGeometryStatistics2D<T>::rGetStatus() const
{
  return mNeedUpdate;
}

template<typename T>
std::size_t SuperGeometryStatistics2D<T>::GetNumMaterial()
{
  Update();
  return mNumMaterials;
}

template<typename T>
std::size_t SuperGeometryStatistics2D<T>::GetNumVoxel(int material)
{
  Update(true);
  return mMaterialToNum[material];
}

template<typename T>
std::size_t SuperGeometryStatistics2D<T>::GetNumVoxel()
{
  Update(true);
  std::size_t total {0};
  for (const auto& r_it : mMaterialToNum) {
    if (r_it.first != 0) total += r_it.second;
  }
  return total;
}

template<typename T>
std::vector<T> SuperGeometryStatistics2D<T>::GetMinPhysR(int material)
{
  Update();
  return mMaterialToMin[material];
}

template<typename T>
std::vector<T> SuperGeometryStatistics2D<T>::GetMaxPhysR(int material)
{
  Update();
  return mMaterialToMax[material];
}

template<typename T>
std::vector<T> SuperGeometryStatistics2D<T>::GetPhysExtent(int material)
{
  Update();
  std::vector<T> phys_extent;
  for (gsl::index d {0}; d < 2; ++d) {
    phys_extent.push_back(mMaterialToMax[material][d] -
        mMaterialToMin[material][d]);
  }
  return phys_extent;
}

template<typename T>
std::vector<T> SuperGeometryStatistics2D<T>::GetPhysRadius(int material)
{
  Update();
  std::vector<T> radius;
  for (gsl::index d {0}; d < 2; ++d) {
    radius.push_back((mMaterialToMax[material][d] -
        mMaterialToMin[material][d]) * 0.5);
  }
  return radius;
}

template<typename T>
std::vector<T> SuperGeometryStatistics2D<T>::GetCenterPhysR(int material)
{
  Update();
  std::vector<T> center;
  for (gsl::index d {0}; d < 2; ++d) {
    center.push_back(mMaterialToMin[material][d] +
        GetPhysRadius(material)[d]);
  }
  return center;
}

template<typename T>
std::vector<int> SuperGeometryStatistics2D<T>::GetType(
    gsl::index globalCuboidIndex
  , gsl::index xIndex
  , gsl::index yIndex)
{
  Update();
  auto local_idx {mpSuperGeometry->rGetLoadBalancer().GetLocalIndex(
      globalCuboidIndex)};
  auto discrete_normal {mpSuperGeometry->rGetExtendedBlockGeometry(
      local_idx).rGetStatistics(false).GetType(xIndex + mOverlap, yIndex +
          mOverlap)};
  return discrete_normal;
}

template<typename T>
std::vector<T> SuperGeometryStatistics2D<T>::ComputeNormal(int material)
{
  Update();

  std::vector<T> normal(2, 0);
  for (gsl::index local_idx {0};
      local_idx < mpSuperGeometry->rGetLoadBalancer().GetSize();
      ++local_idx) {
    for (gsl::index d {0}; d < 2; ++d) {
      if (mpSuperGeometry->rGetBlockGeometry(local_idx).rGetStatistics(false)
          .GetNumVoxel(material) != 0) {
        normal[d] += mpSuperGeometry->rGetBlockGeometry(local_idx)
            .rGetStatistics(false).ComputeNormal(material)[d] *
            mpSuperGeometry->rGetBlockGeometry(local_idx)
                .rGetStatistics(false).GetNumVoxel(material);
      }
    }
  }

#ifdef IBLBM_PARALLEL_MPI
  for (gsl::index d {0}; d < 2; ++d)
      MpiManager::Instance().ReduceAndBcast(normal[d], MPI_SUM);
#endif  // IBLBM_PARALLEL_MPI

  auto num_voxel {GetNumVoxel(material)};
  if (num_voxel != 0) {
    for (gsl::index d {0}; d < 2; ++d) normal[d] /= num_voxel;
  }
  // if no voxels found we expect the normal to be zero
  Ensures(num_voxel || (util::IsNearZero(normal[0]) &&
      util::IsNearZero(normal[1])));

  T norm = sqrt(util::InnerProduct(normal, normal));
  if (norm > 0.0) {
    normal[0] /= norm;
    normal[1] /= norm;
  }
  return normal;
}

template<typename T>
std::vector<int> SuperGeometryStatistics2D<T>::ComputeDiscreteNormal(
    int material
  , T maxNorm/*=1.1*/)
{
  Update();
  auto normal {ComputeNormal(material)};
  std::vector<int> discrete_normal(2, 0);

  // value of cos(theta)
  T smallest_angle {0};
  for (int x {-1}; x <= 1; ++x) {
    for (int y {-1}; y <= 1; ++y) {
      T norm {sqrt(x * x + y * y)};
      if (norm > 0.0 && norm < maxNorm) {
        T angle {(x * normal[0] + y * normal[1]) / norm};
        if (angle >= smallest_angle) {
          smallest_angle = angle;
          discrete_normal[0] = x;
          discrete_normal[1] = y;
        }
      }
    }
  }
  return discrete_normal;
}


template<typename T>
void SuperGeometryStatistics2D<T>::Print()
{
  Update();
  for (const auto& r_it : mMaterialToNum) {
    mOstream << "materialNumber=" << r_it.first <<
        "; count=" << r_it.second <<
        "; minPhysR=(" << mMaterialToMin[r_it.first][0] << ", "<<
            mMaterialToMin[r_it.first][1] << ")" <<
        "; maxPhysR=(" << mMaterialToMax[r_it.first][0] <<", "<<
            mMaterialToMax[r_it.first][1] << ")" << std::endl;
  }
}

// explicit instantiation
template class SuperGeometryStatistics2D<double>;
}  // namespace iblbm
