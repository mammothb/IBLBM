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
void BlockGeometryStatistics2D<T>::Update(bool isVerbose/*=true*/)
{
  if (mNeedUpdate) {
    mMaterialToNum.clear();

    mNx = mpBlockGeometry->GetNx();
    mNy = mpBlockGeometry->GetNy();
    mDeltaR = mpBlockGeometry->GetDeltaR();

    for (gsl::index x {0}; x < mNx; ++x) {
      for (gsl::index y {0}; y < mNy; ++y) TakeStatistics(x, y);
    }

    mNumMaterial = 0;
    for (std::map<int, std::size_t>::iterator iter = mMaterialToNum.begin();
        iter != mMaterialToNum.end(); iter++) {
      ++mNumMaterial;
    }

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
std::map<int, std::size_t> BlockGeometryStatistics2D<T>::GetMaterialToNum()
{
  Update();
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
  for (std::map<int, std::size_t>::iterator iter = mMaterialToNum.begin();
      iter != mMaterialToNum.end(); iter++) {
    if (iter->first != 0) total += iter->second;
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
  std::vector<T> tmp(2, T{});
  mpBlockGeometry->GetPhysR(&(tmp[0]), &(GetMinLatticeR(material)[0]));
  return tmp;
}

template<typename T>
std::vector<T> BlockGeometryStatistics2D<T>::GetMaxPhysR(int material)
{
  std::vector<T> tmp(2, T{});
  mpBlockGeometry->GetPhysR(&(tmp[0]), &(GetMaxLatticeR(material)[0]));
  return tmp;
}

template<typename T>
std::vector<T> BlockGeometryStatistics2D<T>::GetLatticeExtent(
    int material)
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
std::vector<T> BlockGeometryStatistics2D<T>::GetPhysExtent(
    int material)
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
std::vector<T> BlockGeometryStatistics2D<T>::GetPhysRadius(
    int material)
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
std::vector<T> BlockGeometryStatistics2D<T>::GetCenterPhysR(
    int material)
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
  std::vector<int> discreteNormal(3, 0);

  // Ignore nodes which are Do Nothing and fluid nodes by convention
  if (mpBlockGeometry->GetMaterial(xIndex, yIndex) != mFluid &&
      mpBlockGeometry->GetMaterial(xIndex, yIndex) != mDoNothing) {

    // boundary0N and boundary 0P, boundary along y-axis
    if (mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) != mFluid &&
        mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) != mDoNothing &&
        mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) != mFluid &&
        mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) != mDoNothing) {
      if (mpBlockGeometry->GetMaterial(xIndex + 1, yIndex) == mFluid) {
        discreteNormal[0] = 0;
        discreteNormal[1] = -1;
        discreteNormal[2] = 0;
      }
      if (mpBlockGeometry->GetMaterial(xIndex - 1, yIndex) == mFluid) {
        discreteNormal[0] = 0;
        discreteNormal[1] = 1;
        discreteNormal[2] = 0;
      }
    }

    // boundary1N and boundary1P, boundary along x-axis
    if (mpBlockGeometry->GetMaterial(xIndex + 1, yIndex) != mFluid &&
        mpBlockGeometry->GetMaterial(xIndex + 1, yIndex) != mDoNothing &&
        mpBlockGeometry->GetMaterial(xIndex - 1, yIndex) != mFluid &&
        mpBlockGeometry->GetMaterial(xIndex - 1, yIndex) != mDoNothing) {
      if (mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) == mFluid) {
        discreteNormal[0] = 0;
        discreteNormal[1] = 0;
        discreteNormal[2] = -1;
      }
      if (mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) == mFluid) {
        discreteNormal[0] = 0;
        discreteNormal[1] = 0;
        discreteNormal[2] = 1;
      }
    }

    // externalCornerNN and externalCornerNP
    if (mpBlockGeometry->GetMaterial(xIndex + 1, yIndex) != mFluid &&
        mpBlockGeometry->GetMaterial(xIndex + 1, yIndex) != mDoNothing) {
      // lower left corner
      if (mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) != mFluid &&
          mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) != mDoNothing &&
          mpBlockGeometry->GetMaterial(xIndex + 1, yIndex + 1) == mFluid) {
        discreteNormal[0] = 1;
        discreteNormal[1] = -1;
        discreteNormal[2] = -1;
      }
      // upper left corner
      if (mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) != mFluid &&
          mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) != mDoNothing &&
          mpBlockGeometry->GetMaterial(xIndex + 1, yIndex - 1) == mFluid) {
        discreteNormal[0] = 1;
        discreteNormal[1] = -1;
        discreteNormal[2] = 1;
      }
    }

    // externalCornerPN and externalCornerPP
    if (mpBlockGeometry->GetMaterial(xIndex - 1, yIndex) != mFluid &&
        mpBlockGeometry->GetMaterial(xIndex - 1, yIndex) != mDoNothing) {
      // lower right corner
      if (mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) != mFluid &&
          mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) != mDoNothing &&
          mpBlockGeometry->GetMaterial(xIndex - 1, yIndex + 1) == mFluid) {
        discreteNormal[0] = 1;
        discreteNormal[1] = 1;
        discreteNormal[2] = -1;
      }
      // upper right corner
      if (mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) != mFluid &&
          mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) != mDoNothing &&
          mpBlockGeometry->GetMaterial(xIndex - 1, yIndex - 1) == mFluid) {
        discreteNormal[0] = 1;
        discreteNormal[1] = 1;
        discreteNormal[2] = 1;
      }
    }

    // internalCornerNN and internalCornerNP
    if (mpBlockGeometry->GetMaterial(xIndex - 1, yIndex) != mFluid &&
        mpBlockGeometry->GetMaterial(xIndex - 1, yIndex) != mDoNothing) {
      // upper right corner
      if (mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) != mFluid &&
          mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) != mDoNothing &&
          mpBlockGeometry->GetMaterial(xIndex - 1, yIndex - 1) == 0) {
        discreteNormal[0] = 2;
        discreteNormal[1] = -1;
        discreteNormal[2] = -1;
      }
      // lower right corner
      if (mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) != mFluid &&
          mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) != mDoNothing &&
          mpBlockGeometry->GetMaterial(xIndex - 1, yIndex + 1) == 0) {
        discreteNormal[0] = 2;
        discreteNormal[1] = -1;
        discreteNormal[2] = 1;
      }
    }

    // internalCornerPN and internalCornerPP
    if (mpBlockGeometry->GetMaterial(xIndex + 1, yIndex) != mFluid &&
        mpBlockGeometry->GetMaterial(xIndex + 1, yIndex) != mDoNothing) {
      // upper left corner
      if (mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) != mFluid &&
          mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) != mDoNothing &&
          mpBlockGeometry->GetMaterial(xIndex + 1, yIndex - 1) == 0) {
        discreteNormal[0] = 2;
        discreteNormal[1] = 1;
        discreteNormal[2] = -1;
      }
      // lower left corner
      if (mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) != mFluid &&
          mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) != mDoNothing &&
          mpBlockGeometry->GetMaterial(xIndex + 1, yIndex + 1) == 0) {
        discreteNormal[0] = 2;
        discreteNormal[1] = 1;
        discreteNormal[2] = 1;
      }
    }
  }
  return discreteNormal;
}


template<typename T>
std::vector<int> BlockGeometryStatistics2D<T>::ComputeNormal(
    gsl::index xIndex
  , gsl::index yIndex)
{
  Update();
  std::vector<int> normal(2, 0);

  if (xIndex != 0) {
    if (mpBlockGeometry->GetMaterial(xIndex - 1, yIndex) == mFluid)
        normal[0] = -1;
  }
  if (xIndex != mNx - 1) {
    if (mpBlockGeometry->GetMaterial(xIndex + 1, yIndex) == mFluid)
        normal[0] = 1;
  }
  if (yIndex != 0) {
    if (mpBlockGeometry->GetMaterial(xIndex, yIndex - 1) == mFluid)
        normal[1] = -1;
  }
  if (yIndex != mNy - 1) {
    if (mpBlockGeometry->GetMaterial(xIndex, yIndex + 1) == mFluid)
        normal[1] = 1;
  }
  return normal;
}

template<typename T>
std::vector<T> BlockGeometryStatistics2D<T>::ComputeNormal(
    int material)
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
  , T maxNorm)
{
  Update();
  auto normal {ComputeNormal(material)};
  std::vector<int> discreteNormal(2, 0);

  T smallestAngle {0};
  for (int x {-1}; x <= 1; ++x) {
    for (int y {-1}; y <= 1; ++y) {
      T norm {sqrt(x * x + y * y)};
      if (norm > 0.0 && norm < maxNorm) {
        T angle {(x * normal[0] + y * normal[1]) / norm};
        if (angle >= smallestAngle) {
          smallestAngle = angle;
          discreteNormal[0] = x;
          discreteNormal[1] = y;
        }
      }
    }
  }
  return discreteNormal;
}

template<typename T>
bool BlockGeometryStatistics2D<T>::Check(
    int material
  , gsl::index xIndex
  , gsl::index yIndex
  , unsigned xOffset
  , unsigned yOffset)
{
  const auto int_x_offset {static_cast<int>(xOffset)};
  const auto int_y_offset {static_cast<int>(yOffset)};
  Update();
  bool found {true};
  for (int x_offset {-int_x_offset}; x_offset <= int_x_offset; ++x_offset) {
    for (int y_offset {-int_y_offset}; y_offset <= int_y_offset; ++y_offset) {
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
  for (std::map<int, std::size_t>::iterator iter = mMaterialToNum.begin();
      iter != mMaterialToNum.end(); iter++) {
    mOstream << "materialNumber=" << iter->first <<
        "; count=" << iter->second <<
        "; minLatticeR=(" << mMaterialToMin[iter->first][0] << "," <<
        mMaterialToMin[iter->first][1] << ")" <<
        "; maxLatticeR=(" << mMaterialToMax[iter->first][0] << "," <<
        mMaterialToMax[iter->first][1] <<")" << std::endl;
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
    std::vector<gsl::index> minCo {x, y};
    std::vector<gsl::index> maxCo {x, y};
    mMaterialToMin[material] = minCo;
    mMaterialToMax[material] = maxCo;

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
