#ifndef SRC_STREAM_ABSTRACTSTREAMMODEL_HPP_
#define SRC_STREAM_ABSTRACTSTREAMMODEL_HPP_

#include <vector>

#include "AbstractLatticeModel.hpp"

namespace iblbm
{
class AbstractStreamModel
{
 public:
  explicit AbstractStreamModel(AbstractLatticeModel& r_lm);

  virtual ~AbstractStreamModel() = default;

  virtual std::vector<std::vector<double>> Stream(
      const std::vector<std::vector<double>>& r_df) = 0;

 protected:
  AbstractLatticeModel& r_lm_;
};
}  // namespace iblbm
#endif  // SRC_STREAM_ABSTRACTSTREAMMODEL_HPP_
