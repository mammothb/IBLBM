#include "BlockGeometry2D.hpp"

#include "gsl/gsl"

#include "Exception.hpp"

namespace iblbm
{
template<typename T>
BlockGeometry2D<T>::BlockGeometry2D(
    T xPosition
  , T yPosition
  , T deltaR
  , std::size_t nx
  , std::size_t ny
  , gsl::index globalCuboidIndex/*=-1*/)
  : BlockData2D<T, std::size_t>{nx, ny},
    AbstractBlockGeometryInterface2D<T>{globalCuboidIndex},
    mCuboid{xPosition, yPosition, deltaR, nx, ny}
{
  this->mStatistics = BlockGeometryStatistics2D<T>{this};
  AddToStatisticsList(&(this->mStatistics.rGetStatus()));
}

template<typename T>
BlockGeometry2D<T>::BlockGeometry2D(
    const Cuboid2D<T>& rCuboid
  , gsl::index globalCuboidIndex/*=-1*/)
  : BlockData2D<T, std::size_t>{rCuboid.GetNx(), rCuboid.GetNy()},
    AbstractBlockGeometryInterface2D<T>{globalCuboidIndex},
    mCuboid{rCuboid}
{
  this->mStatistics = BlockGeometryStatistics2D<T>{this};
  AddToStatisticsList(&(this->mStatistics.rGetStatus()));
}

template<typename T>
BlockGeometryStatistics2D<T>& BlockGeometry2D<T>::rGetStatistics(
    bool /*isVerbose=true*/)
{
  return this->mStatistics;
}

template<typename T>
const BlockGeometryStatistics2D<T>& BlockGeometry2D<T>::rGetStatistics(
    bool /*isVerbose=true*/) const
{
  return this->mStatistics;
}

template<typename T>
Vector2D<T> BlockGeometry2D<T>::GetOrigin() const
{
  return mCuboid.GetOrigin();
}

template<typename T>
std::size_t BlockGeometry2D<T>::GetNx() const
{
  return mCuboid.GetNx();
}

template<typename T>
std::size_t BlockGeometry2D<T>::GetNy() const
{
  return mCuboid.GetNy();
}

template<typename T>
const T BlockGeometry2D<T>::GetDeltaR() const
{
  return mCuboid.GetDeltaR();
}

template<typename T>
void BlockGeometry2D<T>::GetPhysR(
    const gsl::index& rXIndex
  , const gsl::index& rYIndex
  , T physR[2]) const
{
  mCuboid.GetPhysR(rXIndex, rYIndex, physR);
}

template<typename T>
int BlockGeometry2D<T>::GetMaterial(
    gsl::index xIndex
  , gsl::index yIndex) const
{
  int material;
  if (xIndex < 0 || xIndex > GetNx() - 1 || yIndex < 0 ||
      yIndex > GetNy() - 1) {
    material = BlockGeometryStatistics2D<T>::mDoNothing;
  }
  else {
    material = BlockData2D<T, std::size_t>::rGetData(xIndex, yIndex);
  }
  return material;
}

template<typename T>
void BlockGeometry2D<T>::AddToStatisticsList(bool* pStatisticStatus)
{
  mNeedStatisticsUpdate.push_back(pStatisticStatus);
}

template<typename T>
void BlockGeometry2D<T>::RemoveFromStatisticsList(bool* pStatisticStatus)
{
  mNeedStatisticsUpdate.remove(pStatisticStatus);
}

template<typename T>
void BlockGeometry2D<T>::PrintLayer(
    gsl::index xIndex0
  , gsl::index yIndex0
  , gsl::index xIndex1
  , gsl::index yIndex1
  , bool showLineNumber/*=false*/)
{
  for (gsl::index x {xIndex0}; x <= xIndex1; ++x) {
    if (showLineNumber) this->mOstream << x << ": ";
    for (gsl::index y {yIndex0}; y <= yIndex1; ++y)
        this->mOstream << GetMaterial(x, y) << " ";
    if (xIndex1 - xIndex0 != 0) this->mOstream << std::endl;
  }
  this->mOstream << std::endl;
}

template<typename T>
void BlockGeometry2D<T>::PrintLayer(
    int direction
  , int layer
  , bool showLineNumber/*=false*/)
{
  Expects(direction >= 0 && direction <= 2);
  switch (direction) {
  case 0:
    PrintLayer(layer, 0, layer, GetNy() - 1, showLineNumber);
    break;
  case 1:
    PrintLayer(0, layer, GetNx() - 1, layer, showLineNumber);
    break;
  default:
    NEVER_REACHED;
  }
}

template<typename T>
void BlockGeometry2D<T>::PrintNode(
    gsl::index xIndex
  , gsl::index yIndex)
{
  for (gsl::index x {xIndex - 1}; x <= xIndex + 1; ++x) {
    this->mOstream << "x=" << x << std::endl;
    for (gsl::index y {yIndex - 1}; y <= yIndex + 1; ++y)
        this->mOstream << GetMaterial(x, y) << " ";
    this->mOstream << std::endl;
  }
  this->mOstream << std::endl;
}

template<typename T>
void BlockGeometry2D<T>::ResetStatistics()
{
  for (auto& r_it : mNeedStatisticsUpdate) *r_it = true;
}

// explicit instantiation
template class BlockGeometry2D<double>;
}  // namespace iblbm
