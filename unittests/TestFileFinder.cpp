#include "UnitTest++/UnitTest++.h"
#include "Exception.hpp"
#include "FileFinder.hpp"
#include "IblbmBuildRoot.hpp"
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
    std::string filename {"src/io/FileFinder.hpp"};
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
//    std::cout << file_finder2.GetAbsolutePath() << std::endl;
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
    FileFinder parent("src/io", RelativeTo::IBLBM_SOURCE_ROOT);
    CHECK_EQUAL(parent.GetAbsolutePath(),
        file_finder.GetParent().GetAbsolutePath());
    CHECK_EQUAL("io", parent.GetLeafName());
    CHECK_EQUAL("io", parent.GetLeafNameNoExtension());
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
        fs::path(FileFinder("src/io/FileFinder.hpp").GetAbsolutePath()),
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
}
}
}  // namespace iblbm
