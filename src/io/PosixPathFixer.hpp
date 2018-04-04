#ifndef SRC_IO_POSIXPATHFIXER_HPP_
#define SRC_IO_POSIXPATHFIXER_HPP_

#include "BoostFilesystem.hpp"

namespace iblbm
{
/**
 * A helper struct to convert native paths into POSIX syntax.
 * Mainly for Windows.
 */
struct PosixPathFixer
{
 public:
  /**
   * \param path the native path to convert
   *
   * \return a POSIX string representation of path
   */
  static std::string Fix(const fs::path path)
  {
#ifdef _MSC_VER
    return path.generic_string();
#else
     // Doesn't matter on systems that already use POSIX path format
    return path.string();
#endif
  }
};
}  // namespace iblbm

#endif // SRC_IO_POSIXPATHFIXER_HPP_
