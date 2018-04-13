#include "Cuboid2D.hpp"

#include <limits>

#include "Exception.hpp"

namespace iblbm
{
template<typename T>
Cuboid2D<T>::Cuboid2D(
    T globalXPos
  , T globalYPos
  , T deltaR
  , std::size_t nx
  , std::size_t ny
  , int refinementLevel/*=0*/)
  : mGlobalXPos{globalXPos},
    mGlobalYPos{globalYPos},
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
  : mGlobalXPos{rOrigin[0]},
    mGlobalYPos{rOrigin[1]},
    mDeltaR{deltaR},
    mNx{rLatticeExtent[0]},
    mNy{rLatticeExtent[1]},
    mWeight{std::numeric_limits<std::size_t>::max()},
    mRefinementLevel{refinementLevel},
    mOstream{std::cout, "Cuboid2D"}
{}

template<typename T>
void Cuboid2D<T>::Divide(
    std::size_t numCol
  , std::size_t numRow
  , std::vector<Cuboid2D<T>>& rCuboids) const
{
  // Need to be declared here so it can be accessed in different scopes
  auto global_x_pos_child {mGlobalXPos};
  for (gsl::index x = 0; x < numCol; ++x) {
    auto global_y_pos_child {mGlobalYPos};
    auto nx_child {(mNx + numCol - 1 - x) / numCol};
    for (gsl::index y = 0; y < numRow; ++y) {
      // Calculate the number of nodes per row/col for the child cuboid such
      // that all nodes are mapped over
      auto ny_child {(mNy + numRow - 1 - y) / numRow};
      // Create the child cuiboid to be added to the vector
      Cuboid2D<T> child_cuboid(global_x_pos_child, global_y_pos_child,
          mDeltaR, nx_child, ny_child, mRefinementLevel);
      rCuboids.push_back(child_cuboid);
      // calculate global position of bottom left corner for the next child
      global_y_pos_child += ny_child * mDeltaR;
    }
    global_x_pos_child += nx_child * mDeltaR;
  }
}

template<typename T>
void Cuboid2D<T>::Divide(
    std::size_t p
  , std::vector<Cuboid2D<T>>& rCuboids) const
{
  Expects(p > 0);
  // Initialize layout values as one column of child cuboids so we can avoid
  // undefined error caused by the default CuboidGeometry2D constructor
  std::size_t num_col {1};
  std::size_t num_row {p};

  // Ideally divide the cuboid into child cuboids with same number of nodes
  const auto best_ratio {static_cast<T>(mNx) / static_cast<T>(mNy)};
  // Difference from the ideal ratio, to track how close we got
  auto best_diff {std::numeric_limits<T>::max()};
  for (gsl::index col = 1; col <= p; ++col) {
    const auto row {p / col};
    // Make sure we aren't creating more child nodes than we're asked to
    if (col * row <= p) {
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
  const auto the_rest {p - num_col * num_row};

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
        num_same / static_cast<T>(p)};
    const auto ny_same {static_cast<std::size_t>(vol_same /
        static_cast<T>(mNx))};
    const auto nx_same {mNx};
    const auto ny_diff {mNy - ny_same};
    const auto nx_diff {mNx};
    // First child cuboid to be divided into "same" cuboids
    Cuboid2D<T> first_child(mGlobalXPos, mGlobalYPos, mDeltaR, nx_same,
        ny_same, mRefinementLevel);
    // Second child cuboid to be divided into "diff" cuboids
    Cuboid2D<T> second_child(mGlobalXPos, mGlobalYPos + mDeltaR * ny_same,
        mDeltaR, nx_diff, ny_diff, mRefinementLevel);
    first_child.Divide(num_col, num_row - the_rest, rCuboids);
    // Each row will contain one more cuboid than the "same" child
    second_child.Divide(num_col + 1, the_rest, rCuboids);
  }
  else {
    // Number of "same" child cuboid we can create, the last the_rest
    // column will be shorter to accommodate the the_rest child cuboids
    const auto num_same {num_row * (num_col - the_rest)};
    const auto vol_same {static_cast<T>(mNx) * static_cast<T>(mNy) *
        num_same / static_cast<T>(p)};
    const auto ny_same {mNy};
    // Add 0.9999 to ensure we round up properly
    const auto nx_same {static_cast<std::size_t>(vol_same /
        static_cast<T>(mNy) + 0.9999)};
    const auto ny_diff {mNy - ny_same};
    const auto nx_diff {mNx};
    Cuboid2D<T> first_child(mGlobalXPos, mGlobalYPos, mDeltaR, nx_same,
        ny_same, mRefinementLevel);
    Cuboid2D<T> second_child(mGlobalXPos + mDeltaR * nx_same, mGlobalYPos,
        mDeltaR, nx_diff, ny_diff, mRefinementLevel);
    first_child.Divide(num_col - the_rest, num_row, rCuboids);
    // Each column contain one more cuboid than the "same" child
    second_child.Divide(the_rest, num_row + 1, rCuboids);
  }
}

template<typename T>
void Cuboid2D<T>::Resize(
    gsl::index xIndex
  , gsl::index yIndex
  , std::size_t nx
  , std::size_t ny)
{
  mGlobalXPos += mDeltaR * xIndex;
  mGlobalYPos += mDeltaR * yIndex;
  mNx = nx;
  mNy = ny;
}

template<typename T>
bool Cuboid2D<T>::ContainPoint(
    T globalXpos
  , T globalYpos
  , std::size_t overlap/*=0*/) const
{
  return mGlobalXPos - T{0.5 + overlap} * mDeltaR <= globalXpos &&
      mGlobalXPos + T{mNx - 0.5 + overlap} * mDeltaR > globalXpos &&
      mGlobalYPos - T{0.5 + overlap} * mDeltaR <= globalYpos &&
      mGlobalYPos + T{mNy - 0.5 + overlap} * mDeltaR > globalYpos;
}

template<typename T>
T Cuboid2D<T>::GetGlobalXPosition() const
{
  return mGlobalXPos;
}

template<typename T>
T Cuboid2D<T>::GetGlobalYPosition() const
{
  return mGlobalYPos;
}

template<typename T>
const Vector2D<T> Cuboid2D<T>::GetOrigin() const
{
  return Vector2D<T>(mGlobalXPos, mGlobalYPos);
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
  return Vector2D<T>{mGlobalXPos + mDeltaR * rXIndex, mGlobalYPos + mDeltaR *
      rYIndex};
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
