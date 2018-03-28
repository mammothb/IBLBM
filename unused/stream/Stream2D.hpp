#ifndef SRC_STREAM_STREAM2D_HPP_
#define SRC_STREAM_STREAM2D_HPP_

#include "AbstractStreamModel.hpp"

namespace iblbm
{
class Stream2D : public AbstractStreamModel
{
 public:
  explicit Stream2D(AbstractLatticeModel& rLatticeModel);

  ~Stream2D() = default;

  std::vector<std::vector<double>> Stream(
      const std::vector<std::vector<double>>& rDF);
};
}  // namespace
#endif  // SRC_STREAM_STREAM2D_HPP_
