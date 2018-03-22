#ifndef SRC_BOUNDARY_ABSTRACTBOUNDARYCONDITION_HPP_
#define SRC_BOUNDARY_ABSTRACTBOUNDARYCONDITION_HPP_

#include <cstddef>
#include <vector>

#include "AbstractLatticeModel.hpp"

namespace iblbm
{
class AbstractBoundaryCondition
{
 public:
  AbstractBoundaryCondition(
      bool isBeforeStream
    , bool isDuringStream
    , AbstractLatticeModel& rLatticeModel);

  virtual ~AbstractBoundaryCondition() = default;

  virtual void UpdateNodes(
      std::vector<std::vector<double>>& rDF
    , bool isModifyStream) = 0;

  bool IsBeforeStream() const;
  bool IsDuringStream() const;
  const std::vector<std::size_t> rGetPosition() const;

 protected:
  bool mIsBeforeStream;
  bool mIsDuringStream;
  std::vector<std::size_t> mPosition;
  AbstractLatticeModel& mrLatticeModel;
};
}  // namespace iblbm

#endif // SRC_BOUNDARY_ABSTRACTBOUNDARYCONDITION_HPP_
