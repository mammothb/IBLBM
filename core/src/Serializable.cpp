#include "Serializable.hpp"

#include "Serializer.hpp"

namespace iblbm
{
Serializable::~Serializable()
{}

void Serializable::Save(
    const std::string& rDirectory
  , std::string filename/*=""*/
  , bool cleanOutputDirectory/*=true*/)
{
  Serializer tmp {*this};
  tmp.Save(rDirectory, filename, cleanOutputDirectory);
}

void Serializable::Load(
    const std::string& rDirectory
  , std::string filename/*=""*/)
{
  Serializer tmp {*this};
  tmp.Load(rDirectory, filename);
}
}  // namespace iblbm
