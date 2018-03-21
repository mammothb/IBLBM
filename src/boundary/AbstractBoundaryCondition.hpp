#ifndef SRC_BOUNDARY_ABSTRACTBOUNDARYCONDITION_HPP_
#define SRC_BOUNDARY_ABSTRACTBOUNDARYCONDITION_HPP_

#include <cstddef>
#include <vector>

#include "AbstractLatticeModel.hpp"

namespace iblbm
{
class AbstractBoundaryCondition {
 public:
  AbstractBoundaryCondition(
      bool is_before_stream
    , bool is_during_stream
    , AbstractLatticeModel& r_lm);

  virtual ~AbstractBoundaryCondition() = default;

  virtual void UpdateNodes(
      std::vector<std::vector<double>>& r_df
    , bool is_modify_stream) = 0;

  bool IsBeforeStream() const;
  bool IsDuringStream() const;
  const std::vector<std::size_t> rGetPosition() const;

 protected:
  bool is_before_stream_;
  bool is_during_stream_;
  std::vector<std::size_t> position_;
  AbstractLatticeModel& r_lm_;
};
}  // namespace iblbm

#endif // SRC_BOUNDARY_ABSTRACTBOUNDARYCONDITION_HPP_
