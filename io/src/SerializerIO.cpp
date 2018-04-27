#include "SerializerIO.hpp"

#include <limits>

#include "Base64.hpp"
#include "Exception.hpp"
#include "Serializer.hpp"

namespace iblbm
{
void ConvertSerializerToOstream(
    Serializer& rSerializer
  , std::ostream& rOstream)
{
  rSerializer.ResetCounter();
  // Write binary size into first integer of stream
  auto binary_size {rSerializer.GetSize()};
  Base64Encoder<std::size_t> size_encoder(rOstream, 1);
  size_encoder.Encode(&binary_size, 1);

  Base64Encoder<bool> data_encoder(rOstream, binary_size);
  std::size_t block_size {0};
  const bool* p_data_buffer {nullptr};
  while (p_data_buffer = rSerializer.pGetNextBlock(block_size,
      /*isLoad=*/false), p_data_buffer != nullptr) {
    data_encoder.Encode(p_data_buffer, block_size);
  }
  rSerializer.ResetCounter();
}

void ConvertIstreamToSerializer(
    Serializer& rSerializer
  , std::istream& rIstream)
{
  rSerializer.ResetCounter();
  // Read binary size
  std::size_t binary_size {0};
  Base64Decoder<std::size_t> size_decoder(rIstream, 1);
  size_decoder.Decode(&binary_size, 1);

  Base64Decoder<bool> data_decoder(rIstream, binary_size);
  std::size_t block_size{0};
  bool* p_data_buffer {nullptr};
  while (p_data_buffer = rSerializer.pGetNextBlock(block_size,
      /*isLoad=*/true), p_data_buffer != nullptr) {
    data_decoder.Decode(p_data_buffer, block_size);
  }
  rSerializer.ResetCounter();
}
}  // namespace iblbm
