#ifndef SRC_STREAM_ABSTRACTSTREAMMODEL_HPP_
#define SRC_STREAM_ABSTRACTSTREAMMODEL_HPP_

#include "AbstractLatticeModel.hpp"

namespace iblbm
{
class AbstractStreamModel
{
 public:
  explicit AbstractStreamModel(AbstractLatticeModel& r_lm);

  virtual ~AbstractStreamModel() = default;



};
}  // namespace iblbm
#endif  // SRC_STREAM_ABSTRACTSTREAMMODEL_HPP_
