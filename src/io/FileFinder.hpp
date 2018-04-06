#ifndef SRC_IO_FILEFINDER_HPP_
#define SRC_IO_FILEFINDER_HPP_

#include <string>

#include "BoostFilesystem.hpp"

namespace iblbm
{
/**
 * Enum class of path 'types', i.e., what a path can be relative to. Allows
 * us to write things like RelativeTo::IBLBM_TEST_OUTPUT for readability.
 */
enum class RelativeTo
{
  CWD,  // The current working directory
  IBLBM_TEST_OUTPUT,  // The CHASTE_TEST_OUTPUT folder
  IBLBM_SOURCE_ROOT,  // The root of the Chaste source tree
  IBLBM_BUILD_ROOT,  // The root of the Chaste build tree
  ABSOLUTE,  // This is an absolute path
  ABSOLUTE_OR_CWD  // If it starts with a / then it's absolute, otherwise
                   // interpret relative to CWD
};

/**
 * A helper class for finding files or directories, given paths which can be
 * relative to various locations (e.g. the IBLBM source tree root, the
 * current directory, the IBLBM test output directory, or an absolute path).
 */
class FileFinder
{
 public:
  /**
   * Default constructor for subclasses to use. They should call SetPath() in
   * their constructor.
   *
   * This also allows classes to store a FileFinder instance that hasn't
   * been properly set up yet, and assign to it later using operator=.
   */
  FileFinder();

  /**
   * Main constructor.
   *
   * \param rPath  the path to the file/dir to find
   * \param relativeTo  how to interpret this path
   */
  FileFinder(
      const std::string& rPath
    , RelativeTo relativeTo);

  /**
   * Find a file (or folder) relative to some file or directory.
   * If the second argument is a directory, we look for the given leaf name
   * within it. If the second argument is a file, then we look for a sibling.
   * An exception is raised if rParentOrSibling does not exist.
   *
   * @param rLeafName the leaf name of the file/dir to find
   * @param rParentOrSibling where to look for it
   */
  FileFinder(
      const std::string& rLeafName
    , const FileFinder& rParentOrSibling);

  /**
   * Conversion constructor from a Boost Filesystem path object.
   * Note that since fs::path has a conversion constructor from std::string,
   * this allows us to be initialized with a string or character constant,
   * too. The path will be interpreted as relative to the current working
   * directory, unless it is an absolute path.
   *
   * \param rPath the path to the file/dir to find
   */
  explicit FileFinder(const fs::path& rPath);

  virtual ~FileFinder() = default;

  /**
   * Change this FileFinder to point at a new location.
   *
   * \param rPath  the path to the file/dir to find
   * \param relativeTo  how to interpret this path
   */
  virtual void SetPath(
      const std::string& rPath
    , RelativeTo relativeTo);

  /**
   * Change this FileFinder to point at a new location, relative to some file
   * or directory.
   *
   * @param rLeafName the leaf name of the file/dir to find
   * @param rParentOrSibling  where to look for it
   */
  virtual void SetPath(
      const std::string& rLeafName
    , const FileFinder& rParentOrSibling);

  /** \return true if this FileFinder has been given a path. */
  bool IsPathSet() const;

  /** \return true if we exist. */
  bool Exists() const;

  /** \return true if we are pointing at a file */
  bool IsFile() const;

  /** \return true if we are pointing at a directory */
  bool IsDir() const;

  /**
   * \return true if this is a file of size zero or if this is a folder,
   * whether it contains no non-hidden items. If this doesn't exist, throws.
   */
  bool IsEmpty() const;

  /**
   * If this is a directory that exists (at the instant of this call), the
   * absolute path is guaranteed to end in a '/'.  Otherwise, the path is
   * guaranteed not to end in a '/'.
   *
   * \return the absolute path to this file/dir.
   */
  std::string GetAbsolutePath() const;

  /**
   * \return the leaf name of this file or directory.
   *
   * i.e. the individual file or directory name and none of the preceding
   * folders on its path.
   */
  std::string GetLeafName() const;

  /**
   * \return the leaf name of this file or directory, with any file extension
   *         removed.
   *
   * i.e. the individual file or directory name and none of the preceding
   * folders on its path.
   */
  std::string GetLeafNameNoExtension() const;

  /**
   * \return the extension of the leaf name of this file or directory, if any
   * The '.' will be included in the extension if an extension exists.
   */
  std::string GetExtension() const;

  /**
   * @return a finder for the folder containing this file or directory.
   */
  FileFinder GetParent() const;

  /**
   * \param rBasePath  where the returned path should be relative to
   *
   * \return the relative path to this finder from another.  Throws if this
   * is not found under rBasePath.
   */
  std::string GetRelativePath(const FileFinder& rBasePath) const;

  /**
   * Recursively remove this file or folder. Since this is a potentially very
   * dangerous operation, only locations under the IBLBM test output folder
   * may be removed.
   *
   * Only folders created by an OutputFileHandler, or the contents of such a
   * folder, may be deleted (folders that have .chaste_deletable_folder
   * present).
   *
   * If you need to delete a file or folder without .chaste_deletable_folder,
   * then you have to use DangerousRemove().
   */
  void Remove() const;

  /**
   * This method will allow you to remove any file from under either
   *  * CHASTE_TEST_OUTPUTor
   *  * the source tree
   * (but not elsewhere).
   *
   * For this reason it is a very dangerous operation and should not be used
   * if Remove could be instead.
   *
   * BEWARE: if you have managed to set IBLBM_TEST_OUTPUT to "/" this could
   * wipe your system!
   */
  void DangerousRemove() const;

  /**
   * \param rPath  the path to test
   *
   * \return true if the path is absolute.
   */
  static bool IsAbsolutePath(const std::string& rPath);

 private:
  /**
   * This is code common to Remove() and DangerousRemove(). Should remain
   * private and not to be called from elsewhere. Remove() is only allowed to
   * delete things with a .chaste_deletable_folder in the testoutput
   * directory.
   *
   * DangerousRemove() is allowed to delete anything in the source or
   * testoutput directories.
   *
   * \param isDangerous whether we are doing a dangerous remove.
   */
  void PrivateRemove(bool isDangerous = false) const;
  /** The absolute path to our file. */
  std::string mAbsPath;
  /** Whether to fake one of the fixed paths, e.g. IblbmSourceRoot. */
  static bool msIsFaking;
  /** Which path to fake. */
  static RelativeTo msFakeWhat;
  /** The fake value of the faked path. */
  static std::string msFakePath;
};
}

#endif  // SRC_IO_FILEFINDER_HPP_
