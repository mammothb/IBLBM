#ifndef SRC_LATTICE_ABSTRACTLATTICEMODEL_HPP_
#define SRC_LATTICE_ABSTRACTLATTICEMODEL_HPP_

#include <cstddef>
#include <vector>

namespace iblbm
{
class AbstractLatticeModel
{
 public:
  AbstractLatticeModel(
      std::size_t num_dim
    , std::size_t num_dir
    , std::size_t nx
    , std::size_t ny);

  ~AbstractLatticeModel() = default;

  std::size_t GetNumberOfDimensions() const;

  std::size_t GetNumberOfDirections() const;

  std::size_t GetNx() const;

  std::size_t GetNy() const;

  std::size_t GetNumberOfNodes() const;

  double GetPropagationSpeed() const;

 protected:
  /** Number of dimensions */
  std::size_t num_dim_;
  /** Number of directions */
  std::size_t num_dir_;
  /** Number of columns */
  std::size_t nx_;
  /** Number of rows */
  std::size_t ny_;
  /** Number of nodes */
  std::size_t num_nodes_;
  /** Propagation speed */
  double c_;
  /** Weighting factor */
  std::vector<double> weight_;
  /** Discrete velocity */
  std::vector<std::vector<double>> e_;
  /** Velocity */
  std::vector<std::vector<double>> u_;
};
}  // namespace iblbm
#endif  // SRC_LATTICE_ABSTRACTLATTICEMODEL_HPP_
