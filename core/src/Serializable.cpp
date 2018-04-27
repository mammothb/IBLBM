#include "Serializable.hpp"

#include "Serializer.hpp"

namespace iblbm
{
Serializable::~Serializable()
{}

bool Serializable::Save(std::string filename)
{
  Serializer tmp {*this, filename};
  return tmp.Save();
}

bool Serializable::Load(std::string filename)
{
  Serializer tmp {*this, filename};
  return tmp.Load();
}
}  // namespace iblbm
