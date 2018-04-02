#include "Cuboid2D.hpp"

#include <limits>

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
  , const Vector2D<std::size_t>& rExtent
  , int refinementLevel/*=0*/)
  : mGlobalXPos{rOrigin[0]},
    mGlobalYPos{rOrigin[1]},
    mDeltaR{deltaR},
    mNx{rExtent[0]},
    mNy{rExtent[1]},
    mWeight{std::numeric_limits<std::size_t>::max()},
    mRefinementLevel{refinementLevel},
    mOstream{std::cout, "Cuboid2D"}
{}

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
