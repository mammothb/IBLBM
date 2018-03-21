#include "IblbmEnum.hpp"

#include <iostream>
#include <stdexcept>

namespace iblbm
{
std::ostream& operator<<(std::ostream& r_os, const Edge edge)
{
  switch(edge) {
  case Edge::NOT_EDGE:
    r_os << "NOT_EDGE";
    break;
  case Edge::RIGHT:
    r_os << "RIGHT";
    break;
  case Edge::UPPER:
    r_os << "UPPER";
    break;
  case Edge::LEFT:
    r_os << "LEFT";
    break;
  case Edge::LOWER:
    r_os << "LOWER";
    break;
  case Edge::UPPER_RIGHT:
    r_os << "UPPER_RIGHT";
    break;
  case Edge::UPPER_LEFT:
    r_os << "UPPER_RIGHT";
    break;
  case Edge::LOWER_LEFT:
    r_os << "LOWER_LEFT";
    break;
  case Edge::LOWER_RIGHT:
    r_os << "LOWER_RIGHT";
    break;
  default:
    throw std::runtime_error("Invalid Edge value");
  }
}
}  // namespace iblbm
