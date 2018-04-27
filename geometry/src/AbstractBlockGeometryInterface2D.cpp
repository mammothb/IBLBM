#include "AbstractBlockGeometryInterface2D.hpp"

#include <iostream>

#include "Descriptor.hpp"

namespace iblbm
{
template<typename T>
AbstractBlockGeometryInterface2D<T>::AbstractBlockGeometryInterface2D(
    gsl::index globalCuboidIndex/*=-1*/)
  : mGlobalCuboidIndex{globalCuboidIndex},
    mStatistics{this},
    mOstream{std::cout, "AbstractBlockGeometryInterface2D"}
{}

template<typename T>
AbstractBlockGeometryInterface2D<T>::~AbstractBlockGeometryInterface2D()
{}

template<typename T>
const gsl::index& AbstractBlockGeometryInterface2D<T>::
    rGetGlobalCuboidIndex() const
{
  return mGlobalCuboidIndex;
}

template<typename T>
const Vector2D<gsl::index> AbstractBlockGeometryInterface2D<T>::
    GetLatticeExtent() const
{
  return Vector2D<gsl::index>{static_cast<gsl::index>(GetNx()),
      static_cast<gsl::index>(GetNy())};
}

template<typename T>
void AbstractBlockGeometryInterface2D<T>::GetPhysR(
    const gsl::index latticeR[2]
  , T physR[2]) const
{
  GetPhysR(latticeR[0], latticeR[1], physR);
}

template<typename T>
int& AbstractBlockGeometryInterface2D<T>::rGetMaterial(
    const std::vector<gsl::index>& rLatticeR)
{
  return rGetMaterial(rLatticeR[0], rLatticeR[1]);
}

template<typename T>
const int& AbstractBlockGeometryInterface2D<T>::rGetMaterial(
    const std::vector<gsl::index>& rLatticeR) const
{
  return rGetMaterial(rLatticeR[0], rLatticeR[1]);
}

template<typename T>
std::size_t AbstractBlockGeometryInterface2D<T>::Clean(
    bool isVerbose/*=true*/)
{
  std::size_t counter {0};
  for (gsl::index x {0}; x < GetNx(); ++x) {
    for (gsl::index y {0}; y < GetNy(); ++y) {
      if (rGetMaterial(x, y) != 1 && rGetMaterial(x, y) != 0 &&
          GetMaterial(x + 1, y) != 1 && GetMaterial(x + 1, y + 1) != 1 &&
          GetMaterial(x, y + 1) != 1 && GetMaterial(x - 1, y + 1) != 1 &&
          GetMaterial(x - 1, y) != 1 && GetMaterial(x - 1, y - 1) != 1 &&
          GetMaterial(x, y - 1) != 1 && GetMaterial(x + 1, y - 1) != 1) {
        rGetMaterial(x, y) = 0;
        ++counter;
      }
    }
  }
  if (isVerbose) {
    mOstream << "cleaned "<< counter << " outer boundary voxel(s)" <<
        std::endl;
  }
  return counter;
}

template<typename T>
std::size_t AbstractBlockGeometryInterface2D<T>::OuterClean(
    bool isVerbose/*=true*/)
{
  std::size_t counter {0};
  for (gsl::index x {0}; x < GetNx(); ++x) {
    for (gsl::index y {0}; y < GetNy(); ++y) {
      if (rGetMaterial(x, y) == 1 && (
          GetMaterial(x + 1, y) == 0 || GetMaterial(x + 1, y + 1) == 0 ||
          GetMaterial(x, y + 1) == 0 || GetMaterial(x - 1, y + 1) == 0 ||
          GetMaterial(x - 1, y) == 0 || GetMaterial(x - 1, y - 1) == 0 ||
          GetMaterial(x, y - 1) == 0 || GetMaterial(x + 1, y - 1) == 0)) {
        rGetMaterial(x, y) = 0;
        ++counter;
      }
    }
  }
  if (isVerbose) {
    mOstream << "cleaned "<< counter << " outer fluid voxel(s)" << std::endl;
  }
  return counter;
}

template<typename T>
std::size_t AbstractBlockGeometryInterface2D<T>::InnerClean(
    bool isVerbose/*=true*/)
{
  std::size_t counter {0};

  for (gsl::index x {0}; x < GetNx(); ++x) {
    for (gsl::index y {0}; y < GetNy(); ++y) {
      if (rGetMaterial(x, y) != 1 && rGetMaterial(x, y) != 0) {
        if (GetMaterial(x, y + 1) == 1 && GetMaterial(x, y - 1) == 1 &&
            GetMaterial(x - 1, y) == 1) {
          rGetMaterial(x, y) = 1;
          ++counter;
        }
        if (GetMaterial(x, y + 1) == 1 && GetMaterial(x, y - 1) == 1 &&
            GetMaterial(x + 1, y) == 1) {
          rGetMaterial(x, y) = 1;
          ++counter;
        }
        if (GetMaterial(x + 1, y) == 1 && GetMaterial(x - 1, y) == 1 &&
            GetMaterial(x, y + 1) == 1) {
          rGetMaterial(x, y) = 1;
          ++counter;
        }
        if (GetMaterial(x + 1, y) == 1 && GetMaterial(x - 1, y) == 1 &&
            GetMaterial(x, y - 1) == 1) {
          rGetMaterial(x, y) = 1;
          ++counter;
        }
      }
    }
  }
  if (isVerbose) {
    mOstream << "cleaned "<< counter << " inner boundary voxel(s)" <<
        std::endl;
  }
  return counter;
}

template<typename T>
std::size_t AbstractBlockGeometryInterface2D<T>::InnerClean(
    int fromMaterial
  , bool isVerbose/*=true*/)
{
  std::size_t counter {0};

  for (gsl::index x {0}; x < GetNx(); ++x) {
    for (gsl::index y {0}; y < GetNy(); ++y) {
      if (rGetMaterial(x, y) != 1 && rGetMaterial(x, y) != 0 &&
          rGetMaterial(x, y) == fromMaterial) {
        if (GetMaterial(x, y + 1) == 1 && GetMaterial(x, y - 1) == 1 &&
            GetMaterial(x - 1, y) == 1) {
          rGetMaterial(x, y) = 1;
          ++counter;
        }
        if (GetMaterial(x, y + 1) == 1 && GetMaterial(x, y - 1) == 1 &&
            GetMaterial(x + 1, y) == 1) {
          rGetMaterial(x, y) = 1;
          ++counter;
        }
        if (GetMaterial(x + 1, y) == 1 && GetMaterial(x - 1, y) == 1 &&
            GetMaterial(x, y + 1) == 1) {
          rGetMaterial(x, y) = 1;
          ++counter;
        }
        if (GetMaterial(x + 1, y) == 1 && GetMaterial(x - 1, y) == 1 &&
            GetMaterial(x, y - 1) == 1) {
          rGetMaterial(x, y) = 1;
          ++counter;
        }
      }
    }
  }
  if (isVerbose) {
    mOstream << "cleaned "<< counter <<
        " inner boundary voxel(s) of Type " << fromMaterial << std::endl;
  }
  return counter;
}

template<typename T>
bool AbstractBlockGeometryInterface2D<T>::CheckForErrors(
    bool isVerbose/*=true*/) const
{
  bool error {false};

  for (gsl::index x = 0; x < GetNx(); x++) {
    for (gsl::index y = 0; y < GetNy(); y++) {
      if (rGetMaterial(x, y) == 0 && (
          GetMaterial(x + 1, y) == 1 || GetMaterial(x + 1, y + 1) == 1 ||
          GetMaterial(x, y + 1) == 1 || GetMaterial(x - 1, y + 1) == 1 ||
          GetMaterial(x - 1, y) == 1 || GetMaterial(x - 1, y - 1) == 1 ||
          GetMaterial(x, y - 1) == 1 || GetMaterial(x + 1, y - 1) == 1)) {
        error = true;
      }
    }
  }
  if (isVerbose) {
    if (error) {
      mOstream << "error!" << std::endl;
    }
    else {
      mOstream << "the model is correct!" << std::endl;
    }
  }
  return error;
}

template<typename T>
void AbstractBlockGeometryInterface2D<T>::Rename(
    int fromMaterial
  , int toMaterial)
{
  for (gsl::index x {0}; x < GetNx(); ++x) {
    for (gsl::index y {0}; y < GetNy(); ++y) {
      if (rGetMaterial(x, y) == fromMaterial)
          rGetMaterial(x, y) = toMaterial;
    }
  }
}

template<typename T>
void AbstractBlockGeometryInterface2D<T>::Rename(
    int fromMaterial
  , int toMaterial
  , AbstractIndicatorFunctor2D<T>& rCondition)
{
  T phys_R[2];
  for (gsl::index x {0}; x < GetNx(); ++x) {
    for (gsl::index y = 0; y < GetNy(); ++y) {
      if (rGetMaterial(x, y) == fromMaterial) {
        auto is_inside {false};
        GetPhysR(x, y, phys_R);
        // TODO remove the use of C-arrays
        rCondition(gsl::make_span(&is_inside), std::vector<T>(
            std::begin(phys_R), std::end(phys_R)));
        if (is_inside) rGetMaterial(x, y) = toMaterial;
      }
    }
  }
}

template<typename T>
void AbstractBlockGeometryInterface2D<T>::Rename(
    int fromMaterial
  , int toMaterial
  , unsigned xOffset
  , unsigned yOffset)
{
  auto int_x_offset {static_cast<int>(xOffset)};
  auto int_y_offset {static_cast<int>(yOffset)};
  auto tmp_material {9999};
  for (gsl::index x {0}; x < GetNx(); ++x) {
    for (gsl::index y {0}; y < GetNy(); ++y) {
      if (rGetMaterial(x, y) == fromMaterial) {
        bool found {true};
        for (gsl::index x_offset {-int_x_offset}; x_offset <= int_x_offset;
            ++x_offset) {
          for (gsl::index y_offset {-int_y_offset}; y_offset <= int_y_offset;
              ++y_offset) {
            if (GetMaterial(x + x_offset, y + y_offset) != fromMaterial &&
                GetMaterial(x + x_offset, y + y_offset) != tmp_material) {
              found = false;
            }
          }
        }
        if (found) rGetMaterial(x, y) = tmp_material;
      }
    }
  }
  Rename(tmp_material, toMaterial);
}

template<typename T>
void AbstractBlockGeometryInterface2D<T>::Rename(
    int fromMaterial
  , int toMaterial
  , int testMaterial
  , const std::vector<int>& rTestDirection)
{
  for (gsl::index x {0}; x < GetNx(); ++x) {
    for (gsl::index y {0}; y < GetNy(); ++y) {
      if (rGetMaterial(x, y) == fromMaterial) {
        // flag that indicates the renaming of the current voxel, valid
        // voxels are not renamed
        bool is_valid {true};
        for (gsl::index x_offset {std::min(rTestDirection[0], 0)};
            x_offset <= std::max(rTestDirection[0], 0); ++x_offset) {
          for (gsl::index y_offset {std::min(rTestDirection[1], 0)};
              y_offset <= std::max(rTestDirection[1], 0); ++y_offset) {
            if ((x_offset != 0 || y_offset != 0) &&
                GetMaterial(x + x_offset, y + y_offset) != testMaterial) {
              is_valid = false;
            }
          }
        }
        if (!is_valid) rGetMaterial(x, y) = toMaterial;
      }
    }
  }
}

template<typename T>
void AbstractBlockGeometryInterface2D<T>::Rename(
    int fromMaterial
  , int toMaterial
  , int fluidMaterial
  , AbstractIndicatorFunctor2D<T>& rCondition
  , const std::vector<int>& rDiscreteNormal)
{
  Rename(fromMaterial, toMaterial, rCondition);
  T phys_R[2];
  for (gsl::index x {0}; x < GetNx(); ++x) {
    for (gsl::index y = 0; y < GetNy(); ++y) {
      if (rGetMaterial(x, y) == toMaterial) {
        auto is_inside {false};
        GetPhysR(x, y, phys_R);
        // TODO remove the use of C-arrays
        rCondition(gsl::make_span(&is_inside), std::vector<T>(
            std::begin(phys_R), std::end(phys_R)));
        if (is_inside) {
          if (GetMaterial(x + rDiscreteNormal[0], y + rDiscreteNormal[1]) != fluidMaterial ||
              GetMaterial(x + 2 * rDiscreteNormal[0], y + 2 * rDiscreteNormal[1]) != fluidMaterial ||
              GetMaterial(x - rDiscreteNormal[0], y - rDiscreteNormal[1]) != 0) {
            rGetMaterial(x, y) = fromMaterial;
          }
        }
      }
    }
  }
}

template<typename T>
void AbstractBlockGeometryInterface2D<T>::Rename(
    int fromMaterial
  , int toMaterial
  , int fluidMaterial
  , AbstractIndicatorFunctor2D<T>& rCondition)
{
  Rename(fromMaterial, toMaterial, rCondition);
  auto discrete_normal {rGetStatistics().ComputeDiscreteNormal(toMaterial)};
  T phys_R[2];
  for (gsl::index x = 0; x < GetNx(); ++x) {
    for (gsl::index y = 0; y < GetNy(); ++y) {
      if (rGetMaterial(x, y) == toMaterial) {
        auto is_inside {false};
        GetPhysR(x, y, phys_R);
        // TODO remove the use of C-arrays
        rCondition(gsl::make_span(&is_inside), std::vector<T>(
            std::begin(phys_R), std::end(phys_R)));
        if (is_inside) {
          if (GetMaterial(x + discrete_normal[0], y + discrete_normal[1]) != fluidMaterial ||
              GetMaterial(x + 2 * discrete_normal[0], y + 2 * discrete_normal[1]) != fluidMaterial ||
              GetMaterial(x - discrete_normal[0], y - discrete_normal[1]) != 0 ) {
            rGetMaterial(x, y) = fromMaterial;
          }
        }
      }
    }
  }
}

template<typename T>
template<typename V, template<typename U> class Lattice>
bool AbstractBlockGeometryInterface2D<T>::FindStreamDirections(
    gsl::index xIndex
  , gsl::index yIndex
  , int material
  , const std::list<int>& rBulkMaterials
  , std::array<bool, Lattice<V>::sQ>& rStreamDirections)
{
  if (GetMaterial(xIndex, yIndex) != material) return false;
  bool found {false};
  rStreamDirections.fill(false);
  for (gsl::index q {1}; q < Lattice<V>::sQ; ++q) {
    for (const auto& r_it : rBulkMaterials) {
      // Ignore the rest of the list if we have already found the stream
      // direction
      if (rStreamDirections[q]) break;
      if (rGetMaterial(xIndex + Lattice<V>::sE[q][0],
          yIndex + Lattice<V>::sE[q][1]) == r_it) {
        rStreamDirections[q] = true;
        found = true;
      }
    }
  }
  return found;
}

// explicit instantiation
template class AbstractBlockGeometryInterface2D<double>;
template bool AbstractBlockGeometryInterface2D<double>::
    FindStreamDirections<double, descriptor::D2Q9Descriptor>(
    gsl::index xIndex
  , gsl::index yIndex
  , int material
  , const std::list<int>& rBulkMaterials
  , std::array<bool, descriptor::D2Q9Descriptor<double>::sQ>&
        rStreamDirections);
}  // namespace iblbm
