#ifndef SRC_IO_BASE64_HPP_
#define SRC_IO_BASE64_HPP_

#include <cstddef>
#include <iosfwd>

#include "gsl/gsl"

namespace iblbm
{
// https://stackoverflow.com/a/41094722/4115600
template<typename T>
class Base64Encoder
{
 public:
  Base64Encoder(
      std::ostream& rOstream
    , std::size_t fullLength);

 private:
  /** Base64 alphabets for encoding */
  static const char msEnc64[65];
  std::ostream& mrOstream;
  std::size_t mCharFullLength;
  std::size_t mNumWritten;
  gsl::index mOverflowIndex;
};

class Base64Decoder
{
 public:
};
}  // namespace iblbm

#endif  // SRC_IO_BASE64_HPP_
