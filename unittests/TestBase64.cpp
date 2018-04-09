#include <sstream>

#include "UnitTest++/UnitTest++.h"
#include "Base64.hpp"

namespace iblbm
{
SUITE(TestIo)
{
TEST(TestBase64Encoder_Constructor)
{
  std::ostringstream ostr;
  Base64Encoder<bool> encoder(ostr, 1);
}
}
}  // namespace iblbm
