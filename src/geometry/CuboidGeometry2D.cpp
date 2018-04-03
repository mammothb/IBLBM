#include "CuboidGeometry2D.hpp"

#include <cmath>
#include <iostream>

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
  Split(/*index=*/0, /*p=*/1u);
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
        static_cast<std::size_t>((rIndicatorFunctor.rGetMax()[0] -
            rIndicatorFunctor.rGetMin()[0]) / deltaR + 1.5),
        static_cast<std::size_t>((rIndicatorFunctor.rGetMax()[1] -
            rIndicatorFunctor.rGetMin()[1]) / deltaR + 1.5)},
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
  , std::size_t p)
{
  Cuboid2D<T> temp(mCuboids[index].GetGlobalXPosition(),
      mCuboids[index].GetGlobalYPosition(), mCuboids[index].GetDeltaR(),
      mCuboids[index].GetNx(), mCuboids[index].GetNy());
  temp.Divide(p, mCuboids);
  Remove(index);
}

template<typename T>
void CuboidGeometry2D<T>::Shrink(
    AbstractIndicatorFunctor2D<T>& rIndicatorFunctor)
{
  auto nc {GetNumberOfCuboids()};
  for (gsl::index idx = nc - 1; idx >= 0; --idx) {
    std::size_t num_full_cell {};
    auto nx = mCuboids[idx].GetNx();
    auto ny = mCuboids[idx].GetNy();
    auto max_x {std::numeric_limits<gsl::index>::min()};
    auto max_y {std::numeric_limits<gsl::index>::min()};
    auto new_x {std::numeric_limits<gsl::index>::max()};
    auto new_y {std::numeric_limits<gsl::index>::max()};
    // For each node in the child cuboids, check if its physical coordinate
    // is within the bounds of the indicator functor
    for (gsl::index x = 0; x < nx; ++x) {
      for (gsl::index y = 0; y < ny; ++y) {
        auto is_inside = false;
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
T CuboidGeometry2D<T>::GetMinDeltaR() const
{
  auto min_delta_R {mCuboids[0].GetDeltaR()};
  for (gsl::index i = 0; i < mCuboids.size(); ++i) {
    if (mCuboids[i].GetDeltaR() < min_delta_R)
        min_delta_R = mCuboids[i].GetDeltaR();
  }
  return min_delta_R;
}

template<typename T>
std::size_t CuboidGeometry2D<T>::GetNumberOfCuboids() const
{
  return mCuboids.size();
}

template<typename T>
Cuboid2D<T> CuboidGeometry2D<T>::GetMotherCuboid() const
{
  return mMotherCuboid;
}

template<typename T>
void CuboidGeometry2D<T>::SetIsPeriodic(
    bool isPeriodicX
  , bool isPeriodicY)
{
  mIsPeriodic[0] = isPeriodicX;
  mIsPeriodic[1] = isPeriodicY;
}
// Explicit instantiation
template class CuboidGeometry2D<double>;
}  // namespace iblbm
