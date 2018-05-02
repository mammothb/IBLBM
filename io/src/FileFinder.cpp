#include "FileFinder.hpp"

#include "Exception.hpp"
#include "GetCurrentWorkingDirectory.hpp"
#include "IblbmBuildRoot.hpp"
#include "OutputFileHandler.hpp"
#include "PosixPathFixer.hpp"

namespace iblbm
{
constexpr auto UNSET_PATH = "UNSET!";

/**
 * This macro converts boost filesystem exceptions into IBLBM Exceptions.
 *
 * It should be put round any calls in this class' public methods to
 * either:
 *  * boost filesystem
 *  * or private methods of this class
 *
 * that are likely to throw.
 *
 * \param code some code that could throw a boost file system error
 */
#define CONVERT_ERROR(code)                  \
  try {                                      \
    code;                                    \
  }                                          \
  catch (const fs::filesystem_error& err) {  \
    EXCEPTION(err.what());                   \
  }

FileFinder::FileFinder()
  : mAbsPath{UNSET_PATH}
{}

FileFinder::FileFinder(
    const std::string& rRelativePath
  , RelativeTo relativeTo)
{
  SetPath(rRelativePath, relativeTo);
}

FileFinder::FileFinder(
    const std::string& rLeafName
  , const FileFinder& rParentOrSibling)
{
  SetPath(rLeafName, rParentOrSibling);
}

FileFinder::FileFinder(const fs::path& rPath)
{
  SetPath(fs::complete(rPath).string(), RelativeTo::ABSOLUTE);
}

void FileFinder::SetPath(
    const std::string& rRelativePath
  , RelativeTo relativeTo)
{
  switch (relativeTo) {
  case RelativeTo::IBLBM_SOURCE_ROOT:
      mAbsPath = IblbmSourceRootDir() + rRelativePath;
    break;
  case RelativeTo::IBLBM_BUILD_ROOT:
      mAbsPath = IblbmBuildRootDir() + rRelativePath;
    break;
  case RelativeTo::IBLBM_TEST_OUTPUT:
    mAbsPath = OutputFileHandler::GetIblbmTestOutputDirectory() +
        rRelativePath;
    break;
  case RelativeTo::CWD:
    mAbsPath = GetCurrentWorkingDirectory() + "/" + rRelativePath;
    break;
  case RelativeTo::ABSOLUTE:
    mAbsPath = rRelativePath;
    break;
  case RelativeTo::ABSOLUTE_OR_CWD:
    if (FileFinder::IsAbsolutePath(rRelativePath)) {
      mAbsPath = rRelativePath;
    }
    else {
      mAbsPath = GetCurrentWorkingDirectory() + "/" + rRelativePath;
    }
    break;
  // LCOV_EXCL_START
  default:
    NEVER_REACHED;
  // LCOV_EXCL_STOP
  }

  // Remove any trailing /
  auto it {mAbsPath.end()};
  // Iterator was decremented in the while test
  while (it != mAbsPath.begin() && *(--it) == '/') {}
  // it now points at the last non-slash character, if any
  if (it != mAbsPath.end() && (++it) != mAbsPath.end())
      mAbsPath.erase(it, mAbsPath.end());
}

void FileFinder::SetPath(
    const std::string& rLeafName
  , const FileFinder& rParentOrSibling)
{
  if (!rParentOrSibling.Exists()) {
    EXCEPTION("Reference path '" << rParentOrSibling.GetAbsolutePath() <<
        "' does not exist.");
  }
  if (rParentOrSibling.IsDir()) {
    SetPath(rParentOrSibling.GetAbsolutePath() + rLeafName,
        RelativeTo::ABSOLUTE);
  }
  else {
    SetPath(rParentOrSibling.GetParent().GetAbsolutePath() + rLeafName,
        RelativeTo::ABSOLUTE);
  }
}

bool FileFinder::IsPathSet() const
{
  return mAbsPath != UNSET_PATH;
}

bool FileFinder::Exists() const
{
  return fs::exists(mAbsPath);
}

bool FileFinder::IsFile() const
{
  return fs::is_regular(mAbsPath);
}

bool FileFinder::IsDir() const
{
  return fs::is_directory(mAbsPath);
}

bool FileFinder::IsEmpty() const
{
  auto is_empty {true};
  if (IsFile()) {
    is_empty = fs::file_size(mAbsPath) == 0u;
  }
  else if (IsDir()) {
    fs::directory_iterator it_end;
    for (fs::directory_iterator it_dir(mAbsPath); it_dir != it_end;
        ++it_dir) {
      if (PATH_LEAF_NAME(it_dir->path()).substr(0, 1) != ".") {
        is_empty = false;
        break;
      }
    }
  }
  else {
    EXCEPTION("The path '" << mAbsPath << "' does not exist.");
  }
  return is_empty;
}

std::string FileFinder::GetAbsolutePath() const
{
  if (IsDir()) {
    return mAbsPath + '/';
  }
  else {
    return mAbsPath;
  }
}

std::string FileFinder::GetLeafName() const
{
  return PATH_LEAF_NAME(fs::path(mAbsPath));
}

std::string FileFinder::GetLeafNameNoExtension() const
{
  return fs::basename(mAbsPath);
}

std::string FileFinder::GetExtension() const
{
  return fs::extension(mAbsPath);
}

FileFinder FileFinder::GetParent() const
{
  fs::path our_path(mAbsPath);
  EXCEPT_IF_NOT(our_path.has_branch_path());
  return FileFinder(our_path.branch_path().string(), RelativeTo::ABSOLUTE);
}

std::string FileFinder::GetRelativePath(const FileFinder& rBasePath) const
{
  const auto base_path {rBasePath.GetAbsolutePath()};
  const auto our_path {GetAbsolutePath()};
  if (our_path.substr(0, base_path.length()) != base_path) {
    EXCEPTION("The path '" << our_path << "' is not relative to '" <<
        base_path << "'.");
  }
  return our_path.substr(base_path.length());
}

void FileFinder::Remove() const
{
  PrivateRemove();
}

void FileFinder::DangerousRemove() const
{
  PrivateRemove(/*isDangerous=*/true);
}

bool FileFinder::IsAbsolutePath(const std::string& rPath)
{
  return fs::path(rPath).is_complete();
}

/**
 * Helper function for FileFinder::Remove - recursively remove the given
 * path.
 *
 * \param rPath
 */
void RemoveAll(const fs::path& rPath);

void RemoveAll(const fs::path& rPath)
{
  // First recursively remove any children
  if (fs::is_directory(rPath)) {
    fs::directory_iterator it_end;
    for (fs::directory_iterator it_dir(rPath); it_dir != it_end; ++it_dir)
        RemoveAll(it_dir->path());
  }
  // Now remove the item itself
  fs::remove(rPath);
}

void FileFinder::PrivateRemove(bool isDangerous) const
{
  // Test for bad paths
  const auto test_output {OutputFileHandler::GetIblbmTestOutputDirectory()};
  const auto test_output_path {PosixPathFixer::Fix(fs::path(test_output))};
  const auto abs_path {PosixPathFixer::Fix(fs::path(GetAbsolutePath()))};
  auto in_test_output {abs_path.substr(0, test_output_path.length()) ==
      test_output_path};

  if (!in_test_output) {
    if (isDangerous) {
      const auto source_folder {
          FileFinder("", RelativeTo::IBLBM_SOURCE_ROOT).GetAbsolutePath()};
      const auto source_folder_path {
          PosixPathFixer::Fix(fs::path(source_folder))};
      auto in_source {abs_path.substr(0, source_folder_path.length()) ==
          source_folder_path};

      const auto build_folder {
          FileFinder("", RelativeTo::IBLBM_BUILD_ROOT).GetAbsolutePath()};
      const auto build_folder_path {
          PosixPathFixer::Fix(fs::path(build_folder))};
      auto in_build {abs_path.substr(0, build_folder_path.length()) ==
          build_folder_path};

      if (!(in_source || in_build)) {
        EXCEPTION("Cannot remove location '" << mAbsPath <<
            "' as it is not located within the IBLBM test output folder (" <<
            test_output_path << "), the IBLBM source folder (" <<
            source_folder_path <<") or the IBLBM build folder (" <<
            build_folder_path <<").");
      }
    }
    else {
      EXCEPTION("Cannot remove location '" << mAbsPath <<
          "' as it is not located within the IBLBM test output folder (" <<
          test_output_path << ").");
    }
  }

  if (mAbsPath.find("..") != std::string::npos) {
    EXCEPTION("Cannot remove location '" << mAbsPath <<
        "' as it contains a dangerous path component.");
  }
  if (Exists()) {
    if (!isDangerous) {
      fs::path sig_file(mAbsPath);
      // We need to look for the signature file in the parent folder
      if (IsFile()) sig_file.remove_leaf();
      sig_file /= OutputFileHandler::SIG_FILENAME;
      if (!fs::exists(sig_file)) {
        EXCEPTION("Cannot remove location '" << mAbsPath <<
            "' because the signature file '" <<
            OutputFileHandler::SIG_FILENAME << "' is not present.");
      }
    }
    // Do the removal
    CONVERT_ERROR(RemoveAll(mAbsPath));
  }
}
}  // namespace iblbm
