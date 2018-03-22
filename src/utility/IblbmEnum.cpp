#include "IblbmEnum.hpp"

#include <iostream>
#include <stdexcept>

namespace iblbm
{
std::ostream& operator<<(std::ostream& rOs, const Edge edge)
{
  switch(edge) {
  case Edge::NOT_EDGE:
    rOs << "NOT_EDGE";
    break;
  case Edge::RIGHT:
    rOs << "RIGHT";
    break;
  case Edge::UPPER:
    rOs << "UPPER";
    break;
  case Edge::LEFT:
    rOs << "LEFT";
    break;
  case Edge::LOWER:
    rOs << "LOWER";
    break;
  case Edge::UPPER_RIGHT:
    rOs << "UPPER_RIGHT";
    break;
  case Edge::UPPER_LEFT:
    rOs << "UPPER_RIGHT";
    break;
  case Edge::LOWER_LEFT:
    rOs << "LOWER_LEFT";
    break;
  case Edge::LOWER_RIGHT:
    rOs << "LOWER_RIGHT";
    break;
  default:
    throw std::runtime_error("Invalid Edge value");
  }
}
}  // namespace iblbm
