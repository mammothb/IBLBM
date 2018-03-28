#ifndef SRC_LATTICE_ABSTRACTLATTICEMODEL_HPP_
#define SRC_LATTICE_ABSTRACTLATTICEMODEL_HPP_

#include <cstddef>
#include <vector>

#include "IblbmEnum.hpp"

namespace iblbm
{
class AbstractLatticeModel
{
 public:
  AbstractLatticeModel(
      std::size_t numDim
    , std::size_t numDir
    , std::size_t nx
    , std::size_t ny);

  virtual ~AbstractLatticeModel() = default;

  virtual Edge DetermineOrientation(std::size_t n) = 0;

  std::size_t GetNumberOfDimensions() const;

  std::size_t GetNumberOfDirections() const;

  std::size_t GetNx() const;

  std::size_t GetNy() const;

  std::size_t GetNumberOfNodes() const;

  double GetPropagationSpeed() const;

  const std::vector<double>& rGetWeight() const;

  const std::vector<int>& rGetDistance() const;

  const std::vector<std::vector<double>>& rGetDiscreteVelocity() const;

  const std::vector<std::vector<double>>& rGetVelocity() const;

  void SetVelocity(const std::vector<std::vector<double>>& rVelocity);

 protected:
  /** Number of dimensions */
  std::size_t mNumDim;
  /** Number of directions */
  std::size_t mNumDir;
  /** Number of columns */
  std::size_t mNx;
  /** Number of rows */
  std::size_t mNy;
  /** Number of nodes */
  std::size_t mNumNodes;
  /** Propagation speed */
  double mC;
  /** Weighting factor */
  std::vector<double> mWeight;
  /** Distance (number of node) away from neighboring nodes */
  std::vector<int> mDistance;
  /** Discrete velocity */
  std::vector<std::vector<double>> mDiscreteVelocity;
  /** Velocity */
  std::vector<std::vector<double>> mVelocity;
};
}  // namespace iblbm
#endif  // SRC_LATTICE_ABSTRACTLATTICEMODEL_HPP_
