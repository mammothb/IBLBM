#include "Descriptor.hpp"
#include "UnitConverter.hpp"
#include "UnitTest++.h"

namespace iblbm
{
SUITE(TestCore)
{
// Global variables for testing
static const auto g_loose_tol = 1e-6;
static const auto g_zero_tol = 1e-20;
TEST(TestUnitConverter_Constructor)
{
  auto delta_x = 1.2;
  auto delta_t = 3.4;
  auto char_phys_length = 5.6;
  auto char_phys_velocity = 7.8;
  auto phys_viscosity = 9.0;
  auto phys_density = 10.11;
  auto phys_pressure = 12.13;
  const UnitConverter<double, descriptor::D2Q9Descriptor> converter(delta_x,
      delta_t, char_phys_length, char_phys_velocity, phys_viscosity,
      phys_density, phys_pressure);

  CHECK_CLOSE(delta_x, converter.GetConversionFactorLength(), g_zero_tol);
}
}
}
