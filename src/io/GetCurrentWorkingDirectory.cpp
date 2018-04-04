#include "GetCurrentWorkingDirectory.hpp"

#include "BoostFilesystem.hpp"
#include "Exception.hpp"

namespace iblbm
{
std::string GetCurrentWorkingDirectory()
{
  fs::path cwd;
  try {
    cwd = fs::current_path();
  }
  // LCOV_EXCL_START
  catch (...) {
    // Not sure what could cause this, so not covered, but just in case...
    EXCEPTION("Unable to determine current working directory");
  }
  // LCOV_EXCL_STOP
  return cwd.string();
}
}  // namespace iblbm
