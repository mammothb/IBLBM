#ifndef SRC_STREAM_ABSTRACTSTREAMMODEL_HPP_
#define SRC_STREAM_ABSTRACTSTREAMMODEL_HPP_

#include <vector>

#include "AbstractLatticeModel.hpp"

namespace iblbm
{
class AbstractStreamModel
{
 public:
  explicit AbstractStreamModel(AbstractLatticeModel& rLatticeModel);

  virtual ~AbstractStreamModel() = default;

  virtual std::vector<std::vector<double>> Stream(
      const std::vector<std::vector<double>>& rDF) = 0;

 protected:
  AbstractLatticeModel& mrLatticeModel;
};
}  // namespace iblbm
#endif  // SRC_STREAM_ABSTRACTSTREAMMODEL_HPP_
