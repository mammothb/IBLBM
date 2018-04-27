#include "BlockGeometryStatistics2D.hpp"

#include <cmath>
#include <iostream>

#include "AbstractBlockGeometryInterface2D.hpp"

namespace iblbm
{
template<typename T>
BlockGeometryStatistics2D<T>::BlockGeometryStatistics2D(
    AbstractBlockGeometryInterface2D<T>* pBlockGeometry)
  : mpBlockGeometry{pBlockGeometry},
    mNeedUpdate{true},
    mNx{0},
    mNy{0},
    mDeltaR{1.0},
    mNumMaterial{0},
    mOstream{std::cout, "BlockGeometryStatistics2D"}
{}

template<typename T>
void BlockGeometryStatistics2D<T>::Update(bool isVerbose/*=false*/)
{
  if (mNeedUpdate) {
    mMaterialToNum.clear();

    mNx = mpBlockGeometry->GetNx();
    mNy = mpBlockGeometry->GetNy();
    mDeltaR = mpBlockGeometry->GetDeltaR();

    for (gsl::index x {0}; x < mNx; ++x) {
      for (gsl::index y {0}; y < mNy; ++y) TakeStatistics(x, y);
    }

    mNumMaterial = mMaterialToNum.size();

    if (isVerbose) mOstream << "updated" << std::endl;
    mNeedUpdate = false;
  }
}

template<typename T>
bool& BlockGeometryStatistics2D<T>::rGetStatus()
{
  return mNeedUpdate;
}

template<typename T>
const bool& BlockGeometryStatistics2D<T>::rGetStatus() const
{
  return mNeedUpdate;
}

template<typename T>
std::map<int, std::size_t> BlockGeometryStatistics2D<T>::GetMaterialToNum(
    bool isVerbose/*=false*/)
{
  Update(isVerbose);
  return mMaterialToNum;
}

template<typename T>
std::size_t BlockGeometryStatistics2D<T>::GetNumMaterial()
{
  Update();
  return mNumMaterial;
}

template<typename T>
std::size_t BlockGeometryStatistics2D<T>::GetNumVoxel(int material)
{
  Update();
  return mMaterialToNum[material];
}

template<typename T>
std::size_t BlockGeometryStatistics2D<T>::GetNumVoxel()
{
  Update();
  std::size_t total {0};
  for (const auto& r_it : mMaterialToNum) {
    if (r_it.first != 0) total += r_it.second;
  }
  return total;
}

template<typename T>
std::vector<gsl::index> BlockGeometryStatistics2D<T>::GetMinLatticeR(
    int material)
{
  Update();
  return mMaterialToMin[material];
}

template<typename T>
std::vector<gsl::index> BlockGeometryStatistics2D<T>::GetMaxLatticeR(
    int material)
{
  Update();
  return mMaterialToMax[material];
}

template<typename T>
std::vector<T> BlockGeometryStatistics2D<T>::GetMinPhysR(int material)
{
  std::vector<T> phys_R(2, T{});
  mpBlockGeometry->GetPhysR(&(GetMinLatticeR(material)[0]), &(phys_R[0]));
  return phys_R;
}

template<typename T>
std::vector<T> BlockGeometryStatistics2D<T>::GetMaxPhysR(int material)
{
  std::vector<T> phys_R(2, T{});
  mpBlockGeometry->GetPhysR(&(GetMaxLatticeR(material)[0]), &(phys_R[0]));
  return phys_R;
}

template<typename T>
std::vector<T> BlockGeometryStatistics2D<T>::GetLatticeExtent(int material)
{
  Update();
  std::vector<T> lattice_extent;
  for (gsl::index d {0}; d < 2; ++d) {
    lattice_extent.push_back(mMaterialToMax[material][d] -
        mMaterialToMin[material][d]);
  }
  return lattice_extent;
}

template<typename T>
std::vector<T> BlockGeometryStatistics2D<T>::GetPhysExtent(int material)
{
  Update();
  std::vector<T> phys_extent;
  for (gsl::index d {0}; d < 2; ++d) {
    phys_extent.push_back(GetMaxPhysR(material)[d] -
        GetMinPhysR(material)[d]);
  }
  return phys_extent;
}

template<typename T>
std::vector<T> BlockGeometryStatistics2D<T>::GetPhysRadius(int material)
{
  Update();
  std::vector<T> radius;
  for (gsl::index d {0}; d < 2; ++d) {
    radius.push_back(0.5 * (GetMaxPhysR(material)[d] -
        GetMinPhysR(material)[d]));
  }
  return radius;
}

template<typename T>
std::vector<T> BlockGeometryStatistics2D<T>::GetCenterPhysR(int material)
{
  Update();
  std::vector<T> center;
  for (gsl::index d {0}; d < 2; ++d) {
    center.push_back(GetMinPhysR(material)[d] + GetPhysRadius(material)[d]);
  }
  return center;
}

template<typename T>
std::vector<int> BlockGeometryStatistics2D<T>::GetType(
    gsl::index xIndex
  , gsl::index yIndex)
{
  std::vector<int> discrete_normal(3, 0);

  auto mat {mpBlockGeometry->GetMaterial(xIndex, yIndex)};
  auto mat_ypos {mpBlockGeometry->GetMaterial(xIndex, yIndex + 1)};
  auto mat_yneg {mpBlockGeometry->GetMaterial(xIndex, yIndex - 1)};
  auto mat_xpos {mpBlockGeometry->GetMaterial(xIndex + 1, yIndex)};
  auto mat_xneg {mpBlockGeometry->GetMaterial(xIndex - 1, yIndex)};
  auto mat_xpos_ypos {mpBlockGeometry->GetMaterial(xIndex + 1, yIndex + 1)};
  auto mat_xpos_yneg {mpBlockGeometry->GetMaterial(xIndex + 1, yIndex - 1)};
  auto mat_xneg_ypos {mpBlockGeometry->GetMaterial(xIndex - 1, yIndex + 1)};
  auto mat_xneg_yneg {mpBlockGeometry->GetMaterial(xIndex - 1, yIndex - 1)};

  // Ignore nodes which are Do Nothing and fluid nodes by convention
  if (mat == mFluid || mat == mDoNothing) return discrete_normal;

  // boundary0N and boundary 0P, boundary along y-axis
  if (mat_ypos != mFluid && mat_ypos != mDoNothing &&
      mat_yneg != mFluid && mat_yneg != mDoNothing) {
    if (mat_xpos == mFluid) discrete_normal = {0, -1, 0};
    if (mat_xneg == mFluid) discrete_normal = {0, 1, 0};
  }

  // boundary1N and boundary1P, boundary along x-axis
  if (mat_xpos != mFluid && mat_xpos != mDoNothing &&
      mat_xneg != mFluid && mat_xneg != mDoNothing) {
    if (mat_ypos == mFluid) discrete_normal = {0, 0, -1};
    if (mat_yneg == mFluid) discrete_normal = {0, 0, 1};
  }

  // externalCornerNN and externalCornerNP
  if (mat_xpos != mFluid && mat_xpos != mDoNothing) {
    // lower left corner
    if (mat_ypos != mFluid && mat_ypos != mDoNothing &&
        mat_xpos_ypos == mFluid) {
      discrete_normal = {1, -1, -1};
    }
    // upper left corner
    if (mat_yneg != mFluid && mat_yneg != mDoNothing &&
        mat_xpos_yneg == mFluid) {
      discrete_normal = {1, -1, 1};
    }
  }

  // externalCornerPN and externalCornerPP
  if (mat_xneg != mFluid && mat_xneg != mDoNothing) {
    // lower right corner
    if (mat_ypos != mFluid && mat_ypos != mDoNothing &&
        mat_xneg_ypos == mFluid) {
      discrete_normal = {1, 1, -1};
    }
    // upper right corner
    if (mat_yneg != mFluid && mat_yneg != mDoNothing &&
        mat_xneg_yneg == mFluid) {
      discrete_normal = {1, 1, 1};
    }
  }

  // internalCornerNN and internalCornerNP
  if (mat_xneg != mFluid && mat_xneg != mDoNothing) {
    // upper right corner
    if (mat_yneg != mFluid && mat_yneg != mDoNothing &&
        mat_xneg_yneg == mDoNothing) {
      discrete_normal = {2, -1, -1};
    }
    // lower right corner
    if (mat_ypos != mFluid && mat_ypos != mDoNothing &&
        mat_xneg_ypos == mDoNothing) {
      discrete_normal = {2, -1, 1};
    }
  }

  // internalCornerPN and internalCornerPP
  if (mat_xpos != mFluid && mat_xpos != mDoNothing) {
    // upper left corner
    if (mat_yneg != mFluid && mat_yneg != mDoNothing &&
        mat_xpos_yneg == mDoNothing) {
      discrete_normal = {2, 1, -1};
    }
    // lower left corner
    if (mat_ypos != mFluid && mat_ypos != mDoNothing &&
        mat_xpos_ypos == mDoNothing) {
      discrete_normal = {2, 1, 1};
    }
  }
  return discrete_normal;
}


template<typename T>
std::vector<int> BlockGeometryStatistics2D<T>::ComputeNormal(
    gsl::index xIndex
  , gsl::index yIndex)
{
  Update();
  std::vector<int> normal(2, 0);

  if (xIndex != 0 &&
      mpBlockGeometry->GetMaterial(xIndex - 1, yIndex) == mFluid) {
    normal[0] = -1;
  }
  if (xIndex != mNx - 1 &&
      mpBlockGeometry->GetMaterial(xIndex + 1, yIndex) == mFluid) {
    normal[0] = 1;
  }
  if (yIndex != 0 &&
      mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) == mFluid) {
    normal[1] = -1;
  }
  if (yIndex != mNy - 1 &&
      mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) == mFluid) {
    normal[1] = 1;
  }
  return normal;
}

template<typename T>
std::vector<T> BlockGeometryStatistics2D<T>::ComputeNormal(int material)
{
  Update();
  std::vector<T> normal(2, 0);
  auto min_lattice_R {GetMinLatticeR(material)};
  auto max_lattice_R {GetMaxLatticeR(material)};
  for (gsl::index x {min_lattice_R[0]}; x <= max_lattice_R[0]; ++x) {
    for (gsl::index y {min_lattice_R[1]}; y <= max_lattice_R[1]; ++y) {
      if (mpBlockGeometry->GetMaterial(x, y) == material) {
        normal[0] += ComputeNormal(x, y)[0];
        normal[1] += ComputeNormal(x, y)[1];
      }
    }
  }
  // Convert to unit vector
  T norm {sqrt(normal[0] * normal[0] + normal[1] * normal[1])};
  if (norm > 0.0) {
    normal[0] /= norm;
    normal[1] /= norm;
  }
  return normal;
}

template<typename T>
std::vector<int> BlockGeometryStatistics2D<T>::ComputeDiscreteNormal(
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
bool BlockGeometryStatistics2D<T>::Check(
    int material
  , gsl::index xIndex
  , gsl::index yIndex
  , unsigned xOffset
  , unsigned yOffset)
{
  auto int_x_offset {static_cast<int>(xOffset)};
  auto int_y_offset {static_cast<int>(yOffset)};
  Update();
  bool found {true};
  for (gsl::index x_offset {-int_x_offset}; x_offset <= int_x_offset;
      ++x_offset) {
    for (gsl::index y_offset {-int_y_offset}; y_offset <= int_y_offset;
        ++y_offset) {
      if (mpBlockGeometry->GetMaterial(xIndex + x_offset,
          yIndex + y_offset) != material) {
        found = false;
        break;
      }
    }
  }
  return found;
}

template<typename T>
bool BlockGeometryStatistics2D<T>::Find(
    int material
  , unsigned xOffset
  , unsigned yOffset
  , gsl::index& rXIndex
  , gsl::index& rYIndex)
{
  Update();
  bool found {false};
  for (rXIndex = 0; rXIndex < mNx; ++rXIndex) {
    for (rYIndex = 0; rYIndex < mNy; ++rYIndex) {
      found = Check(material, rXIndex, rYIndex, xOffset, yOffset);
      if (found) return found;
    }
  }
  return found;
}

template<typename T>
void BlockGeometryStatistics2D<T>::Print()
{
  Update();
  for (const auto& r_it : mMaterialToNum) {
    mOstream << "MaterialNumber=" << r_it.first << " count=" << r_it.second <<
        " MinLatticeR=(" << mMaterialToMin[r_it.first][0] << ", " <<
        mMaterialToMin[r_it.first][1] << ")" <<
        " MaxLatticeR=(" << mMaterialToMax[r_it.first][0] << ", " <<
        mMaterialToMax[r_it.first][1] << ")" << std::endl;
  }
}

template<typename T>
void BlockGeometryStatistics2D<T>::TakeStatistics(
    gsl::index x
  , gsl::index y)
{
  auto material = mpBlockGeometry->GetMaterial(x, y);
  if (mMaterialToNum.count(material) == 0) {
    mMaterialToNum[material] = 1;
    std::vector<gsl::index> lattice_R {x, y};
    mMaterialToMin[material] = lattice_R;
    mMaterialToMax[material] = lattice_R;

  }
  else {
    ++mMaterialToNum[material];
    if (x < mMaterialToMin[material][0]) mMaterialToMin[material][0] = x;
    if (y < mMaterialToMin[material][1]) mMaterialToMin[material][1] = y;
    if (x > mMaterialToMax[material][0]) mMaterialToMax[material][0] = x;
    if (y > mMaterialToMax[material][1]) mMaterialToMax[material][1] = y;
  }
}

// explicit instantiation
template class BlockGeometryStatistics2D<double>;
}  // namespace iblbm
