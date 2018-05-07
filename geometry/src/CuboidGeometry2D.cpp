#include "CuboidGeometry2D.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

#include "AbstractIndicatorFunctor2D.hpp"

namespace iblbm
{
template<typename T>
CuboidGeometry2D<T>::CuboidGeometry2D()
  : mMotherCuboid{T{}, T{}, T{}, 0u, 0u},
    mCuboids{{mMotherCuboid}},
    mIsPeriodic(3, false),
    mOstream{std::cout, "CuboidGeometry2D"}
{
  Split(/*index=*/0, /*numCuboid=*/1u);
}

template<typename T>
CuboidGeometry2D<T>::CuboidGeometry2D(
    T originX
  , T originY
  , T deltaR
  , std::size_t nx
  , std::size_t ny
  , std::size_t nc/*=1*/)
  : mMotherCuboid{originX, originY, deltaR, nx, ny},
    mCuboids{{mMotherCuboid}},
    mIsPeriodic(2, false),
    mOstream{std::cout, "CuboidGeometry2D"}
{
  Split(/*index=*/0, nc);
}

template<typename T>
CuboidGeometry2D<T>::CuboidGeometry2D(
    AbstractIndicatorFunctor2D<T>& rIndicatorFunctor
  , T deltaR
  , std::size_t nc/*=1*/)
  : mMotherCuboid{
        rIndicatorFunctor.rGetMin()[0],
        rIndicatorFunctor.rGetMin()[1],
        deltaR,
        static_cast<std::size_t>(rIndicatorFunctor.GetRange()[0] / deltaR +
            1.5),
        static_cast<std::size_t>(rIndicatorFunctor.GetRange()[1] / deltaR +
            1.5)},
    mCuboids{{mMotherCuboid}},
    mIsPeriodic(2, false),
    mOstream{std::cout, "CuboidGeometry2D"}
{
  Split(/*index=*/0, nc);
  Shrink(rIndicatorFunctor);
}

template<typename T>
void CuboidGeometry2D<T>::Add(Cuboid2D<T> cuboid)
{
  mCuboids.push_back(cuboid);
}

template<typename T>
void CuboidGeometry2D<T>::Remove(gsl::index index)
{
  mCuboids.erase(mCuboids.begin() + index);
}

template<typename T>
void CuboidGeometry2D<T>::Split(
    gsl::index index
  , std::size_t numCuboid)
{
  Cuboid2D<T> tmp {mCuboids[index].GetOrigin(), mCuboids[index].GetDeltaR(),
      mCuboids[index].GetLatticeExtent()};
  tmp.Divide(numCuboid, mCuboids);
  Remove(index);
}

template<typename T>
void CuboidGeometry2D<T>::Shrink(
    AbstractIndicatorFunctor2D<T>& rIndicatorFunctor)
{
  auto nc {GetNumCuboid()};
  for (gsl::index idx = nc - 1; idx >= 0; --idx) {
    std::size_t num_full_cell {0};
    auto nx {mCuboids[idx].GetNx()};
    auto ny {mCuboids[idx].GetNy()};
    auto max_x {std::numeric_limits<gsl::index>::min()};
    auto max_y {std::numeric_limits<gsl::index>::min()};
    auto new_x {std::numeric_limits<gsl::index>::max()};
    auto new_y {std::numeric_limits<gsl::index>::max()};
    // For each node in the child cuboids, check if its physical coordinate
    // is within the bounds of the indicator functor
    for (gsl::index x = 0; x < nx; ++x) {
      for (gsl::index y = 0; y < ny; ++y) {
        auto is_inside {false};
        auto phys_R {GetPhysR(idx, x, y)};
        rIndicatorFunctor(gsl::make_span(&is_inside), phys_R);
        // If the physical coordinate is within the bounds of the indicator
        // functor, update the value of lower left corner coordinate
        // (new_x, new_y) and upper right corner coordinate (max_x, max_y)
        if (is_inside) {
          ++num_full_cell;
          max_x = std::max(max_x, x);
          max_y = std::max(max_y, y);
          new_x = std::min(new_x, x);
          new_y = std::min(new_y, y);
        }
      }
    }
    // If some nodes are within the bounds of the indicator functor, resize
    // child cuboid to contain only those nodes. Discard cuboid is none of
    // its nodes are within bounds
    if (num_full_cell > 0) {
      mCuboids[idx].SetWeight(num_full_cell);
      mCuboids[idx].Resize(new_x, new_y, max_x - new_x + 1, max_y - new_y + 1);
    }
    else {
      Remove(idx);
    }
  }
  // Shrink mother cuboid
  auto min_phys_R {GetMinPhysR()};
  auto max_phys_R {GetMaxPhysR()};
  auto min_delta_R {GetMinDeltaR()};
  mMotherCuboid = Cuboid2D<T>(min_phys_R[0], min_phys_R[1], min_delta_R,
      static_cast<std::size_t>((max_phys_R[0] - min_phys_R[0]) /
          min_delta_R + 0.5),
      static_cast<std::size_t>((max_phys_R[1] - min_phys_R[1]) /
          min_delta_R + 0.5));
}

template<typename T>
void CuboidGeometry2D<T>::RefineArea(
    T xPosition0
  , T yPosition0
  , T xPosition1
  , T yPosition1
  , int refinementLevel)
{
  for (gsl::index i {0}; i < GetNumCuboid(); ++i) {
    // Ignore cuboids if they aren't at our target refinementLevel
    if (rGetCuboid(i).GetRefinementLevel() != refinementLevel) continue;

    // Ignore cuboid if they don't intersect our target area
    gsl::index x_index_0 {0};
    gsl::index y_index_0 {0};
    gsl::index x_index_1 {0};
    gsl::index y_index_1 {0};
    if (!rGetCuboid(i).CheckIntersection(xPosition0, yPosition0, xPosition1,
        yPosition1, x_index_0, y_index_0, x_index_1, y_index_1)) {
      continue;
    }

    auto x_position {rGetCuboid(i).GetGlobalXPosition()};
    auto y_position {rGetCuboid(i).GetGlobalYPosition()};
    auto delta_R {rGetCuboid(i).GetDeltaR()};
    auto nx {rGetCuboid(i).GetNx()};
    auto ny {rGetCuboid(i).GetNy()};

    // Split the non-intersected area into several cuboid to maintain their
    // refinement level
    // If the intersection area does not exceed the left edge of the current
    // cuboid, add the entire non-intersection left area
    if (x_index_0 != 0) {
      Cuboid2D<T> left {x_position, y_position, delta_R,
          static_cast<std::size_t>(x_index_0), ny, refinementLevel};
      Add(left);
    }

    // If the intersection area does not exceed the lower edge of the current
    // cuboid, add the entire non-intersection lower area or to the right of
    // previously added left area
    if (y_index_0 != 0) {
      Cuboid2D<T> lower {x_position + static_cast<T>(x_index_0) *
          delta_R, y_position, delta_R, nx - x_index_0,
          static_cast<std::size_t>(y_index_0), refinementLevel};
      Add(lower);
    }

    // If the intersection area does not exceed the right edge of the current
    // cuboid, add the entire non-intersected right area or above previously
    // added lower area
    if (x_index_1 != rGetCuboid(i).GetNx() - 1) {
      Cuboid2D<T> right {x_position + static_cast<T>(x_index_1 + 1) *
          delta_R, y_position + static_cast<T>(y_index_0) * delta_R, delta_R,
          nx - x_index_1 - 1, ny - y_index_0, refinementLevel};
      Add(right);
    }

    // If the intersection area does not exceed the upper edge of the current
    // cuboid, add the entire non-intersected top area or to the left of
    // previously added right area
    if (y_index_1 != rGetCuboid(i).GetNy() - 1) {
      Cuboid2D<T> upper {x_position + static_cast<T>(x_index_0) * delta_R,
          y_position + static_cast<T>(y_index_1 + 1) * delta_R, delta_R,
          static_cast<std::size_t>(x_index_1 - x_index_0 + 1),
          ny - y_index_1 - 1, refinementLevel};
      Add(upper);
    }
    // Shrink the current cuboid to the intersected area and increase its
    // refinement by 1
    rGetCuboid(i).Initialize(x_position + static_cast<T>(x_index_0) *
        delta_R, y_position + static_cast<T>(y_index_0) * delta_R, delta_R,
        x_index_1 - x_index_0 + 1, y_index_1 - y_index_0 + 1,
        refinementLevel);
    rGetCuboid(i).RefineIncrease();
  }
}

template<typename T>
bool CuboidGeometry2D<T>::HasCuboid(
    const Vector2D<T>& rPhysR
  , gsl::index& rGlobalCuboidIndex)
{
  auto tmp_index {GetGlobalCuboidIndex(rPhysR[0], rPhysR[1])};
  if (tmp_index != -1) {
    rGlobalCuboidIndex = tmp_index;
    return true;
  }
  else {
    return false;
  }
}

template<typename T>
bool CuboidGeometry2D<T>::HasCuboid(
    const std::vector<T>& rPhysR
  , gsl::index& rGlobalCuboidIndex) const
{
  auto tmp_index {GetGlobalCuboidIndex(rPhysR[0], rPhysR[1])};
  if (tmp_index != -1) {
    rGlobalCuboidIndex = tmp_index;
    return true;
  }
  else {
    return false;
  }
}

template<typename T>
Cuboid2D<T>& CuboidGeometry2D<T>::rGetCuboid(gsl::index i)
{
  Expects(i < mCuboids.size());
  return mCuboids[i];
}

template<typename T>
const Cuboid2D<T>& CuboidGeometry2D<T>::rGetCuboid(gsl::index i) const
{
  Expects(i < mCuboids.size());
  return mCuboids[i];
}

template<typename T>
bool CuboidGeometry2D<T>::GetLatticeR(
    const std::vector<T>& rPhysR
  , gsl::index& rGlobalCuboidIndex
  , std::vector<gsl::index>& rLatticeR) const
{
  return GetLatticeR(rPhysR.data(), rGlobalCuboidIndex, rLatticeR.data());
}

template<typename T>
bool CuboidGeometry2D<T>::GetLatticeR(
    const Vector2D<T>& rPhysR
  , gsl::index& rGlobalCuboidIndex
  , Vector2D<gsl::index>& rLatticeR) const
{
  auto tmp_index {GetGlobalCuboidIndex(rPhysR[0], rPhysR[1])};
  if (tmp_index < GetNumCuboid()) {
    rGlobalCuboidIndex = tmp_index;
    rLatticeR[0] = static_cast<gsl::index>((rPhysR[0] -
        mCuboids[tmp_index].GetOrigin()[0]) /
        mCuboids[tmp_index].GetDeltaR() + 0.5);
    rLatticeR[1] = static_cast<gsl::index>((rPhysR[1] -
        mCuboids[tmp_index].GetOrigin()[1]) /
        mCuboids[tmp_index].GetDeltaR() + 0.5);
    return true;
  }
  else {
    return false;
  }
}

template<typename T>
bool CuboidGeometry2D<T>::GetLatticeR(
    const T physR[]
  , gsl::index& rGlobalCuboidIndex
  , gsl::index latticeR[]) const
{
  auto tmp_index {GetGlobalCuboidIndex(physR[0], physR[1])};
  if (tmp_index < GetNumCuboid()) {
    rGlobalCuboidIndex = tmp_index;
    latticeR[0] = static_cast<gsl::index>((physR[0] -
        mCuboids[tmp_index].GetOrigin()[0]) /
        mCuboids[tmp_index].GetDeltaR() + 0.5);
    latticeR[1] = static_cast<gsl::index>((physR[1] -
        mCuboids[tmp_index].GetOrigin()[1]) /
        mCuboids[tmp_index].GetDeltaR() + 0.5);
    return true;
  }
  else {
    return false;
  }
}

template<typename T>
bool CuboidGeometry2D<T>::GetFloorLatticeR(
    const std::vector<T>& rPhysR
  , gsl::index& rGlobalCuboidIndex
  , std::vector<gsl::index>& rLatticeR) const
{
  auto tmp_index {GetGlobalCuboidIndex(rPhysR[0], rPhysR[1])};
  if (tmp_index < GetNumCuboid()) {
    rGlobalCuboidIndex = tmp_index;
    rLatticeR[0] = static_cast<gsl::index>(floor((rPhysR[0] -
        mCuboids[tmp_index].GetOrigin()[0]) /
        mCuboids[tmp_index].GetDeltaR()));
    rLatticeR[1] = static_cast<gsl::index>(floor((rPhysR[1] -
        mCuboids[tmp_index].GetOrigin()[1]) /
        mCuboids[tmp_index].GetDeltaR()));
    return true;
  }
  else {
    return false;
  }
}

template<typename T>
bool CuboidGeometry2D<T>::GetFloorLatticeR(
    const Vector2D<T>& rPhysR
  , gsl::index& rGlobalCuboidIndex
  , Vector2D<gsl::index>& rLatticeR) const
{
  auto tmp_index {GetGlobalCuboidIndex(rPhysR[0], rPhysR[1])};
  if (tmp_index < GetNumCuboid()) {
    rGlobalCuboidIndex = tmp_index;
    rLatticeR[0] = static_cast<gsl::index>(floor((rPhysR[0] -
        mCuboids[tmp_index].GetOrigin()[0]) /
        mCuboids[tmp_index].GetDeltaR()));
    rLatticeR[1] = static_cast<gsl::index>(floor((rPhysR[1] -
        mCuboids[tmp_index].GetOrigin()[1]) /
        mCuboids[tmp_index].GetDeltaR()));
    return true;
  }
  else {
    return false;
  }
}

template<typename T>
Vector2D<T> CuboidGeometry2D<T>::GetPhysR(
    gsl::index globalCuboidIndex
  , gsl::index xIndex
  , gsl::index yIndex) const
{
  const auto delta_R {mMotherCuboid.GetDeltaR()};
  const auto lattice_extent {mMotherCuboid.GetLatticeExtent()};
  auto phys_R {mCuboids[globalCuboidIndex].GetPhysR(xIndex, yIndex)};
  for (gsl::index d = 0; d < 2; ++d) {
    if (mIsPeriodic[d]) {
      phys_R[d] = fmod(phys_R[d] - mMotherCuboid.GetOrigin()[d] +
          delta_R * lattice_extent[d], delta_R * lattice_extent[d]);
      // solving the rounding error problem for double
      if (phys_R[d] * phys_R[d] < 1e-3 * delta_R * delta_R) phys_R[d] = T{};
      // add origin
      phys_R[d] += mMotherCuboid.GetOrigin()[d];
    }
  }
  return phys_R;
}

template<typename T>
Vector2D<T> CuboidGeometry2D<T>::GetPhysR(
    gsl::index globalCuboidIndex
  , const std::vector<gsl::index>& rLatticeR) const
{
  return GetPhysR(globalCuboidIndex, rLatticeR[0], rLatticeR[1]);
}

template<typename T>
void CuboidGeometry2D<T>::GetPhysR(
    gsl::index globalCuboidIndex
  , const gsl::index latticeR[2]
  , T physR[2]) const
{
  GetPhysR(globalCuboidIndex, latticeR[0], latticeR[1], physR);
}

template<typename T>
void CuboidGeometry2D<T>::GetPhysR(
    const gsl::index globalCuboidIndex
  , const gsl::index xIndex
  , const gsl::index yIndex
  , T physR[2]) const
{
  mCuboids[globalCuboidIndex].GetPhysR(xIndex, yIndex, physR);
  const auto delta_R {mMotherCuboid.GetDeltaR()};
  const auto lattice_extent {mMotherCuboid.GetLatticeExtent()};
  for (gsl::index d = 0; d < 2; ++d) {
    if (mIsPeriodic[d]) {
      physR[d] = fmod(physR[d] - mMotherCuboid.GetOrigin()[d] +
          delta_R * lattice_extent[d], delta_R * lattice_extent[d]);
      // solving the rounding error problem for double
      if (physR[d] * physR[d] < 1e-3 * delta_R * delta_R) physR[d] = T{};
      // add origin
      physR[d] += mMotherCuboid.GetOrigin()[d];
    }
  }
}

template<typename T>
Vector2D<T> CuboidGeometry2D<T>::GetMinPhysR() const
{
  auto output {mCuboids[0].GetOrigin()};
  for (gsl::index i = 0; i < mCuboids.size(); ++i) {
    if (mCuboids[i].GetOrigin()[0] < output[0])
        output[0] = mCuboids[i].GetOrigin()[0];
    if (mCuboids[i].GetOrigin()[1] < output[1])
        output[1] = mCuboids[i].GetOrigin()[1];
  }
  return output;
}

template<typename T>
Vector2D<T> CuboidGeometry2D<T>::GetMaxPhysR() const
{
  auto output {mCuboids[0].GetOrigin()};
  output[0] += mCuboids[0].GetNx() * mCuboids[0].GetDeltaR();
  output[1] += mCuboids[0].GetNy() * mCuboids[0].GetDeltaR();
  for (gsl::index i = 0; i < mCuboids.size(); ++i) {
    auto x {mCuboids[i].GetOrigin()[0] + mCuboids[i].GetDeltaR() *
        mCuboids[i].GetNx()};
    auto y {mCuboids[i].GetOrigin()[1] + mCuboids[i].GetDeltaR() *
        mCuboids[i].GetNy()};
    if (x > output[0]) output[0] = x;
    if (y > output[1]) output[1] = y;
  }
  return output;
}

template<typename T>
std::size_t CuboidGeometry2D<T>::GetNumCuboid() const
{
  return mCuboids.size();
}

template<typename T>
T CuboidGeometry2D<T>::GetMinRatio() const
{
  T min_ratio {1};
  for (gsl::index i {0}; i < mCuboids.size(); ++i) {
    if (static_cast<T>(mCuboids[i].GetNx()) /
        static_cast<T>(mCuboids[i].GetNy()) < min_ratio) {
      min_ratio = static_cast<T>(mCuboids[i].GetNx()) /
          static_cast<T>(mCuboids[i].GetNy());
    }
  }
  return min_ratio;
}

template<typename T>
T CuboidGeometry2D<T>::GetMaxRatio() const
{
  T max_ratio {1};
  for (gsl::index i {0}; i < mCuboids.size(); ++i) {
    if (static_cast<T>(mCuboids[i].GetNx()) /
        static_cast<T>(mCuboids[i].GetNy()) > max_ratio) {
      max_ratio = static_cast<T>(mCuboids[i].GetNx()) /
          static_cast<T>(mCuboids[i].GetNy());
    }
  }
  return max_ratio;
}

template<typename T>
T CuboidGeometry2D<T>::GetMinPhysVolume() const
{
  T min_volume {mCuboids[0].GetPhysVolume()};
  for (gsl::index i {0}; i < mCuboids.size(); ++i) {
    if (mCuboids[i].GetPhysVolume() < min_volume)
        min_volume = mCuboids[i].GetPhysVolume();
  }
  return min_volume;
}

template<typename T>
T CuboidGeometry2D<T>::GetMaxPhysVolume() const
{
  T max_volume {mCuboids[0].GetPhysVolume()};
  for (gsl::index i {0}; i < mCuboids.size(); ++i) {
    if (mCuboids[i].GetPhysVolume() > max_volume)
        max_volume = mCuboids[i].GetPhysVolume();
  }
  return max_volume;
}

template<typename T>
std::size_t CuboidGeometry2D<T>::GetMinLatticeVolume() const
{
  std::size_t min_nodes {mCuboids[0].GetLatticeVolume()};
  for (gsl::index i {0}; i < mCuboids.size(); ++i) {
    if (mCuboids[i].GetLatticeVolume() < min_nodes)
        min_nodes = mCuboids[i].GetLatticeVolume();
  }
  return min_nodes;
}

template<typename T>
std::size_t CuboidGeometry2D<T>::GetMaxLatticeVolume() const
{
  std::size_t max_nodes {mCuboids[0].GetLatticeVolume()};
  for (gsl::index i {0}; i < mCuboids.size(); ++i) {
    if (mCuboids[i].GetLatticeVolume() > max_nodes)
        max_nodes = mCuboids[i].GetLatticeVolume();
  }
  return max_nodes;
}

template<typename T>
T CuboidGeometry2D<T>::GetMinDeltaR() const
{
  T min_delta {mCuboids[0].GetDeltaR()};
  for (gsl::index i {0}; i < mCuboids.size(); ++i) {
    if (mCuboids[i].GetDeltaR() < min_delta)
        min_delta = mCuboids[i].GetDeltaR();
  }
  return min_delta;
}

template<typename T>
T CuboidGeometry2D<T>::GetMaxDeltaR() const
{
  T max_delta {mCuboids[0].GetDeltaR()};
  for (gsl::index i {0}; i < mCuboids.size(); ++i) {
    if (mCuboids[i].GetDeltaR() > max_delta)
        max_delta = mCuboids[i].GetDeltaR();
  }
  return max_delta;
}

template<typename T>
Cuboid2D<T> CuboidGeometry2D<T>::GetMotherCuboid() const
{
  return mMotherCuboid;
}

template<typename T>
gsl::index CuboidGeometry2D<T>::GetGlobalCuboidIndex(
    T xPosition
  , T yPosition
  , std::size_t offset/*=0*/) const
{
  for (gsl::index i {0}; i < mCuboids.size(); ++i) {
    if (mCuboids[i].ContainPoint(xPosition, yPosition, offset)) return i;
  }
  return -1;
}

template<typename T>
void CuboidGeometry2D<T>::SetIsPeriodic(
    bool isPeriodicX
  , bool isPeriodicY)
{
  mIsPeriodic[0] = isPeriodicX;
  mIsPeriodic[1] = isPeriodicY;
}

template<typename T>
void CuboidGeometry2D<T>::Print() const
{
  mOstream << "===== Cuboid Structure Statistics =====" << std::endl <<
      " Parameter         | Value" << std::endl <<
      " Number of Cuboids | " << GetNumCuboid() << std::endl <<
      " Delta (min)       | " << GetMinDeltaR() << std::endl <<
      " Delta (max)       | " << GetMaxDeltaR() << std::endl <<
      " Ratio (min)       | " << GetMinRatio() << std::endl <<
      " Ratio (max)       | " << GetMaxRatio() << std::endl <<
      " Nodes (min)       | " << GetMinLatticeVolume() << std::endl <<
      " Nodes (max)       | " << GetMaxLatticeVolume() << std::endl <<
      "--------------------" << std::endl;
}

template<typename T>
void CuboidGeometry2D<T>::PrintExtended()
{
  mOstream << "Mothercuboid :" << std::endl;
  GetMotherCuboid().Print();

  for (gsl::index i {0}; i < GetNumCuboid(); ++i) {
    mOstream << "Cuboid #" << i << ":" << std::endl;
    rGetCuboid(i).Print();
  }
}

// Explicit instantiation
template class CuboidGeometry2D<double>;
}  // namespace iblbm
