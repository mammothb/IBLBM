#include "Descriptor.hpp"

/** \file
 * Descriptor for various lattice arrangement.
 *  -- template instantiation
 */
namespace iblbm
{
namespace descriptor
{
// Define static const non-integral data members
// https://stackoverflow.com/questions/9213985
const std::size_t NoExternalField::sNumScalars;
const std::size_t NoExternalField::sNumSpecies;
const int NoExternalField::sForceBeginsAt;
const std::size_t NoExternalField::sSizeOfForce;
const int NoExternalField::sVelocityBeginsAt;
const std::size_t NoExternalField::sSizeOfVelocity;

const std::size_t Force2dDescriptor::sNumScalars;
const std::size_t Force2dDescriptor::sNumSpecies;
const int Force2dDescriptor::sForceBeginsAt;
const std::size_t Force2dDescriptor::sSizeOfForce;

template struct D2Q9DescriptorBase<double>;
template struct D2Q9Descriptor<double>;
}  // namespace descriptor
}  // namespace iblbm
