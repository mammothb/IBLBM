#ifndef SRC_IO_OUTPUTFILEHANDLER_HPP_
#define SRC_IO_OUTPUTFILEHANDLER_HPP_

#include <fstream>
#include <ios>
#include <memory>
#include <string>

namespace iblbm
{
class FileFinder;  // Avoid circular includes

/** Type of our output streams; a managed pointer to an std::ofstream. */
typedef std::shared_ptr<std::ofstream> out_stream;

class OutputFileHandler
{
 public:
  /**
   * Create an OutputFileHandler that will create output files in the given
   * directory. The directory name should be relative to the place where
   * IBLBM test output is stored. If the user needs to know where this is,
   * use the GetIblbmTestOutputDirectory method.
   *
   * Will check that the directory exists and create it if needed.
   *
   * \note This MUST be called collectively, since the directory creation
   * routine contains a barrier call.
   *
   * \param rDirectory  the directory to put output files in.
   * \param cleanOutputDirectory  whether to remove any existing files in the
   *        output directory
   */
  OutputFileHandler(
      const std::string& rDirectory
    , bool cleanOutputDirectory = true);

  /**
   * Create an OutputFileHandler that will create output files in the given
   * directory. This must be located inside the folder where Chaste test
   * output is stored, and will typically be obtained with the FindFile
   * method of this class from a parent handler.
   *
   * Will check that the directory exists and create it if needed.
   *
   * @note This MUST be called collectively, since it contains a barrier call.
   *
   * @param rDirectory  the directory to put output files in
   * @param cleanOutputDirectory  whether to remove any existing files in the output directory
   */
  OutputFileHandler(
      const FileFinder& rDirectory
    , bool cleanOutputDirectory = true);

  /**
   * \return the full pathname to the directory this object will create files
   * in.
   */
  std::string GetOutputDirectoryFullPath() const;

  /**
   * \return the path to this output directory, relative to the IBLBm root
   * output folder.
   */
  std::string GetRelativePath() const;

  /**
   * Open an output file in our directory, and check it was opened
   * successfully. Throws an Exception if not.
   *
   * \param rFileName the name of the file to open, relative to the output
   *        directory.
   * \param mode optionally, flags to use when opening the file (defaults
   *        are as for std::ofstream).
   *
   * \return a managed pointer to the opened file stream.
   */
  out_stream OpenOutputFile(
      const std::string& rFilename
    , std::ios_base::openmode mode = std::ios::out | std::ios::trunc) const;

  /**
   * This just calls the other OpenOutputFile after concatenating the first
   * three arguments together to make the full filename. For example
   * OpenOutputFile("results_", 3, ".dat") creates results_3.dat. See
   * documentation for OpenOutputFile(std::string, std::ios_base::openmode).
   *
   * \param rFileName the root name of the file to open
   * \param number the number to append to the root name of the file
   * \param rFileExtension the file extension
   * \param mode optionally, flags to use when opening the file (defaults are
   *        as for std::ofstream).
   *
   * \return a managed pointer to the opened file stream.
   */
  out_stream OpenOutputFile(
      const std::string& rFilename
    , unsigned number
    , const std::string& rFileExtension
    , std::ios_base::openmode mode = std::ios::out | std::ios::trunc) const;

  /**
   * Copy the given file to this output directory.
   *
   * \note This MUST be called collectively, since it contains a barrier
   *       call.
   *
   * \param rSourceFile the file to copy
   * \return the copied file
   */
  FileFinder CopyFileTo(const FileFinder& rSourceFile) const;

  /**
   * \param leafName  the name of the file to find
   *
   * \return a FileFinder for a file in this output directory.
   */
  FileFinder FindFile(std::string leafName) const;

  /**
   * Static method for getting the test output directory (the directory where
   * chaste stores test output).  This is set from the environment variable
   * IBLBM_TEST_OUTPUT, and defaults to "./testoutput" if it is not set.
   *
   * Attempts to return an absolute path, but may get confused by odd setups.
   *
   * Static so an output file handler does not have to be created if the test
   * output directory is wanted for, say, reading a file.
   * \return the test output directory name (IBLBM_TEST_OUTPUT)
   */
  static std::string GetIblbmTestOutputDirectory();

  /**
   * Static method for getting the parallel output filename for logging.
   * The filename has the following format:
   * - <rFilename>_rank<rank>_size<size>.<rFileExtension>
   * <size> is omitted if appendSize if FALSE
   *
   * \param rFilename Base filename without suffixes
   * \param rFileExtension file format
   * \param appendSize Flag to determine if we are appending the total number
   *        of processes to the filename
   *
   * \return name of the constructed filename
   */
  static std::string GetParallelOutputFilename(
      const std::string& rFilename
    , const std::string& rFileExtension
    , bool appendSize = true);

  /** The name of the IBLBM signature file added to folders we create. */
  static const std::string SIG_FILENAME;

 private:
  /**
   * Functionality common to both constructors.
   *
   * \param rDirectory relative path to the directory to put output files in
   * \param cleanOutputDirectory whether to remove any existing files in the
   *        output directory
   */
  void CommonConstructor(
      const std::string& rDirectory
    , bool cleanOutputDirectory);

  /**
   * Takes a reference to a string and adds a trailing slash if one is not
   * already present
   *
   * \param rDirectory  The directory name to add a trailing slash to.
   */
  static void AddTrailingSlash(std::string& rDirectory);

  /**
   * Check that the desired output directory exists and is writable by us.
   * Create it if needed.
   * Return the full pathname of the output directory.
   *
   * The environment variable CHASTE_TEST_OUTPUT will be examined.  If it is
   * set and non-empty it is taken to be a directory where test output should
   * be stored. Otherwise the current directory is used.
   *
   * \note Contains a barrier, so must be called collectively.
   *
   * \param rDirectory  pathname of the output directory, relative to where
   *        IBLBM output will be stored (user shouldn't care about this).
   *
   * \return full pathname to the output directory
   */
  std::string MakeFoldersAndReturnFullPath(
      const std::string& rDirectory) const;

  /** The directory to store output files in (always ends in "/") */
  std::string mDirectory;
};
}  // namespace iblbm

#endif  // SRC_IO_OUTPUTFILEHANDLER_HPP_
