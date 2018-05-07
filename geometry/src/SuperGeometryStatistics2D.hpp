#ifndef GEOMETRY_SRC_SUPERGEOMETRYSTATISTICS2D_HPP_
#define GEOMETRY_SRC_SUPERGEOMETRYSTATISTICS2D_HPP_

#include <map>
#include <vector>

#include "gsl/gsl"
#include "OstreamManager.hpp"

namespace iblbm
{
template<typename T> class SuperGeometry2D;

template<typename T>
class SuperGeometryStatistics2D
{
 public:
  /**
   * Constructor
   */
  explicit SuperGeometryStatistics2D(SuperGeometry2D<T>* pSuperGeometry);

  /**
   * Copy constructor
   */
  SuperGeometryStatistics2D(const SuperGeometryStatistics2D<T>& rRhs);

  /**
   * Copy assignment
   */
  SuperGeometryStatistics2D<T>& operator=(
      const SuperGeometryStatistics2D<T>& rRhs);

  /** Updates the statistics if it is really needed */
  void Update(bool isVerbose = false);

  /**
   * Read and write access to a flag, which indicates if an update is needed
   */
  bool& rGetStatus();
  /**
   * Read-only access to a flag, which indicates if an update is needed
   */
  const bool& rGetStatus() const;

  /** Returns the number of different materials */
  std::size_t GetNumMaterial();

  /** Returns the number of voxels for a given material number */
  std::size_t GetNumVoxel(int material);

  /** Returns the number of voxels for all material number except 0 */
  std::size_t GetNumVoxel();

  /** Returns the min. phys position in each direction */
  std::vector<T> GetMinPhysR(int material);

  /** Returns the max. phys position in each direction */
  std::vector<T> GetMaxPhysR(int material);

  /** Returns the phys extend as length in each direction */
  std::vector<T> GetPhysExtent(int material);

  /** Returns the phys radius as length in each direction */
  std::vector<T> GetPhysRadius(int material);

  /** Returns the center position */
  std::vector<T> GetCenterPhysR(int material);

  /**
   * \return the boundary type which is characterized by a discrete normal.
   *         the discrete normal contains 3 element:
   *         0: boundary type (0 = straight, 1 = external corner,
   *                           2 = internal corner)
   *         1: unit normal in x-axis
   *         2: unit normal in y-axis
   */
  std::vector<int> GetType(
      gsl::index globalCuboidIndex
    , gsl::index xIndex
    , gsl::index yIndex);

  /** Returns normal that points into the fluid for paraxial surfaces */
  std::vector<T> ComputeNormal(int material);

  /**
   * Returns discrete normal with norm maxNorm that points into the fluid for
   * paraxial surfaces maxNorm=1.1 implies only normals parallel to the axes
   */
  std::vector<int> ComputeDiscreteNormal(
      int material
    , T maxNorm = 1.1);

  /**
   * Prints some statistic information, i.e. the number of voxels and min.
   * and max. physical position for each different material
   */
  void Print();

 private:
  /** Pointer to the underlying data for the statistics */
  SuperGeometry2D<T>* mpSuperGeometry;
  /** Flag to determine if an update is needed */
  bool mNeedUpdate;
  /** Size of ghost layer */
  std::size_t mOverlap;
  /** Number of different material numbers */
  std::size_t mNumMaterials;
  /**
   * Mapping a material number to the number of this kind found in the super
   * geometry
   */
  std::map<int, std::size_t> mMaterialToNum;
  /**
   * Mapping a material number to the min. physical position in each space
   * direction
   */
  std::map<int, std::vector<T>> mMaterialToMin;
  /**
   * Mapping a material number to the max. physical position in each space
   * direction
   */
  std::map<int, std::vector<T>> mMaterialToMax;
  /** Class specific output stream */
  mutable OstreamManager mOstream;
};
}  // namespace iblbm

#endif  // GEOMETRY_SRC_SUPERGEOMETRYSTATISTICS2D_HPP_
