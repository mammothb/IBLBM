#ifndef GEOMETRY_SRC_SUPERGEOMETRY2D_HPP_
#define GEOMETRY_SRC_SUPERGEOMETRY2D_HPP_

#include "BlockGeometry2D.hpp"
#include "BlockGeometryView2D.hpp"
#include "SuperGeometryStatistics2D.hpp"
#include "SuperStructure2D.hpp"

namespace iblbm
{
template<typename T>
class SuperGeometry2D : public SuperStructure2D<T>
{
 public:
  /**
   * Constructor
   *
   * \param rCuboidGeometry Reference to CuboidGeometry which holds the grid
   *        structure
   * \param rLoadBalancer Reference to LoadBalancer to evenly distribute the
   *        load across processes
   * \param overlap Size of ghost cell layer
   */
  SuperGeometry2D(
      CuboidGeometry2D<T>& rCuboidGeometry
    , LoadBalancer<T>& rLoadBalancer
    , std::size_t overlap = 2);

  /**
   * Copy constructor
   */
  SuperGeometry2D(const SuperGeometry2D<T>& rRhs);

  /**
   * Copy assignment
   */
  SuperGeometry2D<T>& operator=(const SuperGeometry2D<T>& rRhs);

  /**
   * Interface for the communicator class: Write access to the memory of the
   * data of the super structure
   */
  bool* operator()(
      gsl::index localCuboidIndex
    , gsl::index xIndex
    , gsl::index yIndex
    , gsl::index dataIndex) override;

  /**
   * Updates the super geometry at the boundaries if needed and afterwards
   * the statistics if needed
   */
  void UpdateStatistics(bool isVerbose = true);

  /**
   * Changes all materials which are not 0 or 1 to 0 if there is no neighbor
   * with material 1. This removes things like a block of boundary nodes
   * since keeping just its outer shell has the same hydrodynamic effect
   */
  std::size_t Clean(bool isVerbose = true);

  /**
   * Changes all materials with material 1 to 0 if there is a neighbor with
   * material 0
   */
  std::size_t OuterClean(bool isVerbose = true);

  /**
   * Changes all materials which are not 0 or 1 to 1 if there is a non robust
   * constiallation
   */
  std::size_t InnerClean(bool isVerbose = true);

  /**
   * Changes all materials with material fromM to 1 if there is a non robust
   * constiallation
   */
  std::size_t InnerClean(
      int boundaryMaterial
    , bool isVerbose = true);

  /**
   * Checks for errors (searches for all outer voxels (=0) with an inner
   * voxel (=1) as a direct neighbor)
   */
  bool CheckForErrors(bool isVerbose = true);

  /** Replaces all material numbers (fromMaterial) to another (toMaterial) */
  virtual void Rename(
      int fromMaterial
    , int toMaterial);

  /**
   * Replaces all material numbers (fromMaterial) to another (toMaterial) if
   * an indicator functor condition is fulfilled
   */
  virtual void Rename(
      int fromMaterial
    , int toMaterial
    , AbstractIndicatorFunctor2D<T>& rCondition);

  /**
   * Replaces all material numbers (fromMaterial) to another (toMaterial) if
   * all materials in the neighborhood (x - xOffset ... x + xOffset), are of
   * the original material number (fromMaterial)
   */
  virtual void Rename(
      int fromMaterial
    , int toMaterial
    , unsigned xOffset
    , unsigned yOffset);

  /**
   * Replaces all material numbers (fromMaterial) to another (toMaterial) if
   * all materials in (x + 1 ... x + rTestDirection[0]) are of another
   * material number (testMaterial)
   */
  virtual void Rename(
      int fromMaterial
    , int toMaterial
    , int testMaterial
    , const std::vector<int>& rTestDirection);

  /**
   * Replaces all material numbers (fromMaterial) to another (toMaterial) if
   * all materials in (iX+discreteNormal[0],iX+2*discreteNormal[0]), are of
   * another material number (fluidM) and if an indicator functor condition
   * is fulfilled, the discreteNormal is computed from all fromMaterial which
   * fulfill the indicator functor condition
   */
  virtual void Rename(
      int fromBoundaryMaterial
    , int toBoundaryMaterial
    , int fluidMaterial
    , AbstractIndicatorFunctor2D<T>& rCondition);

  /**
   * \return Read only access to the dim of the data of the super structure
   */
  std::size_t GetDataSize() const override;

  /**
   * \return Read only access to the data type dim of the data of the super
   * structure
   */
  std::size_t GetSizeofDataType() const override;

  /**
   * Write access to the material numbers, error handling: stops the program
   * if data is not available
   */
  int& rGetMaterial(
      gsl::index globalCuboidIndex
    , gsl::index xIndex
    , gsl::index yIndex);

  /**
   * Read only access to the material numbers
   */
  const int& rGetMaterial(
      gsl::index globalCuboidIndex
    , gsl::index xIndex
    , gsl::index yIndex) const;

  /**
   * Read only access to the material numbers with global communication to
   * all ranks
   */
  int GetMaterialAndCommunicate(
      gsl::index globalCuboidIndex
    , gsl::index xIndex
    , gsl::index yIndex) const;

  /**
   * Write access to the material numbers, error handling: stops the program
   * if data is not available
   */
  int& rGetMaterial(
      gsl::index globalCuboidIndex
    , const std::vector<gsl::index> rLatticeR);

  /**
   * Read only access to the material numbers
   */
  const int& rGetMaterial(
      gsl::index globalCuboidIndex
    , const std::vector<gsl::index> rLatticeR) const;

  /**
   * Read only access to the material numbers with global communication to
   * all ranks
   */
  int GetMaterialAndCommunicate(
      gsl::index globalCuboidIndex
    , const std::vector<gsl::index> rLatticeR) const;

  /**
   * Transforms a lattice to physical position (SI units)
   *
   * \param globalCuboidIndex global cuboid index
   * \param xIndex lattice position in x direction
   * \param yIndex lattice position in y direction
   */
  Vector2D<T> GetPhysR(
      gsl::index globalCuboidIndex
    , gsl::index xIndex
    , gsl::index yIndex) const;

  /**
   * Transforms a lattice to physical position (SI units)
   *
   * \param rLatticeR vector containing lattice position
   */
  Vector2D<T> GetPhysR(
      gsl::index globalCuboidIndex
    , const std::vector<gsl::index>& rLatticeR) const;

  /**
   * Transforms a lattice to physical position (SI units)
   *
   * \param latticeR array containing lattice position
   * \param physR array for physical position output
   */
  void GetPhysR(
      gsl::index globalCuboidIndex
    , const gsl::index latticeR[2]
    , T physR[2]) const;

  /**
   * Transforms a lattice to physical position (SI units)
   *
   * \param globalCuboidIndex global cuboid index
   * \param xIndex lattice position in x direction
   * \param yIndex lattice position in y direction
   * \param physR array for physical position output
   */
  void GetPhysR(
      const gsl::index globalCuboidIndex
    , const gsl::index xIndex
    , const gsl::index yIndex
    , T physR[2]) const;

  /** Read and write access to a single extended block geometry */
  BlockGeometry2D<T>& rGetExtendedBlockGeometry(gsl::index localCuboidIndex);

  /** Read-only access to a single extended block geometry */
  const BlockGeometry2D<T>& rGetExtendedBlockGeometry(
      gsl::index localCuboidIndex) const;

  /** Read and write access to a single extended block geometry */
  BlockGeometryView2D<T>& rGetBlockGeometry(gsl::index localCuboidIndex);

  /** Read-only access to a single extended block geometry */
  const BlockGeometryView2D<T>& rGetBlockGeometry(
      gsl::index localCuboidIndex) const;

  /** Returns the statistics object */
  SuperGeometryStatistics2D<T>& rGetStatistics();

  /**
   * Read and write access to the statistic status flag, update needed = true
   */
  bool& rGetStatisticsStatus();

  /** Read only access to the statistic status flag, update needed = true */
  const bool& rGetStatisticsStatus() const;

  /** Prints some information about the super geometry */
  void Print();

 private:
  /** Vector of block geometries with overlap */
  std::vector<BlockGeometry2D<T>> mExtendedBlockGeometries;
  /** Vector of block geometries without overlap */
  std::vector<BlockGeometryView2D<T>> mBlockGeometries;
  /** Statistics class */
  SuperGeometryStatistics2D<T> mStatistics;
  /** class specific output stream */
  mutable OstreamManager mOstream;
};
}  // namespace iblbm

#endif  // GEOMETRY_SRC_SUPERGEOMETRY2D_HPP_
