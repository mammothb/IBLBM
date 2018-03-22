#include "DataWriter.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>

namespace iblbm
{
DataWriter::DataWriter(AbstractLatticeModel& rLatticeModel)
  : mRoot(fs::current_path()),
    mResultDir(mRoot / "result"),
    mVtkFluidDir(mResultDir / "vtk_fluid"),
    mrLatticeModel(rLatticeModel),
    mpLbmNs(nullptr),
    mpLbmCd(nullptr),
    mpNs(nullptr),
    mpCd(nullptr),
    mFieldNum(3)
{
  DataWriter::InitialzeCleanFolder();
}

void DataWriter::InitialzeCleanFolder()
{
  if (fs::exists(mResultDir)) {
    for (fs::directory_iterator end_it, it(mResultDir); it != end_it; ++it)
        fs::remove_all(it->path());
  }
  else {
    fs::create_directories(mResultDir);
  }
  fs::create_directories(mVtkFluidDir);
}

void DataWriter::RegisterNsEquation(
    LatticeBoltzmannMethod* pLatticeBoltzmannMethod
  , AbstractCollisionModel* pCollisionModel)
{
  mpLbmNs = pLatticeBoltzmannMethod;
  mpNs = pCollisionModel;
  mFieldNums.push_back(++mFieldNum);
  mFieldNames.push_back("pressure");
  mFieldNums.push_back(++mFieldNum);
  mFieldNames.push_back("rho_ns");
}

void DataWriter::WriteResult(unsigned time)
{
  if (!mpNs) throw std::runtime_error("Not NS");
  const auto nx = mrLatticeModel.GetNx();
  const auto ny = mrLatticeModel.GetNy();

  // Writing fluid file
  auto vtk_fluid_filename = mVtkFluidDir / ("fluid_t" +
      std::to_string(time) + ".vtk");
  fs::ofstream vtk_fluid_file(vtk_fluid_filename);

  // Write VTK header
  vtk_fluid_file << "# vtk DataFile Version 3.0" << std::endl;
  vtk_fluid_file << "fluid_state" << std::endl;
  vtk_fluid_file << "ASCII" << std::endl;
  vtk_fluid_file << "DATASET RECTILINEAR_GRID" << std::endl;
  vtk_fluid_file << "DIMENSIONS " << nx << " " << ny << " 1" << std::endl;

  // Write x, y, z coordinates. z set to be 1 since it's 2D
  vtk_fluid_file << "X_COORDINATES " << nx << " float" << std::endl;
  for (auto x = 0u; x < nx; ++x) vtk_fluid_file << x << " ";
  vtk_fluid_file << std::endl;
  vtk_fluid_file << "Y_COORDINATES " << ny << " float" << std::endl;
  for (auto y = 0u; y < ny; ++y) vtk_fluid_file << y << " ";
  vtk_fluid_file << std::endl;
  vtk_fluid_file << "Z_COORDINATES " << 1 << " float" << std::endl;
  vtk_fluid_file << 0 << std::endl;
  vtk_fluid_file << "POINT_DATA " << nx * ny << std::endl;

  // Write density difference
  vtk_fluid_file << "SCALARS density_difference float 1" << std::endl;
  vtk_fluid_file << "LOOKUP_TABLE default" << std::endl;
  for (auto density : mpNs->rGetDensity())
      vtk_fluid_file << density << std::endl;

  // Write velocity as vectors
  vtk_fluid_file << "VECTORS velocity_vector float" << std::endl;
  for (auto v : mrLatticeModel.rGetVelocity())
      vtk_fluid_file << v[0] << " " << v[1] << " 0" << std::endl;

  vtk_fluid_file.close();
}
}  // namespace iblbm
