#include "UnitTest++/UnitTest++.h"
#include "Exception.hpp"
#include "FileFinder.hpp"
#include "IblbmBuildRoot.hpp"
#include "MpiManager.hpp"
#include "OutputFileHandler.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
SUITE(TestIo)
{
TEST(TestFileFinder_Constructor_Default)
{
  FileFinder unset;
  CHECK(!unset.IsPathSet());
  unset.SetPath("", RelativeTo::IBLBM_SOURCE_ROOT);
  CHECK(unset.IsPathSet());
}

TEST(TestFileFinder_Constructor_RelativeTo)
{
  {
    // Can we find our own source file?
    std::string filename {"io/src/FileFinder.hpp"};
    FileFinder file_finder(filename, RelativeTo::IBLBM_SOURCE_ROOT);
    CHECK(file_finder.Exists());
    CHECK(file_finder.IsFile());
    CHECK(!file_finder.IsDir());
    CHECK(!file_finder.IsEmpty());
    CHECK(file_finder.IsPathSet());

    // Check the path is as expected
    std::string abs_path {IblbmSourceRootDir() + filename};
    CHECK_EQUAL(abs_path, file_finder.GetAbsolutePath());

    // CWD should be the Chaste source root
    FileFinder file_finder2(filename, RelativeTo::CWD);
    CHECK(file_finder2.Exists());
    CHECK(file_finder2.IsFile());
    CHECK(!file_finder2.IsDir());

    // Check the path is as expected
    CHECK(fs::equivalent(fs::path(file_finder2.GetAbsolutePath()),
        fs::path(abs_path)));

    // filename is relative
    file_finder2.SetPath(filename, RelativeTo::ABSOLUTE_OR_CWD);
    CHECK(file_finder2.Exists());
    CHECK(file_finder2.IsFile());
    CHECK(!file_finder2.IsDir());

    // Check the path is as expected
    CHECK(fs::equivalent(fs::path(file_finder2.GetAbsolutePath()),
        fs::path(abs_path)));

    // Check we can extract the leaf name
    CHECK_EQUAL("FileFinder.hpp", file_finder.GetLeafName());
    CHECK_EQUAL("FileFinder", file_finder.GetLeafNameNoExtension());
    CHECK_EQUAL(".hpp", file_finder.GetExtension());

    // And the parent folder name
    FileFinder parent("io/src", RelativeTo::IBLBM_SOURCE_ROOT);
    CHECK_EQUAL(parent.GetAbsolutePath(),
        file_finder.GetParent().GetAbsolutePath());
    CHECK_EQUAL("src", parent.GetLeafName());
    CHECK_EQUAL("src", parent.GetLeafNameNoExtension());
    CHECK_EQUAL("", parent.GetExtension());

    // Check we can construct from a Boost path or a string
    CHECK(fs::equivalent(
        fs::path(FileFinder(fs::path(filename)).GetAbsolutePath()),
        fs::path(abs_path)));
    CHECK(fs::equivalent(
        fs::path(FileFinder(filename).GetAbsolutePath()),
        fs::path(abs_path)));
    CHECK(fs::equivalent(
        fs::path(FileFinder(abs_path).GetAbsolutePath()),
        fs::path(abs_path)));
    CHECK(fs::equivalent(
        fs::path(FileFinder("io/src/FileFinder.hpp").GetAbsolutePath()),
        fs::path(abs_path)));
  }

  {
    // Now check a file in the output directory
    std::string dir_name {"TestFileFinder"};
    OutputFileHandler handler(dir_name);
    std::string file_name {"TestFile"};
    FileFinder file_finder(dir_name + "/" + file_name,
        RelativeTo::IBLBM_TEST_OUTPUT);
    CHECK(!file_finder.Exists());
    CHECK(!file_finder.IsFile());
    CHECK(!file_finder.IsDir());

    // Check the path is as expected
    auto abs_path {handler.GetOutputDirectoryFullPath() + file_name};
    CHECK_EQUAL(abs_path, file_finder.GetAbsolutePath());

    // Check finding a sibling and testing for emptiness fail
    // This should try to write files to /, which isn't allowed (we hope!)
    CHECK_THROW_CONTAINS(file_finder.IsEmpty(), "The path '" + abs_path +
        "' does not exist.");
    CHECK_THROW_CONTAINS(FileFinder("sibling", file_finder),
        "Reference path '" + abs_path + "' does not exist.");

    // So that we don't create the file before other processes can check that
    // it doesn't exist
    MpiManager::Instance().Barrier("TestFileFinder_Constructor_RelativeTo-0");

    // Create the file
    auto fp {handler.OpenOutputFile(file_name)};
    fp->close();
    CHECK(file_finder.Exists());
    CHECK(file_finder.IsEmpty());

    // Finding a sibling is now ok
    FileFinder sibling("sibling", file_finder);
    CHECK_EQUAL(sibling.GetAbsolutePath(),
        handler.GetOutputDirectoryFullPath() + "sibling");
    CHECK(sibling.IsPathSet());

    // Check when providing an absolute path
    FileFinder file_finder2(abs_path, RelativeTo::ABSOLUTE);
    CHECK(file_finder2.Exists());
    CHECK_EQUAL(file_finder2.GetAbsolutePath(), abs_path);

    FileFinder file_finder3(abs_path, RelativeTo::ABSOLUTE_OR_CWD);
    CHECK(file_finder3.Exists());
    CHECK_EQUAL(file_finder3.GetAbsolutePath(), abs_path);
  }

  {
    // Can we find our build directory?
    std::string filename {""};
    FileFinder file_finder(filename, RelativeTo::IBLBM_BUILD_ROOT);
    CHECK(file_finder.Exists());
    CHECK(!file_finder.IsFile());
    CHECK(file_finder.IsDir());
    CHECK(!file_finder.IsEmpty());
    CHECK(file_finder.IsPathSet());
  }
}

TEST(TestFileFinder_Remove)
{
  // We shouldn't be able to remove unsafe files, if possible
  FileFinder bad1a("core/src", RelativeTo::IBLBM_SOURCE_ROOT);
  CHECK_THROW_CONTAINS(bad1a.Remove(), "is not located within the IBLBM "
      "test output folder");
  FileFinder bad1b("/", RelativeTo::ABSOLUTE);
  CHECK_THROW_CONTAINS(bad1b.Remove(), "is not located within the IBLBM "
      "test output folder");
  FileFinder bad2("../..", RelativeTo::IBLBM_TEST_OUTPUT);
  CHECK_THROW_CONTAINS(bad2.Remove(), "contains a dangerous path "
      "component.");

  // We can delete individual files
  OutputFileHandler handler("TestFileFinder/TestRemove");
  handler.OpenOutputFile("delete_me");
  FileFinder file {handler.FindFile("delete_me")};
  CHECK(file.Exists());
  file.Remove();
  CHECK(!file.Exists());

  // We can recursively delete folders
  FileFinder dir("TestFileFinder", RelativeTo::IBLBM_TEST_OUTPUT);
  FileFinder subdir("TestFileFinder/TestRemove",
      RelativeTo::IBLBM_TEST_OUTPUT);
  CHECK(subdir.Exists());
  dir.Remove();
  CHECK(!subdir.Exists());
  CHECK(!dir.Exists());

  // We can only delete (content in) folders created by an OutputFileHandler
  // unless we force it
  fs::create_directory(dir.GetAbsolutePath());
  CHECK(dir.IsDir());
  CHECK_THROW_CONTAINS(dir.Remove(), "because the signature file "
      "'.iblbm_deletable_folder' is not present.");
  CHECK(dir.IsDir());
  file.SetPath("file", dir);
  { std::ofstream(file.GetAbsolutePath().c_str()); }
  CHECK(file.IsFile());
  CHECK_THROW_CONTAINS(file.Remove(), "because the signature file "
      "'.iblbm_deletable_folder' is not present.");
  CHECK(file.IsFile());
  file.DangerousRemove();
  CHECK(!file.Exists());
  dir.DangerousRemove();
  CHECK(!dir.Exists());

  FileFinder obscure_file("/SomeObscureFile.obscure", RelativeTo::ABSOLUTE);
  CHECK_THROW_CONTAINS(obscure_file.Remove(), "as it is not located "
      "within the IBLBM test output folder");
  CHECK_THROW_CONTAINS(obscure_file.DangerousRemove(), ", the IBLBM source "
      "folder");
}
}
}  // namespace iblbm
