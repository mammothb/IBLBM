#include "Serializer.hpp"

#include <fstream>

#include "Serializable.hpp"

namespace iblbm
{
Serializer::Serializer(
    Serializable& rSerializable
  , const std::string& rFilename/*="Serializable"*/)
  : mrSerializable(rSerializable),
    mBlockNo(0),
    mSize(0),
    mFilename(rFilename == "" ? "Serializable" : mFilename)
{}

bool Serializer::Save(
    std::string filename/*="Serializable"*/
  , bool forceUnsigned/*=false*/)
{
  ValidateFilename(filename);

  // Determine binary size through `getSerializableSize()` method
  ComputeSize();

  std::ofstream ostr(GetFullFilename(filename).c_str());
  if (ostr) {
//    serializer2ostr(*this, ostr, forceUnsigned);
    ostr.close();
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

const std::string Serializer::GetFullFilename(const std::string& rFilename)
{
  return "";
//  return singleton::directories().getLogOutDir() +
//      createParallelFileName(fileName) + ".dat";
}

/***************************************************************************
 * Serializable
 ***************************************************************************/
Serializable::~Serializable()
{}
}  // namespace iblbm
