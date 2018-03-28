#include "UnitConverter.hpp"

#include "Descriptor.hpp"

namespace iblbm
{
template class UnitConverter<double, descriptor::D2Q9Descriptor>;
template class UnitConverter<double, descriptor::ForcedD2Q9Descriptor>;
}  // namespace iblbm
