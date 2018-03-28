#include <cmath>

#include "UnitTest++/UnitTest++.h"
#include "Descriptor.hpp"
#include "UnitConverter.hpp"

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
  auto char_phys_pressure = 12.13;
  // Variable for expected value calculation
  auto lattice_length = 14u;
  auto lattice_time = 15u;
  auto lattice_velocity = 16.17;
  auto lattice_mass = 18.19;
  auto lattice_force = 20.21;
  auto lattice_pressure = 22.23;

  const UnitConverter<double, descriptor::D2Q9Descriptor> converter(delta_x,
      delta_t, char_phys_length, char_phys_velocity, phys_viscosity,
      phys_density, char_phys_pressure);

  // Expected values
  auto conv_length = delta_x;
  auto conv_time = delta_t;
  auto conv_velocity = conv_length / conv_time;
  auto conv_density = phys_density;
  auto conv_mass = conv_density * conv_length * conv_length * conv_length;
  auto conv_viscosity = conv_length * conv_length / conv_time;
  auto conv_force = conv_mass * conv_length / conv_time / conv_time;
  auto conv_pressure = conv_force / conv_length / conv_length;
  auto resolution = static_cast<unsigned>(char_phys_length / conv_length +
      0.5);
  auto relaxation_time = phys_viscosity / conv_viscosity *
      descriptor::D2Q9Descriptor<double>::sInvCsSqr + 0.5;
  auto char_lattice_velocity = char_phys_velocity / conv_velocity;
  auto reynolds = char_phys_velocity * char_phys_length / phys_viscosity;
  auto phys_length = conv_length * static_cast<double>(lattice_length);
  auto phys_time = conv_time * static_cast<double>(lattice_time);
  auto phys_velocity = conv_velocity * lattice_velocity;
  auto lattice_density = phys_density / conv_density;
  auto phys_mass = conv_mass * lattice_mass;
  auto lattice_viscosity = phys_viscosity / conv_viscosity;
  auto phys_force = conv_force * lattice_force;
  auto phys_pressure = conv_pressure * lattice_pressure + char_phys_pressure;

  CHECK_EQUAL(resolution, converter.GetResolution());
  CHECK_CLOSE(relaxation_time, converter.GetRelaxationTime(), g_zero_tol);
  CHECK_CLOSE(1.0 / relaxation_time, converter.GetRelaxationFrequency(),
      g_zero_tol);
  CHECK_CLOSE(char_phys_length, converter.GetCharPhysLength(), g_zero_tol);
  CHECK_CLOSE(char_phys_velocity, converter.GetCharPhysVelocity(),
      g_zero_tol);
  CHECK_CLOSE(char_lattice_velocity, converter.GetCharLatticeVelocity(),
      g_zero_tol);
  CHECK_CLOSE(phys_viscosity, converter.GetPhysViscosity(), g_zero_tol);
  CHECK_CLOSE(phys_density, converter.GetPhysDensity(), g_zero_tol);
  CHECK_CLOSE(char_phys_pressure, converter.GetCharPhysPressure(),
      g_zero_tol);
  CHECK_CLOSE(reynolds, converter.GetReynoldsNumber(), g_zero_tol);
  CHECK_CLOSE(phys_length, converter.GetPhysLength(lattice_length),
      g_zero_tol);
  CHECK_EQUAL(lattice_length, converter.GetLatticeLength(phys_length));
  CHECK_CLOSE(conv_length, converter.GetConversionFactorLength(),
      g_zero_tol);
  CHECK_CLOSE(conv_length, converter.GetPhysDeltaX(), g_zero_tol);
  CHECK_CLOSE(phys_time, converter.GetPhysTime(lattice_time), g_zero_tol);
  CHECK_EQUAL(lattice_time, converter.GetLatticeTime(phys_time));
  CHECK_CLOSE(conv_time, converter.GetConversionFactorTime(), g_zero_tol);
  CHECK_CLOSE(conv_time, converter.GetPhysDeltaT(), g_zero_tol);
  CHECK_CLOSE(phys_velocity, converter.GetPhysVelocity(lattice_velocity),
      g_zero_tol);
  CHECK_CLOSE(lattice_velocity, converter.GetLatticeVelocity(phys_velocity),
      g_zero_tol);
  CHECK_CLOSE(conv_velocity, converter.GetConversionFactorVelocity(),
      g_zero_tol);
  CHECK_CLOSE(phys_density, converter.GetPhysDensity(lattice_density),
      g_zero_tol);
  CHECK_CLOSE(lattice_density, converter.GetLatticeDensity(phys_density),
      g_zero_tol);
  CHECK_CLOSE(conv_density, converter.GetConversionFactorDensity(),
      g_zero_tol);
  CHECK_CLOSE(phys_mass, converter.GetPhysMass(lattice_mass), g_zero_tol);
  CHECK_CLOSE(lattice_mass, converter.GetLatticeMass(phys_mass), g_zero_tol);
  CHECK_CLOSE(conv_mass, converter.GetConversionFactorMass(), g_zero_tol);
  CHECK_CLOSE(phys_viscosity, converter.GetPhysViscosity(lattice_viscosity),
      g_zero_tol);
  CHECK_CLOSE(lattice_viscosity, converter.GetLatticeViscosity(),
      g_zero_tol);
  CHECK_CLOSE(conv_viscosity, converter.GetConversionFactorViscosity(),
      g_zero_tol);
  CHECK_CLOSE(phys_force, converter.GetPhysForce(lattice_force), g_zero_tol);
  CHECK_CLOSE(lattice_force, converter.GetLatticeForce(phys_force),
      g_zero_tol);
  CHECK_CLOSE(conv_force, converter.GetConversionFactorForce(), g_zero_tol);
  CHECK_CLOSE(phys_pressure, converter.GetPhysPressure(lattice_pressure),
      g_zero_tol);
  CHECK_CLOSE(lattice_pressure, converter.GetLatticePressure(phys_pressure),
      g_zero_tol);
  CHECK_CLOSE(conv_pressure, converter.GetConversionFactorPressure(),
      g_zero_tol);
}

TEST(TestUnitConverterFromResolutionAndRelaxationTime_Constructor)
{
  auto resolution = 12u;
  auto relaxation_time = 1.2;
  auto char_phys_length = 3.4;
  auto char_phys_velocity = 5.6;
  auto phys_viscosity = 7.8;
  auto phys_density = 9.0;
  auto char_phys_pressure = 11.12;
  // Variable for expected value calculation
  auto lattice_length = 14u;
  auto lattice_time = 15u;
  auto lattice_velocity = 16.17;
  auto lattice_mass = 18.19;
  auto lattice_force = 20.21;
  auto lattice_pressure = 22.23;

  const UnitConverterFromResolutionAndRelaxationTime<double,
      descriptor::D2Q9Descriptor> converter(resolution, relaxation_time,
          char_phys_length, char_phys_velocity, phys_viscosity, phys_density,
          char_phys_pressure);

  // Expected values
  auto delta_x = char_phys_length / static_cast<double>(resolution);
  auto delta_t = (relaxation_time - 0.5) /
      descriptor::D2Q9Descriptor<double>::sInvCsSqr * pow(char_phys_length /
      static_cast<double>(resolution), 2) / phys_viscosity;
  auto conv_length = delta_x;
  auto conv_time = delta_t;
  auto conv_velocity = conv_length / conv_time;
  auto conv_density = phys_density;
  auto conv_mass = conv_density * conv_length * conv_length * conv_length;
  auto conv_viscosity = conv_length * conv_length / conv_time;
  auto conv_force = conv_mass * conv_length / conv_time / conv_time;
  auto conv_pressure = conv_force / conv_length / conv_length;
  auto char_lattice_velocity = char_phys_velocity / conv_velocity;
  auto reynolds = char_phys_velocity * char_phys_length / phys_viscosity;
  auto phys_length = conv_length * static_cast<double>(lattice_length);
  auto phys_time = conv_time * static_cast<double>(lattice_time);
  auto phys_velocity = conv_velocity * lattice_velocity;
  auto lattice_density = phys_density / conv_density;
  auto phys_mass = conv_mass * lattice_mass;
  auto lattice_viscosity = phys_viscosity / conv_viscosity;
  auto phys_force = conv_force * lattice_force;
  auto phys_pressure = conv_pressure * lattice_pressure + char_phys_pressure;

  CHECK_EQUAL(resolution, converter.GetResolution());
  CHECK_CLOSE(relaxation_time, converter.GetRelaxationTime(), g_zero_tol);
  CHECK_CLOSE(1.0 / relaxation_time, converter.GetRelaxationFrequency(),
      g_zero_tol);
  CHECK_CLOSE(char_phys_length, converter.GetCharPhysLength(), g_zero_tol);
  CHECK_CLOSE(char_phys_velocity, converter.GetCharPhysVelocity(),
      g_zero_tol);
  CHECK_CLOSE(char_lattice_velocity, converter.GetCharLatticeVelocity(),
      g_zero_tol);
  CHECK_CLOSE(phys_viscosity, converter.GetPhysViscosity(), g_zero_tol);
  CHECK_CLOSE(phys_density, converter.GetPhysDensity(), g_zero_tol);
  CHECK_CLOSE(char_phys_pressure, converter.GetCharPhysPressure(),
      g_zero_tol);
  CHECK_CLOSE(reynolds, converter.GetReynoldsNumber(), g_zero_tol);
  CHECK_CLOSE(phys_length, converter.GetPhysLength(lattice_length),
      g_zero_tol);
  CHECK_EQUAL(lattice_length, converter.GetLatticeLength(phys_length));
  CHECK_CLOSE(conv_length, converter.GetConversionFactorLength(),
      g_zero_tol);
  CHECK_CLOSE(conv_length, converter.GetPhysDeltaX(), g_zero_tol);
  CHECK_CLOSE(phys_time, converter.GetPhysTime(lattice_time), g_zero_tol);
  CHECK_EQUAL(lattice_time, converter.GetLatticeTime(phys_time));
  CHECK_CLOSE(conv_time, converter.GetConversionFactorTime(), g_zero_tol);
  CHECK_CLOSE(conv_time, converter.GetPhysDeltaT(), g_zero_tol);
  CHECK_CLOSE(phys_velocity, converter.GetPhysVelocity(lattice_velocity),
      g_zero_tol);
  CHECK_CLOSE(lattice_velocity, converter.GetLatticeVelocity(phys_velocity),
      g_zero_tol);
  CHECK_CLOSE(conv_velocity, converter.GetConversionFactorVelocity(),
      g_zero_tol);
  CHECK_CLOSE(phys_density, converter.GetPhysDensity(lattice_density),
      g_zero_tol);
  CHECK_CLOSE(lattice_density, converter.GetLatticeDensity(phys_density),
      g_zero_tol);
  CHECK_CLOSE(conv_density, converter.GetConversionFactorDensity(),
      g_zero_tol);
  CHECK_CLOSE(phys_mass, converter.GetPhysMass(lattice_mass), g_zero_tol);
  CHECK_CLOSE(lattice_mass, converter.GetLatticeMass(phys_mass), g_zero_tol);
  CHECK_CLOSE(conv_mass, converter.GetConversionFactorMass(), g_zero_tol);
  CHECK_CLOSE(phys_viscosity, converter.GetPhysViscosity(lattice_viscosity),
      g_zero_tol);
  CHECK_CLOSE(lattice_viscosity, converter.GetLatticeViscosity(),
      g_zero_tol);
  CHECK_CLOSE(conv_viscosity, converter.GetConversionFactorViscosity(),
      g_zero_tol);
  CHECK_CLOSE(phys_force, converter.GetPhysForce(lattice_force), g_zero_tol);
  CHECK_CLOSE(lattice_force, converter.GetLatticeForce(phys_force),
      g_zero_tol);
  CHECK_CLOSE(conv_force, converter.GetConversionFactorForce(), g_zero_tol);
  CHECK_CLOSE(phys_pressure, converter.GetPhysPressure(lattice_pressure),
      g_zero_tol);
  CHECK_CLOSE(lattice_pressure, converter.GetLatticePressure(phys_pressure),
      g_zero_tol);
  CHECK_CLOSE(conv_pressure, converter.GetConversionFactorPressure(),
      g_zero_tol);
}

TEST(TestUnitConverterFromResolutionAndLatticeVelocity_Constructor)
{
  auto resolution = 12u;
  auto char_lattice_velocity = 1.2;
  auto char_phys_length = 3.4;
  auto char_phys_velocity = 5.6;
  auto phys_viscosity = 7.8;
  auto phys_density = 9.0;
  auto char_phys_pressure = 11.12;
  // Variable for expected value calculation
  auto lattice_length = 14u;
  auto lattice_time = 15u;
  auto lattice_velocity = 16.17;
  auto lattice_mass = 18.19;
  auto lattice_force = 20.21;
  auto lattice_pressure = 22.23;

  const UnitConverterFromResolutionAndLatticeVelocity<double,
      descriptor::D2Q9Descriptor> converter(resolution,
      char_lattice_velocity, char_phys_length, char_phys_velocity,
      phys_viscosity, phys_density, char_phys_pressure);

  // Expected values
  auto delta_x = char_phys_length / static_cast<double>(resolution);
  auto delta_t = char_lattice_velocity / char_phys_velocity *
      char_phys_length / static_cast<double>(resolution);
  auto conv_length = delta_x;
  auto conv_time = delta_t;
  auto conv_velocity = conv_length / conv_time;
  auto conv_density = phys_density;
  auto conv_mass = conv_density * conv_length * conv_length * conv_length;
  auto conv_viscosity = conv_length * conv_length / conv_time;
  auto conv_force = conv_mass * conv_length / conv_time / conv_time;
  auto conv_pressure = conv_force / conv_length / conv_length;
  auto relaxation_time = phys_viscosity / conv_viscosity *
      descriptor::D2Q9Descriptor<double>::sInvCsSqr + 0.5;
  auto reynolds = char_phys_velocity * char_phys_length / phys_viscosity;
  auto phys_length = conv_length * static_cast<double>(lattice_length);
  auto phys_time = conv_time * static_cast<double>(lattice_time);
  auto phys_velocity = conv_velocity * lattice_velocity;
  auto lattice_density = phys_density / conv_density;
  auto phys_mass = conv_mass * lattice_mass;
  auto lattice_viscosity = phys_viscosity / conv_viscosity;
  auto phys_force = conv_force * lattice_force;
  auto phys_pressure = conv_pressure * lattice_pressure + char_phys_pressure;

  CHECK_EQUAL(resolution, converter.GetResolution());
  CHECK_CLOSE(relaxation_time, converter.GetRelaxationTime(), g_zero_tol);
  CHECK_CLOSE(1.0 / relaxation_time, converter.GetRelaxationFrequency(),
      g_zero_tol);
  CHECK_CLOSE(char_phys_length, converter.GetCharPhysLength(), g_zero_tol);
  CHECK_CLOSE(char_phys_velocity, converter.GetCharPhysVelocity(),
      g_zero_tol);
  CHECK_CLOSE(char_lattice_velocity, converter.GetCharLatticeVelocity(),
      g_loose_tol);
  CHECK_CLOSE(phys_viscosity, converter.GetPhysViscosity(), g_zero_tol);
  CHECK_CLOSE(phys_density, converter.GetPhysDensity(), g_zero_tol);
  CHECK_CLOSE(char_phys_pressure, converter.GetCharPhysPressure(),
      g_zero_tol);
  CHECK_CLOSE(reynolds, converter.GetReynoldsNumber(), g_zero_tol);
  CHECK_CLOSE(phys_length, converter.GetPhysLength(lattice_length),
      g_zero_tol);
  CHECK_EQUAL(lattice_length, converter.GetLatticeLength(phys_length));
  CHECK_CLOSE(conv_length, converter.GetConversionFactorLength(),
      g_zero_tol);
  CHECK_CLOSE(conv_length, converter.GetPhysDeltaX(), g_zero_tol);
  CHECK_CLOSE(phys_time, converter.GetPhysTime(lattice_time), g_zero_tol);
  CHECK_EQUAL(lattice_time, converter.GetLatticeTime(phys_time));
  CHECK_CLOSE(conv_time, converter.GetConversionFactorTime(), g_zero_tol);
  CHECK_CLOSE(conv_time, converter.GetPhysDeltaT(), g_zero_tol);
  CHECK_CLOSE(phys_velocity, converter.GetPhysVelocity(lattice_velocity),
      g_zero_tol);
  CHECK_CLOSE(lattice_velocity, converter.GetLatticeVelocity(phys_velocity),
      g_zero_tol);
  CHECK_CLOSE(conv_velocity, converter.GetConversionFactorVelocity(),
      g_zero_tol);
  CHECK_CLOSE(phys_density, converter.GetPhysDensity(lattice_density),
      g_zero_tol);
  CHECK_CLOSE(lattice_density, converter.GetLatticeDensity(phys_density),
      g_zero_tol);
  CHECK_CLOSE(conv_density, converter.GetConversionFactorDensity(),
      g_zero_tol);
  CHECK_CLOSE(phys_mass, converter.GetPhysMass(lattice_mass), g_zero_tol);
  CHECK_CLOSE(lattice_mass, converter.GetLatticeMass(phys_mass), g_zero_tol);
  CHECK_CLOSE(conv_mass, converter.GetConversionFactorMass(), g_zero_tol);
  CHECK_CLOSE(phys_viscosity, converter.GetPhysViscosity(lattice_viscosity),
      g_zero_tol);
  CHECK_CLOSE(lattice_viscosity, converter.GetLatticeViscosity(),
      g_zero_tol);
  CHECK_CLOSE(conv_viscosity, converter.GetConversionFactorViscosity(),
      g_zero_tol);
  CHECK_CLOSE(phys_force, converter.GetPhysForce(lattice_force), g_zero_tol);
  CHECK_CLOSE(lattice_force, converter.GetLatticeForce(phys_force),
      g_zero_tol);
  CHECK_CLOSE(conv_force, converter.GetConversionFactorForce(), g_zero_tol);
  CHECK_CLOSE(phys_pressure, converter.GetPhysPressure(lattice_pressure),
      g_zero_tol);
  CHECK_CLOSE(lattice_pressure, converter.GetLatticePressure(phys_pressure),
      g_zero_tol);
  CHECK_CLOSE(conv_pressure, converter.GetConversionFactorPressure(),
      g_zero_tol);
}

TEST(TestUnitConverterFromRelaxationTimeAndLatticeVelocity_Constructor)
{
  auto relaxation_time = 1.2;
  auto char_lattice_velocity = 3.4;
  auto char_phys_length = 5.6;
  auto char_phys_velocity = 7.8;
  auto phys_viscosity = 9.0;
  auto phys_density = 11.12;
  auto char_phys_pressure = 13.14;
  // Variable for expected value calculation
  auto lattice_length = 14u;
  auto lattice_time = 15u;
  auto lattice_velocity = 16.17;
  auto lattice_mass = 18.19;
  auto lattice_force = 20.21;
  auto lattice_pressure = 22.23;

  const UnitConverterFromRelaxationTimeAndLatticeVelocity<double,
      descriptor::D2Q9Descriptor> converter(relaxation_time,
      char_lattice_velocity, char_phys_length, char_phys_velocity,
      phys_viscosity, phys_density, char_phys_pressure);

  // Expected values
  auto delta_x = phys_viscosity * char_lattice_velocity /
      char_phys_velocity * descriptor::D2Q9Descriptor<double>::sInvCsSqr /
      (relaxation_time - 0.5);
  auto delta_t = phys_viscosity * char_lattice_velocity *
      char_lattice_velocity / char_phys_velocity / char_phys_velocity *
      descriptor::D2Q9Descriptor<double>::sInvCsSqr / (relaxation_time -
          0.5);
  auto conv_length = delta_x;
  auto conv_time = delta_t;
  auto conv_velocity = conv_length / conv_time;
  auto conv_density = phys_density;
  auto conv_mass = conv_density * conv_length * conv_length * conv_length;
  auto conv_viscosity = conv_length * conv_length / conv_time;
  auto conv_force = conv_mass * conv_length / conv_time / conv_time;
  auto conv_pressure = conv_force / conv_length / conv_length;
  auto resolution = static_cast<unsigned>(char_phys_length / conv_length +
      0.5);
  auto reynolds = char_phys_velocity * char_phys_length / phys_viscosity;
  auto phys_length = conv_length * static_cast<double>(lattice_length);
  auto phys_time = conv_time * static_cast<double>(lattice_time);
  auto phys_velocity = conv_velocity * lattice_velocity;
  auto lattice_density = phys_density / conv_density;
  auto phys_mass = conv_mass * lattice_mass;
  auto lattice_viscosity = phys_viscosity / conv_viscosity;
  auto phys_force = conv_force * lattice_force;
  auto phys_pressure = conv_pressure * lattice_pressure + char_phys_pressure;

  CHECK_EQUAL(resolution, converter.GetResolution());
  CHECK_CLOSE(relaxation_time, converter.GetRelaxationTime(), g_loose_tol);
  CHECK_CLOSE(1.0 / relaxation_time, converter.GetRelaxationFrequency(),
      g_loose_tol);
  CHECK_CLOSE(char_phys_length, converter.GetCharPhysLength(), g_zero_tol);
  CHECK_CLOSE(char_phys_velocity, converter.GetCharPhysVelocity(),
      g_zero_tol);
  CHECK_CLOSE(char_lattice_velocity, converter.GetCharLatticeVelocity(),
      g_loose_tol);
  CHECK_CLOSE(phys_viscosity, converter.GetPhysViscosity(), g_zero_tol);
  CHECK_CLOSE(phys_density, converter.GetPhysDensity(), g_zero_tol);
  CHECK_CLOSE(char_phys_pressure, converter.GetCharPhysPressure(),
      g_zero_tol);
  CHECK_CLOSE(reynolds, converter.GetReynoldsNumber(), g_zero_tol);
  CHECK_CLOSE(phys_length, converter.GetPhysLength(lattice_length),
      g_zero_tol);
  CHECK_EQUAL(lattice_length, converter.GetLatticeLength(phys_length));
  CHECK_CLOSE(conv_length, converter.GetConversionFactorLength(),
      g_zero_tol);
  CHECK_CLOSE(conv_length, converter.GetPhysDeltaX(), g_zero_tol);
  CHECK_CLOSE(phys_time, converter.GetPhysTime(lattice_time), g_zero_tol);
  CHECK_EQUAL(lattice_time, converter.GetLatticeTime(phys_time));
  CHECK_CLOSE(conv_time, converter.GetConversionFactorTime(), g_zero_tol);
  CHECK_CLOSE(conv_time, converter.GetPhysDeltaT(), g_zero_tol);
  CHECK_CLOSE(phys_velocity, converter.GetPhysVelocity(lattice_velocity),
      g_zero_tol);
  CHECK_CLOSE(lattice_velocity, converter.GetLatticeVelocity(phys_velocity),
      g_zero_tol);
  CHECK_CLOSE(conv_velocity, converter.GetConversionFactorVelocity(),
      g_zero_tol);
  CHECK_CLOSE(phys_density, converter.GetPhysDensity(lattice_density),
      g_zero_tol);
  CHECK_CLOSE(lattice_density, converter.GetLatticeDensity(phys_density),
      g_zero_tol);
  CHECK_CLOSE(conv_density, converter.GetConversionFactorDensity(),
      g_zero_tol);
  CHECK_CLOSE(phys_mass, converter.GetPhysMass(lattice_mass), g_zero_tol);
  CHECK_CLOSE(lattice_mass, converter.GetLatticeMass(phys_mass), g_zero_tol);
  CHECK_CLOSE(conv_mass, converter.GetConversionFactorMass(), g_zero_tol);
  CHECK_CLOSE(phys_viscosity, converter.GetPhysViscosity(lattice_viscosity),
      g_zero_tol);
  CHECK_CLOSE(lattice_viscosity, converter.GetLatticeViscosity(),
      g_zero_tol);
  CHECK_CLOSE(conv_viscosity, converter.GetConversionFactorViscosity(),
      g_zero_tol);
  CHECK_CLOSE(phys_force, converter.GetPhysForce(lattice_force), g_zero_tol);
  CHECK_CLOSE(lattice_force, converter.GetLatticeForce(phys_force),
      g_zero_tol);
  CHECK_CLOSE(conv_force, converter.GetConversionFactorForce(), g_zero_tol);
  CHECK_CLOSE(phys_pressure, converter.GetPhysPressure(lattice_pressure),
      g_zero_tol);
  CHECK_CLOSE(lattice_pressure, converter.GetLatticePressure(phys_pressure),
      g_zero_tol);
  CHECK_CLOSE(conv_pressure, converter.GetConversionFactorPressure(),
      g_zero_tol);
}
}
}
