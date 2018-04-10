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
  else {
    Base64Encoder<std::size_t> size_encoder(rOstream, 1);
    size_encoder.Encode(&binary_size, 1);
  }

  Base64Encoder<bool> data_encoder(rOstream, binary_size);
  std::size_t block_size {};
  const bool* p_data_buffer {nullptr};
  while (p_data_buffer = rSerializer.pGetNextBlock(block_size,
      /*isLoad=*/false), p_data_buffer != nullptr) {
//    FormatDataAsBinary(p_data_buffer, block_size);
//    std::streambuf* backup {rOstream.rdbuf()};
//    rOstream.rdbuf(std::cout.rdbuf());
    data_encoder.Encode(p_data_buffer, block_size);
//    rOstream.rdbuf(backup);
//    std::cout << std::endl;

  }
  rSerializer.ResetCounter();
}

void ConvertIstreamToSerializer(
    Serializer& rSerializer
  , std::istream& rIstream
  , bool forceUnsigned/*=false*/)
{
  ;
}
}  // namespace iblbm
