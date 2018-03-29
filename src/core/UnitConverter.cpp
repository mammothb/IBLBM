#include "UnitConverter.hpp"

#include "Descriptor.hpp"

namespace iblbm
{
template<typename T, template<typename U> class Lattice>
void UnitConverter<T, Lattice>::Print() const
{
  mOstream <<
      "============= UnitConverter information =============" << std::endl <<
      "Parameter (unit)                  | Variable/Symbol | Value" << std::endl <<
      "Resolution                        | N               | " << GetResolution() << std::endl <<
      "Lattice velocity                  | Char. lattice u | " << GetCharLatticeVelocity() << std::endl <<
      "Lattice relaxation time           | tau             | " << GetRelaxationTime() << std::endl <<
      "Lattice relaxation frequency      | omega           | " << GetRelaxationFrequency() << std::endl <<
      "Char. length (m)                  | Char. L         | " << GetCharPhysLength() << std::endl <<
      "Char. speed (m/s)                 | Char. u         | " << GetCharPhysVelocity() << std::endl <<
      "Phys. kinematic viscosity (m^2/s) | Char. nu        | " << GetPhysViscosity() << std::endl <<
      "Phys. density (kg/m^d)            | Char. rho       | " << GetPhysDensity() << std::endl <<
      "Char. pressure (N/m^2)            | Char. Phys. P   | " << GetCharPhysPressure() << std::endl <<
      "-----------------------------------------------------" << std::endl <<
      "Conversion Factor (unit) | Variable/Symbol | Value" << std::endl <<
      "Voxel length (m)         | Phys. Delta x   | " << GetConversionFactorLength() << std::endl <<
      "Time step (s)            | Phys. Delta t   | " << GetConversionFactorTime() << std::endl <<
      "Velocity factor (m/s)    | Phys. u         | " << GetConversionFactorVelocity() << std::endl <<
      "Density factor (kg/m^3)  | Phys. rho       | " << GetConversionFactorDensity() <<  std::endl <<
      "Mass factor (kg)         | Phys. M         | " << GetConversionFactorMass() << std::endl <<
      "Viscosity factor (m^2/s) | Phys. nu        | " << GetConversionFactorViscosity() << std::endl <<
      "Force factor (N)         | Phys. F         | " << GetConversionFactorForce() << std::endl <<
      "Pressure factor (N/m^2)  | Phys. P         | " << GetConversionFactorPressure() << std::endl <<
      "--------------------------------------------" << std::endl;
}
template class UnitConverter<double, descriptor::D2Q9Descriptor>;
template class UnitConverter<double, descriptor::ForcedD2Q9Descriptor>;
}  // namespace iblbm
