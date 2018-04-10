#include "Base64.hpp"

#include "Exception.hpp"

namespace iblbm
{
/***************************************************************************
 * Base64Encoder
 ***************************************************************************/
template<typename T>
const unsigned char Base64Encoder<T>::msEnc64[65] =
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

  gsl::index position {};
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
  , gsl::index& rPosition)
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
  mrOstream << msEnc64[pData[0] >> 2];
  // do AND with 0x03 (00000011) to retain the last 2 bits of pData[0]
  // left shift pData[0] by 4 and right shift pData[1] by 4 and do OR to read
  // them as 6 bit group
  mrOstream << msEnc64[((pData[0] & 0x03) << 4) | (pData[1] >> 4)];
  // do AND with 0x0F (00001111) to retain last 4 bits of pData[1]
  // left shift pData[1] by 2 and right shift pData[2] by 6 and do OR to read
  // them as 6 bit group
  mrOstream << msEnc64[((pData[1] & 0x0F) << 2) | (pData[2] >> 6)];
  // do AND with 0x3F (00111111) to retain last 6 bits of pData[2]
  mrOstream << msEnc64[pData[2] & 0x3F];
}

template<typename T>
void Base64Encoder<T>::EncodePartialGroup(
    const unsigned char* pData
  , gsl::index length)
{
  mrOstream << msEnc64[pData[0] >> 2];
  if (length == 2) {
    mrOstream << msEnc64[((pData[0] & 0x03) << 4) | (pData[1] >> 4)];
    mrOstream << msEnc64[(pData[1] & 0x0F) << 2];
  }
  else {
    mrOstream << msEnc64[(pData[0] & 0x03) << 4];
    mrOstream << static_cast<unsigned char>('=');
  }
  mrOstream << static_cast<unsigned char>('=');
}

/***************************************************************************
 * Base64Decoder
 ***************************************************************************/
template<typename T>
const unsigned char Base64Decoder<T>::msDec64[256] = {0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 63, 62, 62, 63, 52, 53, 54, 55, 56,
    57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0,
    0, 63, 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
    42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

template<typename T>
Base64Decoder<T>::Base64Decoder(
    std::istream& rIstream
  , std::size_t fullLength)
  : mrIstream(rIstream),
    mCharFullLength(fullLength * sizeof(T)),
    mNumRead(0),
    mOverflowIndex(3)
{}

template<typename T>
void Base64Decoder<T>::Decode(
    T* pData
  , std::size_t length)
{
  auto p_char_data {reinterpret_cast<unsigned char*>(pData)};
  auto char_length {length * sizeof(T)};
  IBLBM_PRECONDITION(mNumRead + char_length <= mCharFullLength);

  gsl::index position {};
  FlushOverflow(p_char_data, char_length, position);
  while (position + 3 <= char_length) {
    DecodeGroup(p_char_data + position);
    position += 3;
  };
  if (position < char_length) {
    DecodeGroup(mOverflow);
    mOverflowIndex = 0;
    FlushOverflow(p_char_data, char_length, position);
  }
  mNumRead += char_length;
}

template<typename T>
void Base64Decoder<T>::FlushOverflow(
    unsigned char* pCharData
  , std::size_t charLength
  , gsl::index& rPosition)
{
  while (mOverflowIndex < 3 && rPosition < charLength) {
    pCharData[rPosition] = mOverflow[mOverflowIndex];
    ++mOverflowIndex;
    ++rPosition;
  }
}

template<typename T>
void Base64Decoder<T>::DecodeGroup(unsigned char* pData)
{
  unsigned char input[4];
  for (gsl::index i = 0; i < 4; ++i) {
    unsigned char next_char;
    mrIstream >> next_char;
    input[i] = msDec64[next_char];
  }
  pData[0] = input[0] << 2 | input[1] >> 4;
  pData[1] = input[1] << 4 | input[2] >> 2;
  pData[2] = input[2] << 6 | input[3];
}

// explicit instantiation
template class Base64Encoder<bool>;
template class Base64Encoder<std::size_t>;
template class Base64Encoder<unsigned>;
template class Base64Decoder<bool>;
template class Base64Decoder<std::size_t>;
template class Base64Decoder<unsigned>;
}  // namespace iblbm
