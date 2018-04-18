#include "Cuboid2D.hpp"

#include <cmath>
#include <limits>

#include "Exception.hpp"

namespace iblbm
{
template<typename T>
Cuboid2D<T>::Cuboid2D(
    T xPosition
  , T yPosition
  , T deltaR
  , std::size_t nx
  , std::size_t ny
  , int refinementLevel/*=0*/)
  : mXPosition{xPosition},
    mYPosition{yPosition},
    mDeltaR{deltaR},
    mNx{nx},
    mNy{ny},
    mWeight{std::numeric_limits<std::size_t>::max()},
    mRefinementLevel{refinementLevel},
    mOstream{std::cout, "Cuboid2D"}
{}

template<typename T>
Cuboid2D<T>::Cuboid2D(
    const Vector2D<T>& rOrigin
  , T deltaR
  , const Vector2D<std::size_t>& rLatticeExtent
  , int refinementLevel/*=0*/)
  : mXPosition{rOrigin[0]},
    mYPosition{rOrigin[1]},
    mDeltaR{deltaR},
    mNx{rLatticeExtent[0]},
    mNy{rLatticeExtent[1]},
    mWeight{std::numeric_limits<std::size_t>::max()},
    mRefinementLevel{refinementLevel},
    mOstream{std::cout, "Cuboid2D"}
{}

template<typename T>
Cuboid2D<T>::Cuboid2D(
    const Cuboid2D<T>& rRhs
  , std::size_t overlap/*=0*/)
  : mWeight{rRhs.mWeight},
    mRefinementLevel{rRhs.mRefinementLevel},
    mOstream{std::cout, "Cuboid2D"}
{
  this->Initialize(rRhs.mXPosition - rRhs.mDeltaR * overlap,
      rRhs.mYPosition - rRhs.mDeltaR * overlap, rRhs.mDeltaR,
      rRhs.mNx + 2 * overlap, rRhs.mNy + 2 * overlap);
}

template<typename T>
Cuboid2D<T>& Cuboid2D<T>::operator=(Cuboid2D<T> const& rRhs)
{
  this->Initialize(rRhs.mXPosition, rRhs.mYPosition, rRhs.mDeltaR, rRhs.mNx,
      rRhs.mNy);
  mWeight = rRhs.mWeight;
  mRefinementLevel = rRhs.mRefinementLevel;
  return *this;
}

template<typename T>
void Cuboid2D<T>::Initialize(
    T xPosition
  , T yPosition
  , T deltaR
  , std::size_t nx
  , std::size_t ny
  , int refinementLevel/*=0*/)
{
  mXPosition = xPosition;
  mYPosition = yPosition;
  mDeltaR = deltaR;
  mNx = nx;
  mNy = ny;
  mRefinementLevel = refinementLevel;
}

template<typename T>
void Cuboid2D<T>::Divide(
    std::size_t numCol
  , std::size_t numRow
  , std::vector<Cuboid2D<T>>& rCuboids) const
{
  // Need to be declared here so it can be accessed in different scopes
  auto pos_x_child {mXPosition};
  for (gsl::index x = 0; x < numCol; ++x) {
    auto pos_y_child {mYPosition};
    auto nx_child {(mNx + numCol - 1 - x) / numCol};
    for (gsl::index y = 0; y < numRow; ++y) {
      // Calculate the number of nodes per row/col for the child cuboid such
      // that all nodes are mapped over
      auto ny_child {(mNy + numRow - 1 - y) / numRow};
      // Create the child cuiboid to be added to the vector
      Cuboid2D<T> child_cuboid(pos_x_child, pos_y_child, mDeltaR, nx_child,
          ny_child, mRefinementLevel);
      rCuboids.push_back(child_cuboid);
      // calculate global position of bottom left corner for the next child
      pos_y_child += ny_child * mDeltaR;
    }
    pos_x_child += nx_child * mDeltaR;
  }
}

template<typename T>
void Cuboid2D<T>::Divide(
    std::size_t numCuboid
  , std::vector<Cuboid2D<T>>& rCuboids) const
{
  Expects(numCuboid > 0);
  // Initialize layout values as one column of child cuboids so we can avoid
  // undefined error caused by the default CuboidGeometry2D constructor
  std::size_t num_col {1};
  std::size_t num_row {numCuboid};

  // Ideally divide the cuboid into child cuboids with same number of nodes
  const auto best_ratio {static_cast<T>(mNx) / static_cast<T>(mNy)};
  // Difference from the ideal ratio, to track how close we got
  auto best_diff {std::numeric_limits<T>::max()};
  for (gsl::index col = 1; col <= numCuboid; ++col) {
    const auto row {numCuboid / col};
    // Make sure we aren't creating more child nodes than we're asked to
    if (col * row <= numCuboid) {
      const auto curr_diff {std::abs(best_ratio - static_cast<T>(col) /
          static_cast<T>(row))};
      if (curr_diff <= best_diff) {
        best_diff = curr_diff;
        num_col = col;
        num_row = row;
      }
    }
  }

  const auto ratio {static_cast<T>(num_col) / static_cast<T>(num_row)};
  const auto the_rest {numCuboid - num_col * num_row};

  if (the_rest == 0) {
    Divide(num_col, num_row, rCuboids);
    return;
  }

  // If the optimal layout is taller than our node layout and we have more
  // rows than the_rest child cuboid
  if (ratio < best_ratio && num_row - the_rest >= 0) {
    // Number of "same" child cuboid we can create, the last the_rest
    // rows will be narrower to accommodate the the_rest child cuboids
    const auto num_same {num_col * (num_row - the_rest)};
    // Number of nodes allocated to the "same" child cuboids
    const auto vol_same {static_cast<T>(mNx) * static_cast<T>(mNy) *
        num_same / static_cast<T>(numCuboid)};
    const auto ny_same {static_cast<std::size_t>(vol_same /
        static_cast<T>(mNx))};
    const auto nx_same {mNx};
    const auto ny_diff {mNy - ny_same};
    const auto nx_diff {mNx};
    // First child cuboid to be divided into "same" cuboids
    Cuboid2D<T> child_1(mXPosition, mYPosition, mDeltaR, nx_same, ny_same,
        mRefinementLevel);
    // Second child cuboid to be divided into "diff" cuboids
    Cuboid2D<T> child_2(mXPosition, mYPosition + mDeltaR * ny_same, mDeltaR,
        nx_diff, ny_diff, mRefinementLevel);
    child_1.Divide(num_col, num_row - the_rest, rCuboids);
    // Each row will contain one more cuboid than the "same" child
    child_2.Divide(num_col + 1, the_rest, rCuboids);
  }
  else {
    // Number of "same" child cuboid we can create, the last the_rest
    // column will be shorter to accommodate the the_rest child cuboids
    const auto num_same {num_row * (num_col - the_rest)};
    const auto vol_same {static_cast<T>(mNx) * static_cast<T>(mNy) *
        num_same / static_cast<T>(numCuboid)};
    const auto ny_same {mNy};
    // Add 0.9999 to ensure we round up properly
    const auto nx_same {static_cast<std::size_t>(vol_same /
        static_cast<T>(mNy) + 0.9999)};
    const auto ny_diff {mNy - ny_same};
    const auto nx_diff {mNx};
    Cuboid2D<T> child_1(mXPosition, mYPosition, mDeltaR, nx_same, ny_same,
        mRefinementLevel);
    Cuboid2D<T> child_2(mXPosition + mDeltaR * nx_same, mYPosition, mDeltaR,
        nx_diff, ny_diff, mRefinementLevel);
    child_1.Divide(num_col - the_rest, num_row, rCuboids);
    // Each column contain one more cuboid than the "same" child
    child_2.Divide(the_rest, num_row + 1, rCuboids);
  }
}

template<typename T>
void Cuboid2D<T>::Resize(
    gsl::index xIndex
  , gsl::index yIndex
  , std::size_t nx
  , std::size_t ny)
{
  mXPosition += mDeltaR * xIndex;
  mYPosition += mDeltaR * yIndex;
  mNx = nx;
  mNy = ny;
}

template<typename T>
bool Cuboid2D<T>::ContainPoint(
    T xPosition
  , T yPosition
  , std::size_t overlap/*=0*/) const
{
  return mXPosition - T{0.5 + overlap} * mDeltaR <= xPosition &&
      mXPosition + T{mNx - 0.5 + overlap} * mDeltaR > xPosition &&
      mYPosition - T{0.5 + overlap} * mDeltaR <= yPosition &&
      mYPosition + T{mNy - 0.5 + overlap} * mDeltaR > yPosition;
}

template<typename T>
bool Cuboid2D<T>::ContainPoint(
    T xPosition
  , T yPosition
  , gsl::index& rXIndex
  , gsl::index& rYIndex
  , std::size_t overlap/*=0*/) const
{
  if (ContainPoint(xPosition, yPosition, overlap)) {
    rXIndex = static_cast<gsl::index>(floor((xPosition - mXPosition +
        static_cast<T>(overlap) * mDeltaR) / mDeltaR + 0.5));
    rYIndex = static_cast<gsl::index>(floor((yPosition - mYPosition +
        static_cast<T>(overlap) * mDeltaR) / mDeltaR + 0.5));
    return true;
  }
  else {
    return false;
  }
}

template<typename T>
bool Cuboid2D<T>::CheckIntersection(
    T xPosition0
  , T yPosition0
  , T xPosition1
  , T yPosition1
  , std::size_t overlap/*=0*/) const
{
  T x_0 = std::max(mXPosition - overlap * mDeltaR, xPosition0);
  T y_0 = std::max(mYPosition - overlap * mDeltaR, yPosition0);
  T x_1 = std::min(mXPosition + (mNx - 1 + overlap) * mDeltaR, xPosition1);
  T y_1 = std::min(mYPosition + (mNy - 1 + overlap) * mDeltaR, yPosition1);

  return x_1 >= x_0 && y_1 >= y_0;
}

template<typename T>
bool Cuboid2D<T>::CheckIntersection(
    T xPosition
  , T yPosition
  , std::size_t overlap/*=0*/) const
{
  // This is just ContainPoint(xPosition, yPosition, overlap)
  return CheckIntersection(xPosition, yPosition, xPosition, yPosition,
      overlap);
}

template<typename T>
bool Cuboid2D<T>::CheckIntersection(
    T xPosition0
  , T yPosition0
  , T xPosition1
  , T yPosition1
  , gsl::index& rXIndex0
  , gsl::index& rYIndex0
  , gsl::index& rXIndex1
  , gsl::index& rYIndex1
  , std::size_t overlap/*=0*/) const
{
  if (CheckIntersection(xPosition0, yPosition0, xPosition1,
      yPosition1, overlap)) {
    rXIndex0 = std::max(static_cast<gsl::index>((xPosition0 - mXPosition) /
        mDeltaR + overlap + 0.5), gsl::index{0});
    rYIndex0 = std::max(static_cast<gsl::index>((yPosition0 - mYPosition) /
        mDeltaR + overlap + 0.5), gsl::index{0});
    rXIndex1 = std::min(static_cast<gsl::index>((xPosition1 - mXPosition) /
        mDeltaR + overlap + 0.5), static_cast<gsl::index>(mNx - 1 + 2 *
        overlap));
    rYIndex1 = std::min(static_cast<gsl::index>((yPosition1 - mYPosition) /
        mDeltaR + overlap + 0.5), static_cast<gsl::index>(mNy - 1 + 2 *
        overlap));
    return true;
  }
  else {
    rXIndex0 = 1;
    rXIndex1 = 0;
    rYIndex0 = 1;
    rYIndex1 = 0;
    return false;
  }
}

template<typename T>
void Cuboid2D<T>::RefineToLevel(int refinementLevel)
{
  if (refinementLevel <= mRefinementLevel) return;
  auto level_diff {static_cast<int>(std::pow(2, refinementLevel -
      mRefinementLevel))};
  mDeltaR /= static_cast<T>(level_diff);
  mNx *= level_diff;
  mNy *= level_diff;
  mRefinementLevel = refinementLevel;
}

template<typename T>
void Cuboid2D<T>::RefineIncrease()
{
  mDeltaR /= 2.0;
  mNx *= 2;
  mNy *= 2;
  ++mRefinementLevel;
}

template<typename T>
void Cuboid2D<T>::RefineDecrease()
{
  if (mRefinementLevel == 0) return;
  mDeltaR *= 2.0;
  mNx /= 2;
  mNy /= 2;
  --mRefinementLevel;
}

template<typename T>
T Cuboid2D<T>::GetGlobalXPosition() const
{
  return mXPosition;
}

template<typename T>
T Cuboid2D<T>::GetGlobalYPosition() const
{
  return mYPosition;
}

template<typename T>
const Vector2D<T> Cuboid2D<T>::GetOrigin() const
{
  return Vector2D<T>(mXPosition, mYPosition);
}

template<typename T>
T Cuboid2D<T>::GetDeltaR() const
{
  return mDeltaR;
}

template<typename T>
std::size_t Cuboid2D<T>::GetNx() const
{
  return mNx;
}

template<typename T>
std::size_t Cuboid2D<T>::GetNy() const
{
  return mNy;
}

template<typename T>
const Vector2D<std::size_t> Cuboid2D<T>::GetLatticeExtent() const
{
  return Vector2D<std::size_t>(mNx, mNy);
}

template<typename T>
std::size_t Cuboid2D<T>::GetWeight() const
{
  return mWeight;
}

template<typename T>
T Cuboid2D<T>::GetPhysPerimeter() const
{
  return T{2} * mDeltaR * mNy + T{2} * mDeltaR * mNx;
}

template<typename T>
std::size_t Cuboid2D<T>::GetLatticePerimeter() const
{
  Expects(2 * mNy + 2 * mNx >= 4);
  return 2 * mNy + 2 * mNx - 4;
}

template<typename T>
T Cuboid2D<T>::GetPhysVolume() const
{
  return mDeltaR * mNy * mDeltaR * mNx;
}

template<typename T>
std::size_t Cuboid2D<T>::GetLatticeVolume() const
{
  return mNy * mNx;
}

template<typename T>
Vector2D<T> Cuboid2D<T>::GetPhysR(
    const gsl::index& rXIndex
  , const gsl::index& rYIndex) const
{
  return Vector2D<T>{mXPosition + mDeltaR * rXIndex, mYPosition + mDeltaR *
      rYIndex};
}

template<typename T>
void Cuboid2D<T>::GetPhysR(
    const gsl::index latticeR[2]
  , T physR[2]) const
{
  physR[0] = mXPosition + latticeR[0] * mDeltaR;
  physR[1] = mYPosition + latticeR[1] * mDeltaR;
}

template<typename T>
void Cuboid2D<T>::GetPhysR(
    const gsl::index& rXIndex
  , const gsl::index& rYIndex
  , T physR[2]) const
{
  physR[0] = mXPosition + rXIndex * mDeltaR;
  physR[1] = mYPosition + rYIndex * mDeltaR;
}

template<typename T>
void Cuboid2D<T>::GetLatticeR(
    const T physR[2]
  , gsl::index latticeR[2]) const
{
  latticeR[0] = static_cast<gsl::index>(floor((physR[0] - mXPosition) /
      mDeltaR + 0.5));
  latticeR[1] = static_cast<gsl::index>(floor((physR[1] - mYPosition) /
      mDeltaR + 0.5));
}

template<typename T>
void Cuboid2D<T>::GetLatticeR(
    const Vector2D<T>& rPhysR
  , gsl::index latticeR[2]) const
{
  latticeR[0] = static_cast<gsl::index>(floor((rPhysR[0] - mXPosition) /
      mDeltaR + 0.5));
  latticeR[1] = static_cast<gsl::index>(floor((rPhysR[1] - mYPosition) /
      mDeltaR + 0.5));
}

template<typename T>
void Cuboid2D<T>::GetFloorLatticeR(
    const std::vector<T>& rPhysR
  , std::vector<gsl::index>& rLatticeR) const
{
  GetFloorLatticeR(&rPhysR[0], &rLatticeR[0]);
}

template<typename T>
void Cuboid2D<T>::GetFloorLatticeR(
    const T physR[2]
  , gsl::index latticeR[2]) const
{
  latticeR[0] = static_cast<gsl::index>(floor((physR[0] - mXPosition) /
      mDeltaR));
  latticeR[1] = static_cast<gsl::index>(floor((physR[1] - mYPosition) /
      mDeltaR));
}

template<typename T>
int Cuboid2D<T>::GetRefinementLevel() const
{
  return mRefinementLevel;
}

template<typename T>
void Cuboid2D<T>::SetWeight(std::size_t weight)
{
  mWeight = weight;
}

template<typename T>
void Cuboid2D<T>::Print() const
{
  mOstream <<
      "==== Cuboid2D information =====" << std::endl <<
      "Parameter          | Variable | Value" << std::endl <<
      "Lower left corner  | (x, y)   | (" << GetGlobalXPosition() << ", " << GetGlobalYPosition() << ")" << std::endl <<
      "Node spacing       | Delta R  | " << GetDeltaR() << std::endl <<
      "Perimeter          |          | " << GetPhysPerimeter() << std::endl <<
      "Volume             |          | " << GetPhysVolume() << std::endl <<
      "Extent             | (nx, ny) | (" << GetNx() << ", " << GetNy() << ")" << std::endl <<
      "Nodes at perimeter |          | " << GetLatticePerimeter() << std::endl <<
      "Nodes in volume    |          | " << GetLatticeVolume() << std::endl <<
      "-------------------------------" << std::endl;
}

// Explicit instantiation
template class Cuboid2D<double>;
}  // namespace iblbm
