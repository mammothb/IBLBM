#ifndef IO_SRC_IBLBMBUILDROOT_HPP_
#define IO_SRC_IBLBMBUILDROOT_HPP_

#include <string>

namespace iblbm
{
/**
 * \return the path to the root directory of the IBLBM build tree.
 * Will always give you the absolute path with a trailing slash.
 */
const char* IblbmBuildRootDir();

/**
 * \return the path to the root directory of the IBLBM source tree.
 * Will always give you the absolute path with a trailing slash.
 */
const char* IblbmSourceRootDir();
}  // namespace iblbm

#endif  // IO_SRC_IBLBMBUILDROOT_HPP_
