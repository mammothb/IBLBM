#ifndef IO_SRC_GETCURRENTWORKINGDIRECTORY_HPP_
#define IO_SRC_GETCURRENTWORKINGDIRECTORY_HPP_

#include <string>

namespace iblbm
{
/**
 * \return Get the current working directory. Will not include a trailing
 * slash.
 */
std::string GetCurrentWorkingDirectory();
}  // namespace iblbm

#endif  // IO_SRC_GETCURRENTWORKINGDIRECTORY_HPP_
