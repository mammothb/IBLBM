#include <iostream>
#include <sstream>
#include <vector>

#include "UnitTest++/UnitTest++.h"
#include "Base64.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
class TestBase64Encoder
{
 public:
  template<typename T>
  std::size_t GetCharFullLength(Base64Encoder<T> rEncoder)
  {
    return rEncoder.mCharFullLength;
  }

  template<typename T>
  std::size_t GetNumWritten(Base64Encoder<T> rEncoder)
  {
    return rEncoder.mNumWritten;
  }

  template<typename T>
  gsl::index GetOverflowIndex(Base64Encoder<T> rEncoder)
  {
    return rEncoder.mOverflowIndex;
  }
};

class TestBase64Decoder
{
 public:
  template<typename T>
  std::size_t GetCharFullLength(Base64Decoder<T> rDecoder)
  {
    return rDecoder.mCharFullLength;
  }

  template<typename T>
  std::size_t GetNumRead(Base64Decoder<T> rDecoder)
  {
    return rDecoder.mNumRead;
  }

  template<typename T>
  gsl::index GetOverflowIndex(Base64Decoder<T> rDecoder)
  {
    return rDecoder.mOverflowIndex;
  }
};

SUITE(TestIo)
{
TEST(TestBase64Encoder_Constructor)
{
  TestBase64Encoder tester;
  std::ostringstream ostr;
  std::size_t length_1 {1};
  Base64Encoder<std::size_t> encoder_1(ostr, length_1);

  CHECK_EQUAL(length_1 * sizeof(std::size_t),
      tester.GetCharFullLength<std::size_t>(encoder_1));
  CHECK_EQUAL(0, tester.GetNumWritten(encoder_1));
  CHECK_EQUAL(0, tester.GetOverflowIndex(encoder_1));

  std::size_t length_2 {2};
  Base64Encoder<bool> encoder_2(ostr, length_2);

  CHECK_EQUAL(length_2 * sizeof(bool),
      tester.GetCharFullLength<bool>(encoder_2));
  CHECK_EQUAL(0, tester.GetNumWritten<bool>(encoder_2));
  CHECK_EQUAL(0, tester.GetOverflowIndex<bool>(encoder_2));
}

TEST(TestBase64Encoder_Encode_Size)
{
  TestBase64Encoder tester;
  // std::size_t occupies 8 bytes, we are testing 8 different number which
  // will occupy different number of bytes of std::size_t

  // byte 0
  {
    std::ostringstream ostr;
    std::size_t length {1};
    Base64Encoder<std::size_t> encoder(ostr, length);

    std::size_t size_data {123};
    encoder.Encode(&size_data, length);

    std::string exp_str {"ewAAAAAAAAA="};
    CHECK_EQUAL(exp_str, ostr.str());
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumWritten<std::size_t>(encoder));
  }

  // byte 0, 1
  {
    std::ostringstream ostr;
    std::size_t length {1};
    Base64Encoder<std::size_t> encoder(ostr, length);

    std::size_t size_data {12345};
    encoder.Encode(&size_data, length);

    std::string exp_str {"OTAAAAAAAAA="};
    CHECK_EQUAL(exp_str, ostr.str());
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumWritten<std::size_t>(encoder));
  }

  // byte 0, 1, 2
  {
    std::ostringstream ostr;
    std::size_t length {1};
    Base64Encoder<std::size_t> encoder(ostr, length);

    std::size_t size_data {12345678};
    encoder.Encode(&size_data, length);

    std::string exp_str {"TmG8AAAAAAA="};
    CHECK_EQUAL(exp_str, ostr.str());
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumWritten<std::size_t>(encoder));
  }

  // byte 0, 1, 2, 3
  {
    std::ostringstream ostr;
    std::size_t length {1};
    Base64Encoder<std::size_t> encoder(ostr, length);

    std::size_t size_data {1234567890};
    encoder.Encode(&size_data, length);

    std::string exp_str {"0gKWSQAAAAA="};
    CHECK_EQUAL(exp_str, ostr.str());
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumWritten<std::size_t>(encoder));
  }

  // byte 0, 1, 2, 3, 4
  {
    std::ostringstream ostr;
    std::size_t length {1};
    Base64Encoder<std::size_t> encoder(ostr, length);

    std::size_t size_data {123456789012};
    encoder.Encode(&size_data, length);

    std::string exp_str {"FBqZvhwAAAA="};
    CHECK_EQUAL(exp_str, ostr.str());
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumWritten<std::size_t>(encoder));
  }

  // byte 0, 1, 2, 3, 4, 5
  {
    std::ostringstream ostr;
    std::size_t length {1};
    Base64Encoder<std::size_t> encoder(ostr, length);

    std::size_t size_data {12345678901234};
    encoder.Encode(&size_data, length);

    std::string exp_str {"8i/OczoLAAA="};
    CHECK_EQUAL(exp_str, ostr.str());
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumWritten<std::size_t>(encoder));
  }

  // byte 0, 1, 2, 3, 4, 5, 6
  {
    std::ostringstream ostr;
    std::size_t length {1};
    Base64Encoder<std::size_t> encoder(ostr, length);

    std::size_t size_data {1234567890123456};
    encoder.Encode(&size_data, length);

    std::string exp_str {"wLqKPNViBAA="};
    CHECK_EQUAL(exp_str, ostr.str());
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumWritten<std::size_t>(encoder));
  }

  // byte 0, 1, 2, 3, 4, 5, 6, 7
  {
    std::ostringstream ostr;
    std::size_t length {1};
    Base64Encoder<std::size_t> encoder(ostr, length);

    std::size_t size_data {1234567890123456789};
    encoder.Encode(&size_data, length);

    std::string exp_str {"FYHpffQQIhE="};
    CHECK_EQUAL(exp_str, ostr.str());
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumWritten<std::size_t>(encoder));
  }
}

TEST(TestBase64Encoder_Encode_Bool)
{
  TestBase64Encoder tester;

  std::ostringstream ostr;
  std::vector<gsl::index> data {123, 12345, 12345678, 1234567890,
      123456789012, 1245678901234, 1234567890123456, 1234567890123456789};
  std::size_t length {data.size() * sizeof(gsl::index)};
  Base64Encoder<bool> encoder(ostr, length);

  auto p_data {reinterpret_cast<bool*>(data.data())};
  encoder.Encode(p_data, length);

  std::string exp_str {"ewAAAAAAAAA5MAAAAAAAAE5hvAAAAAAA0gKWSQAAAAAUGpm+"
      "HAAAAPKXPwgiAQAAwLqKPNViBAAVgel99BAiEQ=="};
  CHECK_EQUAL(exp_str, ostr.str());
  CHECK_EQUAL(length, tester.GetNumWritten<bool>(encoder));
}

TEST(TestBase64Decoder_Constructor)
{
  TestBase64Decoder tester;
  std::istringstream istr;
  std::size_t length_1 {1};
  Base64Decoder<std::size_t> decoder_1(istr, length_1);

  CHECK_EQUAL(length_1 * sizeof(std::size_t),
      tester.GetCharFullLength<std::size_t>(decoder_1));
  CHECK_EQUAL(0, tester.GetNumRead(decoder_1));
  CHECK_EQUAL(3, tester.GetOverflowIndex(decoder_1));

  std::size_t length_2 {2};
  Base64Decoder<bool> decoder_2(istr, length_2);

  CHECK_EQUAL(length_2 * sizeof(bool),
      tester.GetCharFullLength<bool>(decoder_2));
  CHECK_EQUAL(0, tester.GetNumRead<bool>(decoder_2));
  CHECK_EQUAL(3, tester.GetOverflowIndex<bool>(decoder_2));
}

TEST(TestBase64Decoder_Decode_Size)
{
  TestBase64Decoder tester;
  // std::size_t occupies 8 bytes, we are testing 8 different number which
  // will occupy different number of bytes of std::size_t

  // byte 0
  {
    std::istringstream istr;
    std::size_t length {1};
    Base64Decoder<std::size_t> decoder(istr, length);

    istr.str("ewAAAAAAAAA=");

    std::size_t size_data {};
    decoder.Decode(&size_data, length);

    std::size_t exp_size {123};
    CHECK_EQUAL(exp_size, size_data);
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumRead<std::size_t>(decoder));
  }

  // byte 0, 1
  {
    std::istringstream istr;
    std::size_t length {1};
    Base64Decoder<std::size_t> decoder(istr, length);

    istr.str("OTAAAAAAAAA=");

    std::size_t size_data {};
    decoder.Decode(&size_data, length);

    std::size_t exp_size {12345};
    CHECK_EQUAL(exp_size, size_data);
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumRead<std::size_t>(decoder));
  }

  // byte 0, 1, 2
  {
    std::istringstream istr;
    std::size_t length {1};
    Base64Decoder<std::size_t> decoder(istr, length);

    istr.str("TmG8AAAAAAA=");

    std::size_t size_data {};
    decoder.Decode(&size_data, length);

    std::size_t exp_size {12345678};
    CHECK_EQUAL(exp_size, size_data);
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumRead<std::size_t>(decoder));
  }

  // byte 0, 1, 2, 3
  {
    std::istringstream istr;
    std::size_t length {1};
    Base64Decoder<std::size_t> decoder(istr, length);

    istr.str("0gKWSQAAAAA=");

    std::size_t size_data {};
    decoder.Decode(&size_data, length);

    std::size_t exp_size {1234567890};
    CHECK_EQUAL(exp_size, size_data);
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumRead<std::size_t>(decoder));
  }

  // byte 0, 1, 2, 3, 4
  {
    std::istringstream istr;
    std::size_t length {1};
    Base64Decoder<std::size_t> decoder(istr, length);

    istr.str("FBqZvhwAAAA=");

    std::size_t size_data {};
    decoder.Decode(&size_data, length);

    std::size_t exp_size {123456789012};
    CHECK_EQUAL(exp_size, size_data);
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumRead<std::size_t>(decoder));
  }

  // byte 0, 1, 2, 3, 4, 5
  {
    std::istringstream istr;
    std::size_t length {1};
    Base64Decoder<std::size_t> decoder(istr, length);

    istr.str("8i/OczoLAAA=");

    std::size_t size_data {};
    decoder.Decode(&size_data, length);

    std::size_t exp_size {12345678901234};
    CHECK_EQUAL(exp_size, size_data);
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumRead<std::size_t>(decoder));
  }

  // byte 0, 1, 2, 3, 4, 5, 6
  {
    std::istringstream istr;
    std::size_t length {1};
    Base64Decoder<std::size_t> decoder(istr, length);

    istr.str("wLqKPNViBAA=");

    std::size_t size_data {};
    decoder.Decode(&size_data, length);

    std::size_t exp_size {1234567890123456};
    CHECK_EQUAL(exp_size, size_data);
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumRead<std::size_t>(decoder));
  }

  // byte 0, 1, 2, 3, 4, 5, 6, 7
  {
    std::istringstream istr;
    std::size_t length {1};
    Base64Decoder<std::size_t> decoder(istr, length);

    istr.str("FYHpffQQIhE=");

    std::size_t size_data {};
    decoder.Decode(&size_data, length);

    std::size_t exp_size {1234567890123456789};
    CHECK_EQUAL(exp_size, size_data);
    CHECK_EQUAL(length * sizeof(std::size_t),
        tester.GetNumRead<std::size_t>(decoder));
  }
}

TEST(TestBase64Decoder_Decode_Bool)
{
  TestBase64Decoder tester;

  std::istringstream istr;
  std::vector<gsl::index> exp_data {123, 12345, 12345678, 1234567890,
      123456789012, 1245678901234, 1234567890123456, 1234567890123456789};
  std::size_t length {exp_data.size() * sizeof(gsl::index)};
  Base64Decoder<bool> decoder(istr, length);

  istr.str("ewAAAAAAAAA5MAAAAAAAAE5hvAAAAAAA0gKWSQAAAAAUGpm+"
      "HAAAAPKXPwgiAQAAwLqKPNViBAAVgel99BAiEQ==");

  std::vector<gsl::index> data(exp_data.size());
  auto p_data {reinterpret_cast<bool*>(data.data())};
  decoder.Decode(p_data, length);

  CHECK(testutil::CheckEqualVector(data, exp_data));
  CHECK_EQUAL(length, tester.GetNumRead<bool>(decoder));
}
}
}  // namespace iblbm
