#ifndef SRC_CORE_UNITCONVERTER_HPP_
#define SRC_CORE_UNITCONVERTER_HPP_

#include <cmath>

namespace iblbm
{
/**
 * Conversion between physical and lattice units, as well as discretization.
 * Be aware of the nomenclature:
 * We distinguish between physical (dimensioned) and lattice (dimensionless)
 * values.
 * A specific conversion factor maps the two different scopes,
 * e.g. physLength = conversionLength * latticeLength
 *
 * For pressure and temperature we first shift the physical values by a
 * characteristic value to assure a lattice pressure and lattice temperature
 * between 0 and 1,
 * e.g. physPressure - charPhysPressure = conversionPressure * latticePressure
 *
 *  \param mRelaxationTime relaxation time, have to be greater than
 *         0.5!
 *  \param mPhysViscosity physical kinematic viscosity, m^2/s
 *  \param mPhysDensity physical density, kg/m^3
 *  \param mConvLength conversion factor for length, m
 *  \param mConvTime conversion factor for time, s
 *  \param mConvMass conversion factor for mass, kg
 *  \param mConvVelocity conversion velocity, m/s
 *  \param mConvViscosity conversion kinematic viscosity, m^2/s
 *  \param mConvDensity conversion density, kg/m^3
 *  \param mConvForce conversion force, kg*m/s^2
 *  \param mConvPressure conversion pressure, kg/m*s^2
 *  \param mResolution number of grid points per mCharPhysLength
 *  \param mCharLatticeVelocity
 */
template <typename T, template <typename U> class Lattice>
class UnitConverter
{
 public:
  /** Constructor:
   *  \param physDeltaX spacing between two lattice cells, m
   *  \param physDeltaT time step, s
   *  \param charPhysLength reference/characteristic length of simulation
   *         geometry, m
   *  \param charPhysVelocity maximal or highest expected velocity during
   *         simulation, m/s
   *  \param physViscosity physical kinematic viscosity, m^2/s
   *  \param physDensity physical density, kg/m^3
   *  \param charPhysPressure reference/characteristic physical pressure,
   *         Pa = kg/m*s^2
   */
  constexpr UnitConverter(
      T physDeltaX
    , T physDeltaT
    , T charPhysLength
    , T charPhysVelocity
    , T physViscosity
    , T physDensity
    , T charPhysPressure = 0)
    : mConvLength{physDeltaX},
      mConvTime{physDeltaT},
      mConvVelocity{mConvLength / mConvTime},
      mConvDensity{physDensity},
      mConvMass{mConvDensity * mConvLength * mConvLength * mConvLength},
      mConvViscosity{mConvLength * mConvLength / mConvTime},
      mConvForce{mConvMass * mConvLength / mConvTime / mConvTime},
      mConvPressure{mConvForce / mConvLength / mConvLength},
      mCharPhysLength{charPhysLength},
      mCharPhysVelocity{charPhysVelocity},
      mPhysViscosity{physViscosity},
      mPhysDensity{physDensity},
      mCharPhysPressure{charPhysPressure},
      mResolution{static_cast<unsigned>(mCharPhysLength / mConvLength +
          0.5)},
      mRelaxationTime{mPhysViscosity / mConvViscosity *
          Lattice<T>::sInvCsSqr + 0.5},
      mCharLatticeVelocity{mCharPhysVelocity / mConvVelocity}
  {}

  virtual ~UnitConverter() = default;

  /** \return resolution */
  constexpr unsigned GetResolution() const
  {
    return mResolution;
  }

  /** \return relaxation time in lattice units */
  constexpr T GetRelaxationTime() const
  {
    return mRelaxationTime;
  }

  /** \return relaxation frequency in lattice units */
  constexpr T GetRelaxationFrequency() const
  {
    return 1.0 / mRelaxationTime;
  }

  /** \return characteristic length in physical units */
  constexpr T GetCharPhysLength() const
  {
    return mCharPhysLength;
  }

  /** \return characteristic velocity in physical units */
  constexpr T GetCharPhysVelocity() const
  {
    return mCharPhysVelocity;
  }

  /** \return characteristic velocity in lattice units */
  constexpr T GetCharLatticeVelocity() const
  {
    return mCharLatticeVelocity;
  }

  /** \return viscosity in physical units */
  constexpr T GetPhysViscosity() const
  {
    return mPhysViscosity;
  }

  /** \return density in physical units */
  constexpr T GetPhysDensity() const
  {
    return mPhysDensity;
  }

  /** \return characteristic pressure in physical units */
  constexpr T GetCharPhysPressure() const
  {
    return mCharPhysPressure;
  }

  /** \return Reynolds number */
  constexpr T GetReynoldsNumber() const
  {
    return mCharPhysVelocity * mCharPhysLength / mPhysViscosity;
  }

  /** \return conversion from lattice to physical length */
  constexpr T GetPhysLength(unsigned latticeLength) const
  {
    return mConvLength * static_cast<T>(latticeLength);
  }

  /** \return conversion from physical to lattice length, returns number of
   * voxels for given physical length
   */
  constexpr unsigned GetLatticeLength(T physLength) const
  {
    return static_cast<unsigned>(physLength / mConvLength + 0.5);
  }

  /** \return access (read-only) to private member variable */
  constexpr T GetConversionFactorLength() const
  {
    return mConvLength;
  }

  /** \return grid spacing (voxel length), m */
  constexpr T GetPhysDeltaX() const
  {
    return mConvLength;
  }

  /** \return conversion from lattice to  physical time */
  constexpr T GetPhysTime(unsigned latticeTime) const
  {
    return mConvTime * static_cast<T>(latticeTime);
  }

  /** \return conversion from physical to lattice time */
  constexpr unsigned GetLatticeTime(T physTime) const
  {
    return static_cast<unsigned>(physTime / mConvTime + 0.5);
  }

  /** \return  access (read-only) to private member variable */
  constexpr T GetConversionFactorTime() const
  {
    return mConvTime;
  }

  /** \return time spacing (timestep length), s */
  constexpr T GetPhysDeltaT() const
  {
    return mConvTime;
  }

  /** \return conversion from lattice to  physical velocity */
  constexpr T GetPhysVelocity(T latticeVelocity) const
  {
    return mConvVelocity * latticeVelocity;
  }

  /** \return conversion from physical to lattice velocity */
  constexpr T GetLatticeVelocity(T physVelocity) const
  {
    return physVelocity / mConvVelocity;
  }

  /** \return access (read-only) to private member variable */
  constexpr T GetConversionFactorVelocity() const
  {
    return mConvVelocity;
  }

  /** \return conversion from lattice to physical density */
  constexpr T GetPhysDensity(T latticeDensity) const
  {
    return mConvDensity * latticeDensity;
  }

  /** \return conversion from physical to lattice density */
  constexpr T GetLatticeDensity(T physDensity) const
  {
    return physDensity / mConvDensity;
  }

  /** \return access (read-only) to private member variable */
  constexpr T GetConversionFactorDensity() const
  {
    return mConvDensity;
  }

  /** \return conversion from lattice to physical mass */
  constexpr T GetPhysMass(T latticeMass) const
  {
    return mConvMass * latticeMass;
  }

  /** \return conversion from physical to lattice mass */
  constexpr T GetLatticeMass(T physMass) const
  {
    return physMass / mConvMass;
  }

  /** \return access (read-only) to private member variable */
  constexpr T GetConversionFactorMass() const
  {
    return mConvMass;
  }

  /** \return conversion from lattice to  physical viscosity */
  constexpr T GetPhysViscosity(T latticeViscosity) const
  {
    return mConvViscosity * latticeViscosity;
  }

  /** \return conversion from physical to lattice viscosity */
  constexpr T GetLatticeViscosity() const
  {
    return mPhysViscosity / mConvViscosity;
  }

  /** \return access (read-only) to private member variable */
  constexpr T GetConversionFactorViscosity() const
  {
    return mConvViscosity;
  }

  /** \return conversion from lattice to  physical force */
  constexpr T GetPhysForce(T latticeForce) const
  {
    return mConvForce * latticeForce;
  }

  /** \return conversion from physical to lattice force */
  constexpr T GetLatticeForce(T physForce) const
  {
    return physForce / mConvForce;
  }

  /** \return access (read-only) to private member variable */
  constexpr T GetConversionFactorForce() const
  {
    return mConvForce;
  }

  /** \return conversion from lattice to physical pressure */
  constexpr T GetPhysPressure(T latticePressure) const
  {
    return mConvPressure * latticePressure + mCharPhysPressure;
  }

  /** \return conversion from physical to lattice pressure */
  constexpr T GetLatticePressure(T physPressure) const
  {
    return (physPressure - mCharPhysPressure) / mConvPressure;
  }

  /** \return access (read-only) to private member variable */
  constexpr T GetConversionFactorPressure() const
  {
    return mConvPressure;
  }

 protected:
  /** Conversion factors */
  const T mConvLength;  // m
  const T mConvTime;  // s
  const T mConvVelocity;  // m/s
  const T mConvDensity;  // kg/m^3
  const T mConvMass;  // kg
  const T mConvViscosity;  // m^2/s
  const T mConvForce;  // kg*m/s^2
  const T mConvPressure;  // kg/m*s^2

  /** Physical units, characteristic or reference values */
  const T mCharPhysLength;  // m
  const T mCharPhysVelocity;  // m/s
  const T mPhysViscosity;  // m^2/s
  const T mPhysDensity;  // kg/m^3
  const T mCharPhysPressure;  // kg/m*s^2

  /** Lattice units, discretization parameters */
  const unsigned mResolution;
  const T mRelaxationTime;
  const T mCharLatticeVelocity;
};

template <typename T, template <typename U> class Lattice>
class UnitConverterFromResolutionAndRelaxationTime
  : public UnitConverter<T, Lattice>
{
 public:
  constexpr UnitConverterFromResolutionAndRelaxationTime(
      unsigned resolution
    , T relaxationTime
    , T charPhysLength
    , T charPhysVelocity
    , T physViscosity
    , T physDensity
    , T charPhysPressure = 0)
    : UnitConverter<T, Lattice>(
          /*physDeltaX=*/charPhysLength / static_cast<T>(resolution),
          /*physDeltaT=*/(relaxationTime - 0.5) / Lattice<T>::sInvCsSqr *
              pow(charPhysLength / static_cast<T>(resolution), 2) /
              physViscosity,
          charPhysLength,
          charPhysVelocity,
          physViscosity,
          physDensity,
          charPhysPressure
      )
  {}
};

template <typename T, template <typename U> class Lattice>
class UnitConverterFromResolutionAndLatticeVelocity
 : public UnitConverter<T, Lattice>
{
 public:
  constexpr UnitConverterFromResolutionAndLatticeVelocity(
      unsigned resolution
    , T charLatticeVelocity
    , T charPhysLength
    , T charPhysVelocity
    , T physViscosity
    , T physDensity
    , T charPhysPressure = 0)
    : UnitConverter<T, Lattice>(
          /*physDeltaX=*/charPhysLength / static_cast<double>(resolution),
          /*physDeltaT=*/charLatticeVelocity / charPhysVelocity *
              charPhysLength / static_cast<double>(resolution),
          charPhysLength,
          charPhysVelocity,
          physViscosity,
          physDensity,
          charPhysPressure
      )
  {}
};

template <typename T, template <typename U> class Lattice>
class UnitConverterFromRelaxationTimeAndLatticeVelocity
 : public UnitConverter<T, Lattice>
{
 public:
  constexpr UnitConverterFromRelaxationTimeAndLatticeVelocity(
      T relaxationTime
    , T charLatticeVelocity
    , T charPhysLength
    , T charPhysVelocity
    , T physViscosity
    , T physDensity
    , T charPhysPressure = 0)
    : UnitConverter<T, Lattice>(
          /*physDeltaX=*/physViscosity * charLatticeVelocity /
              charPhysVelocity * Lattice<T>::sInvCsSqr /
              (relaxationTime - 0.5),
          /*physDeltaT=*/physViscosity * charLatticeVelocity *
              charLatticeVelocity / charPhysVelocity / charPhysVelocity *
              Lattice<T>::sInvCsSqr / (relaxationTime - 0.5),
          charPhysLength,
          charPhysVelocity,
          physViscosity,
          physDensity,
          charPhysPressure
      )
  {}
};
}  // namespace iblbm

#endif  // SRC_CORE_UNITCONVERTER_HPP_
