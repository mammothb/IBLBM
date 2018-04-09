#include "SerializerIO.hpp"

#include <limits>

#include "Base64.hpp"
#include "Exception.hpp"
#include "Serializer.hpp"

namespace iblbm
{
void ConvertSerializerToOstream(
    Serializer& rSerializer
  , std::ostream& rOstream
  , bool forceUnsigned/*=false*/)
{
  rSerializer.ResetCounter();
  // Write binary size into first integer of stream
  auto binary_size {rSerializer.GetSize()};
  if (forceUnsigned) {
    IBLBM_PRECONDITION(binary_size <= std::numeric_limits<unsigned>::max());
    auto unsigned_binary_size {static_cast<unsigned>(binary_size)};
  }
}

void ConvertIstreamToSerializer(
    Serializer& rSerializer
  , std::istream& rIstream
  , bool forceUnsigned/*=false*/)
{
  ;
}
}  // namespace iblbm
