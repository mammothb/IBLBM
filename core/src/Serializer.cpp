#include "Serializer.hpp"

#include <fstream>

#include "FileFinder.hpp"
#include "MpiManager.hpp"
#include "OutputFileHandler.hpp"
#include "Serializable.hpp"
#include "SerializerIO.hpp"

namespace iblbm
{
Serializer::Serializer(
    Serializable& rSerializable
  , std::string filename/*="Serializable"*/)
  : mrSerializable{rSerializable},
    mBlockIndex{0},
    mSize{0},
    mFilename{filename == "" ? "Serializable" : filename}
{}

void Serializer::ResetCounter()
{
  mBlockIndex = 0;
}

std::size_t Serializer::GetSize() const
{
  return mSize;
}

bool* Serializer::pGetNextBlock(
    std::size_t& rBlockSize
  , const bool isLoad)
{
  return mrSerializable.pGetBlock(mBlockIndex++, rBlockSize, isLoad);
}

void Serializer::Load(
    const std::string& rDirectory
  , std::string filename/*=""*/)
{
  ValidateFilename(filename);
  FileFinder file {rDirectory + "/" + GetFullFilename(filename),
      RelativeTo::IBLBM_TEST_OUTPUT};

  // Determine binary size through `getSerializableSize()` method
  ComputeSize();

  std::ifstream in_fstream(file.GetAbsolutePath().c_str());
  if (in_fstream) {
    ConvertIstreamToSerializer(*this, in_fstream);
    in_fstream.close();
  }
}

void Serializer::Save(
    const std::string& rDirectory
  , std::string filename/*=""*/
  , bool cleanOutputDirectory/*=true*/)
{
  ValidateFilename(filename);
  // Clear directory if requested (and make sure it exists)
  OutputFileHandler handler {rDirectory, cleanOutputDirectory};
  auto file_path {handler.GetOutputDirectoryFullPath() +
      GetFullFilename(filename)};

  // Determine binary size through 'GetSerializableSize()' method
  ComputeSize();

  std::ofstream out_fstream(file_path.c_str());
  if (out_fstream) {
    ConvertSerializerToOstream(*this, out_fstream);
    out_fstream.close();
  }
  // Make sure everything is written before any process continues.
  MpiManager::Instance().Barrier("Serializer::Save");
}

void Serializer::ComputeSize(const bool forceRecompute/*=false*/)
{
  if (forceRecompute || mSize == 0)
      mSize = mrSerializable.GetSerializableSize();
}

void Serializer::ValidateFilename(std::string& rFilename)
{
  if (rFilename == "") rFilename = mFilename;
}

const std::string Serializer::GetFullFilename(std::string filename)
{
  return OutputFileHandler::GetParallelOutputFilename(filename, ".dat");
}
}  // namespace iblbm
