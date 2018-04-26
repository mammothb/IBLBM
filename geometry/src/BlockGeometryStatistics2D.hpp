#ifndef GEOMETRY_SRC_BLOCKGEOMETRYSTATISTICS2D_HPP_
#define GEOMETRY_SRC_BLOCKGEOMETRYSTATISTICS2D_HPP_

#include <map>

#include "gsl/gsl"

#include "OstreamManager.hpp"

namespace iblbm
{
template<typename T> class AbstractBlockGeometryInterface2D;

template<typename T>
class BlockGeometryStatistics2D
{
 public:
  /** As a convention, material 0 is Do Nothing */
  static const int mDoNothing = 0;
  /** As a convention, material 1 is fluid */
  static const int mFluid = 1;

  /**
   * Constructor
   */
  explicit BlockGeometryStatistics2D(
      AbstractBlockGeometryInterface2D<T>* pBlockGeometry);

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

  /** \return the map with the numbers of voxels for each material */
  std::map<int, std::size_t> GetMaterialToNum(bool isVerbose = false);

  /** \return mNumMaterial */
  std::size_t GetNumMaterial();

  /** \return number of voxel of the specified material */
  std::size_t GetNumVoxel(int material);

  /** \return number of voxel of the all material except material = 0 */
  std::size_t GetNumVoxel();

  /** \return the min. lattice position in each direction */
  std::vector<gsl::index> GetMinLatticeR(int material);

  /** \return the max. lattice position in each direction */
  std::vector<gsl::index> GetMaxLatticeR(int material);

  /** \return the min. phys position in each direction */
  std::vector<T> GetMinPhysR(int material);

  /** \return the max. phys position in each direction */
  std::vector<T> GetMaxPhysR(int material);

  /** \return the lattice extend as length in each direction */
  std::vector<T> GetLatticeExtent(int material);

  /** \return the phys extend as length in each direction */
  std::vector<T> GetPhysExtent(int material);

  /** \return the phys radius as length in each direction */
  std::vector<T> GetPhysRadius(int material);

  /** \return the center position */
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
      gsl::index xIndex
    , gsl::index yIndex);

  /** Returns normal that points into the fluid for paraxial surfaces */
  std::vector<int> ComputeNormal(
      gsl::index xIndex
    , gsl::index yIndex);

  /**
   * Returns normal that points into the fluid for paraxial surfaces
   * Sums all normal from nodes of target material and divides by the
   * magnitude
   */
  std::vector<T> ComputeNormal(int material);

  /**
   * Returns discrete normal with norm maxNorm that points into the fluid for
   * paraxial surfaces maxNorm = 1.1 implies only normals parallel to the
   * axises
   */
  std::vector<int> ComputeDiscreteNormal (
      int material
    , T maxNorm = 1.1);

  /**
   * Returns true if at position (x, y) and in a neighbourhood of size
   * (xOffset, yOffset) only voxels of the given material are found
   */
  bool Check(
      int material
    , gsl::index xIndex
    , gsl::index yIndex
    , unsigned xOffset
    , unsigned yOffset);

  /**
   * Returns true and a position (x, y) if there is a neighborhood of size
   * (xOffset, yOffset) around (x, y) with only voxels of the given material
   */
  bool Find(
      int material
    , unsigned xOffset
    , unsigned yOffset
    , gsl::index& rXIndex
    , gsl::index& rYIndex);

  /**
   * Prints some statistic information, i.e. the number of voxels and min.
   * and max. physical position for each different material
   */
  void Print();

 private:
  /**
   * Helper function to simplify the implementation
   */
  void TakeStatistics(
      gsl::index xIndex
    , gsl::index yIndex);

  /** Pointer to the underlying data from which the statistics is taken */
  AbstractBlockGeometryInterface2D<T>* mpBlockGeometry;
  /** Flag to indicate if an update is needed */
  bool mNeedUpdate;
  /** Number of voxel in x-direction */
  std::size_t mNx;
  /** Number of voxel in y-direction */
  std::size_t mNy;
  /** node spacing */
  T mDeltaR;
  /** number of different material numbers */
  std::size_t mNumMaterial;
  /**
   * Maps a material number to the number of this material found in the
   * super geometry
   */
  std::map<int, std::size_t> mMaterialToNum;
  /**
   * Mapping a material number to the min. lattice position in each space
   * direction
   */
  std::map<int, std::vector<gsl::index>> mMaterialToMin;
  /**
   * Mapping a material number to the max. lattice position in each space
   * direction
   */
  std::map<int, std::vector<gsl::index>> mMaterialToMax;
  /** Class specific output stream */
  mutable OstreamManager mOstream;
};
}  // namespace iblbm

#endif  // GEOMETRY_SRC_BLOCKGEOMETRYSTATISTICS2D_HPP_
