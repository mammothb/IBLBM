#ifndef SRC_IO_BASE64_HPP_
#define SRC_IO_BASE64_HPP_

#include <array>
#include <bitset>
#include <cstddef>
#include <iostream>

#include "gsl/gsl"

namespace iblbm
{
/**
 * Encoding algorithm inspired by
 * http://web.mit.edu/freebsd/head/contrib/wpa/src/utils/base64.c
 * Decoding map inspired by
 * https://stackoverflow.com/a/37109258/4115600
 */
template<typename T>
class Base64Encoder
{
  friend class TestBase64Encoder;
 public:
  /**
   * Constructor
   *
   * \param rOstream reference to output stream to store encoded data
   * \param fullLength full length of pre-encoded data
   */
  Base64Encoder(
      std::ostream& rOstream
    , std::size_t fullLength);

  /**
   * Encodes pData. Support continuing from previous left over bits, only
   * adds padding when we have reached mCharFullLength.
   *
   * \param pData pointer to data
   * \param length length (number of bytes) of data
   */
  void Encode(
      const T* pData
    , std::size_t length);

 private:
  /** Base64 alphabets for encoding */
  static const unsigned char msEnc64[65];

  /**
   * Read pCharData left to right and concatenate 3 8-bit input groups into
   * a 24-bit mOverflow. While we read, we will encode fully filled 24-bit
   * groups with EncodeGroup() or do nothing with partially filled 24-bit
   * mOverflow
   *
   * \param pCharData input char data
   * \param charLength length (number of bytes) of input data
   * \param rPosition current read position on the input char data
   */
  void FillOverflow(
      const unsigned char* pCharData
    , std::size_t charLength
    , gsl::index& rPosition);

  /**
   * Top up mOverflow with 0's to form a fully filled 24-bit input group and
   * encode with EncodePartialGroup().
   */
  void FlushOverflow();

  /**
   * Encode a fully filled 24-bit input group
   *
   * \param pData 24-bit group
   */
  void EncodeGroup(const unsigned char* pData);

  /**
   * Encode a partially filled 24-bit input group. Different treatment based
   * on the length of the partially filled group.
   * - length = 1 (8 bits): the final unit of encoded output will be two
   *   characters followed by two "=" padding characters
   * - length = 2 (16 bits): the final unit of encoded output will be three
   *   characters followed by one "=" padding character.
   *
   * \param pData pointer to 24-bit input group topped up with 0's
   * \param length length of actual data
   */
  void EncodePartialGroup(
      const unsigned char* pData
    , gsl::index length);

  /** Reference to output stream */
  std::ostream& mrOstream;
  /** Total length of output in bytes */
  std::size_t mCharFullLength;
  /** Total bytes written */
  std::size_t mNumWritten;
  /** Position in the overflow array */
  gsl::index mOverflowIndex;
  /** Overflow array for preparing 24-bit input groups */
  unsigned char mOverflow[3];
};

template<typename T>
class Base64Decoder
{
  friend class TestBase64Decoder;
 public:
  /**
   * Constructor
   *
   * \param rIstream reference to input stream for bas64 encoded data
   * \param fullLength full length of decoded data
   */
  Base64Decoder(
      std::istream& rIstream
    , std::size_t fullLength);

  /**
   * Repeatedly decode 24-bit groups to 3 8-bit output groups, if length is
   * not divisible by 3, left over groups are stored in mOverflow to be
   * decoded first in the next round
   *
   * \param pData pointer to where the decoded data should be written
   * \param length length (number of bytes) of data
   */
  void Decode(
      T* pData
    , std::size_t length);

 private:
  /** Look up table for Base64 decoding */
  static const unsigned char msDec64[256];

  /**
   * Read up to 24-bits from overflow array into pCharData to prepare for
   * decoding
   *
   * \param pCharData pointer to data for decoding
   * \param charLength length (number of bytes) of data to be decoded
   * \param rPosition current write position on the output char data
   */
  void FlushOverflow(
      unsigned char* pCharData
    , std::size_t charLength
    , gsl::index& rPosition);

  /**
   * Decode a 24-bit group into 3 8-bit output groups
   *
   * \param pData pointer to output group
   */
  void DecodeGroup(unsigned char* pData);

  /** Reference to input stream */
  std::istream& mrIstream;
  /** Total length of input (after decode) in bytes */
  std::size_t mCharFullLength;
  /** Total bytes read */
  std::size_t mNumRead;
  /** Position in the overflow array */
  gsl::index mOverflowIndex;
  /** Overflow array for preparing 24-bit input groups */
  unsigned char mOverflow[3];
};

/***************************************************************************
 * Helper functions for debugging
 ***************************************************************************/
/**
 * Print pData in 8 bit groups based on how the encoder/decoder sees it,
 * e.g., std::size_t data {12345} is read as
 * 00111001 00110000 00000000 00000000 00000000 00000000 00000000 00000000
 * for debugging/testing Base64Encoder against online encoders such as
 * https://cryptii.com/binary-to-base64
 *
 * \param pData pointer to data to be read
 * \param length length (number of bytes) of data
 */
template<typename T>
void FormatDataAsBinary(
    const T* pData
  , std::size_t length)
{
  auto p_data {reinterpret_cast<const unsigned char*>(pData)};
  auto char_length {length * sizeof(T)};
  for (gsl::index i = 0; i < char_length; ++i)
      std::cout << std::bitset<8>(p_data[i]) << " ";
  std::cout << std::endl;
}
}  // namespace iblbm

#endif  // SRC_IO_BASE64_HPP_
