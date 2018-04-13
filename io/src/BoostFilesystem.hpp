#ifndef SRC_IO_BOOSTFILESYSTEM_HPP_
#define SRC_IO_BOOSTFILESYSTEM_HPP_

/**
 * \file
 * Include the Boost Filesystem library headers, and set up the 'fs'
 * namespace alias.
 * This header also ensures that we use version 2 of the library when
 * available.
 */

#include <boost/version.hpp>

#if BOOST_VERSION <= 104900
/** Which version of the library to use. */
#define BOOST_FILESYSTEM_VERSION 2
/** How to get a leafname as a string (in version 2). */
#define PATH_LEAF_NAME(path) path.leaf()
#else
/** How to get a leafname as a string (in version 3). */
#define PATH_LEAF_NAME(path) path.leaf().string()
#endif

/**
 * There is potential binary incompatibility when linking the filesystem
 * library (built with C++98) and Chaste (built with C++11).  This is due to
 * the use of scoped enums in the filesystem library, which have to be
 * emulated pre-C++11 and thus generate different decorated function names.
 * See #2811 and https://svn.boost.org/trac10/ticket/6779 for details.
 * This was resolved in Boost 1.57, so we special-case the older versions.
 */
#if BOOST_VERSION <= 105600
/** Change boost SCOPED_ENUMS behavior on older versions */
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#else
#include <boost/filesystem.hpp>
#endif
#include <boost/filesystem/fstream.hpp>

namespace fs = boost::filesystem;

#endif  // SRC_IO_BOOSTFILESYSTEM_HPP_
