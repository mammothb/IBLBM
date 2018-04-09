#include "Base64.hpp"

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

template class Base64Encoder<bool>;
template class Base64Encoder<std::size_t>;
}  // namespace iblbm
