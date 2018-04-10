#include "Base64.hpp"

#include "Exception.hpp"

namespace iblbm
{
template<typename T>
const char Base64Encoder<T>::msEnc64[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

template<typename T>
Base64Encoder<T>::Base64Encoder(
    std::ostream& rOstream
  , std::size_t fullLength)
  : mrOstream(rOstream),
    mCharFullLength(fullLength * sizeof(T)),
    mNumWritten(0),
    mOverflowIndex(0)
{}

template<typename T>
void Base64Encoder<T>::Encode(
    const T* pData
  , std::size_t length)
{
  auto p_char_data {reinterpret_cast<const unsigned char*>(pData)};
  auto char_length {length * sizeof(T)};
  IBLBM_PRECONDITION(mNumWritten + char_length <= mCharFullLength);

  std::size_t position {};
  FillOverflow(p_char_data, char_length, position);
  // Keep encoding fully filled 24-bit groups while we can
  while (position + 3 <= char_length) {
    EncodeGroup(p_char_data + position);
    position += 3;
  }
  // Partially fill the remaining 24-bit group
  FillOverflow(p_char_data, char_length, position);
  mNumWritten += char_length;
  // Only pad if we have reached the end of the data
  if (mNumWritten == mCharFullLength) FlushOverflow();
}

template<typename T>
void Base64Encoder<T>::FillOverflow(
    const unsigned char* pCharData
  , std::size_t charLength
  , std::size_t& rPosition)
{
  while (mOverflowIndex < 3 && rPosition < charLength) {
    mOverflow[mOverflowIndex] = pCharData[rPosition];
    ++mOverflowIndex;
    ++rPosition;
  }
  if (mOverflowIndex == 3) {
    EncodeGroup(mOverflow);
    mOverflowIndex = 0;
  }
}

template<typename T>
void Base64Encoder<T>::FlushOverflow()
{
  if (mOverflowIndex > 0) {
    for (gsl::index i = mOverflowIndex; i < 3; ++i) mOverflow[i] = 0;
    EncodePartialGroup(mOverflow, mOverflowIndex);
    mOverflowIndex = 0;
  }
}

template<typename T>
void Base64Encoder<T>::EncodeGroup(const unsigned char* pData)
{
  // Right shift by 2 to read it as its first 6 bits
  mrOstream << static_cast<unsigned char>(msEnc64[pData[0] >> 2]);
  // do AND with 0x03 (00000011) to retain the last 2 bits of pData[0]
  // do AND with 0xF0 (11110000) to retain the first 4 bits of pData[1]
  // left shift pData[0] by 4 and right shift pData[1] by 4 and do OR to read
  // them as 6 bit group
  mrOstream << static_cast<unsigned char>(msEnc64[((pData[0] & 0x03) << 4) |
      ((pData[1] & 0xF0) >> 4)]);
  // do AND with 0x0F (00001111) to retain last 4 bits of pData[1]
  // do AND with 0xC0 (11000000) to retain first 2 bits of pData[2]
  // left shift pData[1] by 2 and right shift pData[2] by 6 and do OR to read
  // them as 6 bit group
  mrOstream << static_cast<unsigned char>(msEnc64[((pData[1] & 0x0F) << 2) |
      ((pData[2] & 0xC0) >> 6)]);
  // do AND with 0x3F (00111111) to retain last 6 bits of pData[2]
  mrOstream << static_cast<unsigned char>(msEnc64[pData[2] & 0x3F]);
}

template<typename T>
void Base64Encoder<T>::EncodePartialGroup(
    const unsigned char* pData
  , gsl::index length)
{
  mrOstream << static_cast<unsigned char>(msEnc64[pData[0] >> 2]);
  mrOstream << static_cast<unsigned char>(msEnc64[((pData[0] & 0x03) << 4) |
      ((pData[1] & 0xF0) >> 4)]);
  mrOstream << static_cast<unsigned char>(length == 2 ?
      msEnc64[((pData[1] & 0x0F) << 2) | ((pData[2] & 0xC0) >> 6)] : '=');
  mrOstream << static_cast<unsigned char>('=');
}

template class Base64Encoder<bool>;
template class Base64Encoder<std::size_t>;
}  // namespace iblbm
