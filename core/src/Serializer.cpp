#include "Serializer.hpp"

#include <fstream>

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

bool Serializer::Load(std::string filename/*="Serializable"*/)
{
  ValidateFilename(filename);

  // Determine binary size through `getSerializableSize()` method
  ComputeSize();

  std::ifstream in_fstream(GetFullFilename(filename).c_str());
  if (in_fstream) {
    ConvertIstreamToSerializer(*this, in_fstream);
    in_fstream.close();
    return true;
  }
  else {
    return false;
  }
}

bool Serializer::Save(std::string filename/*="Serializable"*/)
{
  ValidateFilename(filename);

  // Determine binary size through `getSerializableSize()` method
  ComputeSize();

  std::ofstream out_fstream(GetFullFilename(filename).c_str());
  if (out_fstream) {
    ConvertSerializerToOstream(*this, out_fstream);
    out_fstream.close();
    return true;
  }
  else {
    return false;
  }
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
