#include "BlockGeometryView2D.hpp"

#include "AbstractBlockGeometryInterface2D.hpp"

namespace iblbm
{
template<typename T>
BlockGeometryView2D<T>::BlockGeometryView2D(
    AbstractBlockGeometryInterface2D<T>& rBlockGeometry
  , gsl::index xIndex0
  , gsl::index yIndex0
  , gsl::index xIndex1
  , gsl::index yIndex1)
  : AbstractBlockGeometryInterface2D<T>{
        rBlockGeometry.rGetGlobalCuboidIndex()},
    BlockStructure2D{static_cast<std::size_t>(xIndex1 - xIndex0 + 1),
        static_cast<std::size_t>(yIndex1 - yIndex0 + 1)},
    mpBlockGeometry{&rBlockGeometry},
    mXIndex0{xIndex0},
    mYIndex0{yIndex0},
    mNx{static_cast<std::size_t>(xIndex1 - xIndex0 + 1)},
    mNy{static_cast<std::size_t>(yIndex1 - yIndex0 + 1)}
{
  this->mStatistics = BlockGeometryStatistics2D<T>(this);
  AddToStatisticsList(&(this->mStatistics.rGetStatus()));
}

template<typename T>
BlockGeometryView2D<T>::~BlockGeometryView2D()
{
  RemoveFromStatisticsList(&(this->mStatistics.rGetStatus()));
}

template<typename T>
BlockGeometryView2D<T>::BlockGeometryView2D(
    const BlockGeometryView2D<T>& rRhs)
  : AbstractBlockGeometryInterface2D<T>{rRhs},
    BlockStructure2D{rRhs.mNx, rRhs.mNy},
    mpBlockGeometry{rRhs.mpBlockGeometry},
    mXIndex0{rRhs.mXIndex0},
    mYIndex0{rRhs.mYIndex0},
    mNx{rRhs.mNx},
    mNy{rRhs.mNy}
{
  this->mGlobalCuboidIndex = rRhs.mGlobalCuboidIndex;
  this->mStatistics = BlockGeometryStatistics2D<T>(this);
  AddToStatisticsList(&(this->mStatistics.rGetStatus()));
}

template<typename T>
BlockGeometryView2D<T>& BlockGeometryView2D<T>::operator=(
    const BlockGeometryView2D<T>& rRhs)
{
  mpBlockGeometry = rRhs.mpBlockGeometry;
  mXIndex0 = rRhs.mXIndex0;
  mYIndex0 = rRhs.mYIndex0;
  mNx = rRhs.mNx;
  mNy = rRhs.mNy;
  this->mGlobalCuboidIndex = rRhs.mGlobalCuboidIndex;
  this->mStatistics = BlockGeometryStatistics2D<T>(this);
  AddToStatisticsList(&(this->mStatistics.rGetStatus()));
  return *this;
}

template<typename T>
BlockGeometryStatistics2D<T>& BlockGeometryView2D<T>::rGetStatistics(
    bool /*isVerbose=true*/)
{
  return this->mStatistics;
}

template<typename T>
const BlockGeometryStatistics2D<T>& BlockGeometryView2D<T>::rGetStatistics(
    bool /*isVerbose=true*/) const
{
  return this->mStatistics;
}

template<typename T>
Vector2D<T> BlockGeometryView2D<T>::GetOrigin() const
{
  Vector2D<T> origin;
  origin[0] = mpBlockGeometry->GetOrigin()[0] + static_cast<T>(mXIndex0) *
      GetDeltaR();
  origin[1] = mpBlockGeometry->GetOrigin()[1] + static_cast<T>(mYIndex0) *
      GetDeltaR();
  return origin;
}

template<typename T>
std::size_t BlockGeometryView2D<T>::GetNx() const
{
  return mNx;
}

template<typename T>
std::size_t BlockGeometryView2D<T>::GetNy() const
{
  return mNy;
}

template<typename T>
const T BlockGeometryView2D<T>::GetDeltaR() const
{
  return mpBlockGeometry->GetDeltaR();
}

template<typename T>
void BlockGeometryView2D<T>::GetPhysR(
    const gsl::index& rXIndex
  , const gsl::index& rYIndex
  , T physR[2]) const
{
  mpBlockGeometry->GetPhysR(mXIndex0 + rXIndex, mYIndex0 + rYIndex, physR);
}

template<typename T>
int BlockGeometryView2D<T>::GetMaterial(
    gsl::index xIndex
  , gsl::index yIndex) const
{
  return mpBlockGeometry->GetMaterial(mXIndex0 + xIndex, mYIndex0 + yIndex);
}

template<typename T>
int& BlockGeometryView2D<T>::rGetMaterial(
    gsl::index xIndex
  , gsl::index yIndex)
{
  return mpBlockGeometry->rGetMaterial(mXIndex0 + xIndex, mYIndex0 + yIndex);
}

template<typename T>
const int& BlockGeometryView2D<T>::rGetMaterial(
    gsl::index xIndex
  , gsl::index yIndex) const
{
  return mpBlockGeometry->rGetMaterial(mXIndex0 + xIndex, mYIndex0 + yIndex);
}

template<typename T>
void BlockGeometryView2D<T>::AddToStatisticsList(bool* pStatisticsStatus)
{
  mpBlockGeometry->AddToStatisticsList(pStatisticsStatus);
}

template<typename T>
void BlockGeometryView2D<T>::RemoveFromStatisticsList(bool* pStatisticsStatus)
{
  mpBlockGeometry->RemoveFromStatisticsList(pStatisticsStatus);
}

// Explicit instantiation
template class BlockGeometryView2D<double>;
}  // namespace iblbm
