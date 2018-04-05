#include "OutputFileHandler.hpp"

#include <cstdlib>

#include "Exception.hpp"
#include "FileFinder.hpp"
#include "MpiManager.hpp"

namespace iblbm
{
const std::string OutputFileHandler::SIG_FILENAME(".iblbm_deletable_folder");

/**
 * Recursively remove the contents of the given folder, but leave any hidden
 * files present at the top level.
 *
 * @param rPath path to the directory to clean
 * @param isTop whether this is the top level directory
 */
void CleanFolder(
    const fs::path& rPath
  , bool isTop = true);

void CleanFolder(
    const fs::path& rPath
  , bool isTop)
{
  assert(fs::is_directory(rPath));
  fs::directory_iterator it_end;
  // First recursively remove the children
  for (fs::directory_iterator it_dir(rPath); it_dir != it_end; ++it_dir) {
    if (fs::is_directory(it_dir->status())) {
      CleanFolder(it_dir->path(), false);
    }
    else {
      const fs::path& r_item_path(it_dir->path());
      if (!isTop || PATH_LEAF_NAME(r_item_path)[0] != '.') {
        fs::remove(r_item_path);
      }
    }
  }
  // Now remove the folder itself, if not top
  if (!isTop) fs::remove(rPath);
}

OutputFileHandler::OutputFileHandler(
    const std::string& rDirectory
  , bool cleanOutputDirectory/*=true*/)
{
  CommonConstructor(rDirectory, cleanOutputDirectory);
}

OutputFileHandler::OutputFileHandler(
    const FileFinder& rDirectory
  , bool cleanOutputDirectory/*=true*/)
{
  FileFinder output_root("", RelativeTo::IBLBM_TEST_OUTPUT);
  std::string relative_path;
  try {
    relative_path = rDirectory.GetRelativePath(output_root);
  }
  catch (const Exception&) {
    EXCEPTION("The location provided to OutputFileHandler must be inside "
        "IBLBM_TEST_OUTPUT; '" << rDirectory.GetAbsolutePath() <<
        "' is not under '" << output_root.GetAbsolutePath() << "'.");
  }
  if (*output_root.GetAbsolutePath().rbegin() != '/' &&
      !relative_path.empty()) {
    assert(*relative_path.begin() == '/');
    relative_path.erase(0, 1);  // Remove leading slash
  }
  CommonConstructor(relative_path, cleanOutputDirectory);
}

std::string OutputFileHandler::GetOutputDirectoryFullPath() const
{
  return mDirectory;
}

std::string OutputFileHandler::GetRelativePath() const
{
  FileFinder output_root("", RelativeTo::IBLBM_TEST_OUTPUT);
  auto relative_path {FindFile("").GetRelativePath(output_root)};
  // Remove trailing slash
  if (!relative_path.empty() && *relative_path.rbegin() == '/')
      relative_path.erase(--relative_path.end());
  return relative_path;
}

out_stream OutputFileHandler::OpenOutputFile(
    const std::string& rFilename
  , std::ios_base::openmode mode/*=std::ios::out | std::ios::trunc*/) const
{
  auto p_output_file {std::make_shared<std::ofstream>(mDirectory + rFilename,
      mode)};
  if (!p_output_file->is_open()) {
    EXCEPTION("Could not open file \"" + rFilename + "\" in " + mDirectory);
  }
  return p_output_file;
}

out_stream OutputFileHandler::OpenOutputFile(
    const std::string& rFilename
  , unsigned number
  , const std::string& rFileExtension
  , std::ios_base::openmode mode/*=std::ios::out | std::ios::trunc*/) const
{
  std::stringstream string_stream;
  string_stream << rFilename << number << rFileExtension;
  return OpenOutputFile(string_stream.str(), mode);
}

FileFinder OutputFileHandler::CopyFileTo(const FileFinder& rSourceFile) const
{
  if (!rSourceFile.IsFile()) {
    EXCEPTION("Can only copy single files:\n" <<
        rSourceFile.GetAbsolutePath() << " is not a file.");
  }
  fs::path from_path(rSourceFile.GetAbsolutePath());
  fs::path to_path(GetOutputDirectoryFullPath());
  to_path /= from_path.leaf();
  if (MpiManager::Instance().AmMaster()) {
    try {
      fs::copy_file(from_path, to_path);
    }
    // LCOV_EXCL_START
    catch (const fs::filesystem_error& err) {
      TERMINATE("Error copying file '" << rSourceFile.GetAbsolutePath() <<
          "': " << err.what());
    }
    // LCOV_EXCL_STOP
  }
  MpiManager::Instance().Barrier("OutputFileHandler::CopyFileTo");
  return FileFinder(to_path.string(), RelativeTo::ABSOLUTE);
}

FileFinder OutputFileHandler::FindFile(std::string leafName) const
{
  return FileFinder(GetOutputDirectoryFullPath() + leafName,
      RelativeTo::ABSOLUTE);
}

std::string OutputFileHandler::GetIblbmTestOutputDirectory()
{
  char* iblbm_test_output {getenv("IBLBM_TEST_OUTPUT")};
  FileFinder directory_root;
  if (iblbm_test_output == nullptr || *iblbm_test_output == 0) {
    // Default to 'testoutput' folder within the current directory
    directory_root.SetPath("testoutput", RelativeTo::CWD);
  }
  else {
    directory_root.SetPath(iblbm_test_output, RelativeTo::ABSOLUTE_OR_CWD);
  }
  // Note that FileFinder::GetAbsolutePath adds a trailing slash, but only
  // if the directory exists at the time of the call
  auto iblbm_test_output_directory {directory_root.GetAbsolutePath()};
  AddTrailingSlash(iblbm_test_output_directory);
  return iblbm_test_output_directory;
}

void OutputFileHandler::CommonConstructor(
    const std::string& rDirectory
  , bool cleanOutputDirectory)
{
  // Is it a valid request for a directory?
  if (rDirectory.find("..") != std::string::npos) {
    EXCEPTION("Will not create directory: " + rDirectory + " due to it " +
        "potentially being above, and cleaning, IBLBM_TEST_OUTPUT.");
  }
  // The notion of absolute path on Windows is rather different.
  // For example, / and /foo are not absolute paths.
  // However, fs::path.has_root_path() captures the intended semantics here
  // as follows

  if (fs::path(rDirectory).has_root_path()) {
    EXCEPTION("The constructor argument to OutputFileHandler must be a "
        "relative path; '" << rDirectory << "' is absolute.");
  }

  mDirectory = MakeFoldersAndReturnFullPath(rDirectory);

  // Clean directory but don't ever clean IBLBM_TEST_OUTPUT at the top level
  if (rDirectory != "" && cleanOutputDirectory) {
    FileFinder sig_file(mDirectory + SIG_FILENAME, RelativeTo::ABSOLUTE);
    if (!sig_file.Exists()) {
      EXCEPTION("Cannot delete " + mDirectory + " because signature " +
          "file \"" + SIG_FILENAME + "\" is not present.");
    }

    // Are we the master process? Only the master should delete files
    if (MpiManager::Instance().AmMaster()) CleanFolder(mDirectory);
    // Wait for master to finish before going on to use the directory.
    MpiManager::Instance().Barrier("OutputFileHandler");
  }
}

void OutputFileHandler::AddTrailingSlash(std::string& rDirectory)
{
  // Add a trailing slash if not already there
  if (rDirectory != "" && *rDirectory.rbegin() != '/')
      rDirectory = rDirectory + "/";
}

std::string OutputFileHandler::MakeFoldersAndReturnFullPath(
    const std::string& rDirectory) const
{
  fs::path output_root(GetIblbmTestOutputDirectory());
  fs::path rel_path(rDirectory);

  // rDirectory has a trailing slash, which gives an unhelpful last component
  if (!rel_path.empty() && *(--rel_path.end()) == ".")
      rel_path.remove_leaf();

  // Make master wait (because other processes may be checking whether a
  // directory exists)
  MpiManager::Instance().Barrier(
      "OutputFileHandler::MakeFoldersAndReturnFullPathBeforeCreation");
  // Are we the master process? Only the master should make any new
  // directories
  if (MpiManager::Instance().AmMaster()) {
    try {
      // If necessary make the IblbmTestOutputDirectory - don't make it
      // deleteable by IBLBM
      // Note that this is a no-op if the folder exists already
      fs::create_directories(output_root);

      // Now make all the sub-folders requested one-by-one and add the
      // .iblbm_deletable_folder file to them
      fs::path next_folder {output_root};
      for (auto it_path = rel_path.begin(); it_path != rel_path.end();
          ++it_path) {
        next_folder /= *it_path;
        auto created_dir {fs::create_directory(next_folder)};
        if (created_dir) {
          // Add the IBLBM signature file
          fs::ofstream sig_file(next_folder / SIG_FILENAME);
          sig_file.close();
        }
      }
    }
    // LCOV_EXCL_START
    catch (const fs::filesystem_error& err) {
      TERMINATE("Error making test output folder: " << err.what());
    }
    // LCOV_EXCL_STOP
  }

  // Wait for master to finish before going on to use the directory.
  MpiManager::Instance().Barrier(
      "OutputFileHandler::MakeFoldersAndReturnFullPath");

  auto path_with_slash {(output_root / rel_path).string()};
  AddTrailingSlash(path_with_slash);
  return path_with_slash;
}
}  // namespace iblbm
