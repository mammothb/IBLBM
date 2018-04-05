#include <string>

#include "UnitTest++/UnitTest++.h"
#include "Exception.hpp"
#include "FileFinder.hpp"
#include "IblbmSyscall.hpp"
#include "MpiManager.hpp"
#include "OutputFileHandler.hpp"
#include "UnitTestCustomUtilities.hpp"

namespace iblbm
{
SUITE(TestIo)
{
TEST(TestOutputFileHandler_GetIblbmTestOutputDirectory)
{
  // Test that IBLBM_TEST_OUTPUT always has a trailing slash even before
  // a class object is instantiated
  const auto iblbm_test_output {
      OutputFileHandler::GetIblbmTestOutputDirectory()};
  CHECK_EQUAL('/',  *iblbm_test_output.rbegin());

  // Make a handler that points straight to the IBLBM_TEST_OUTPUT directory.
  OutputFileHandler handler {""};
  const auto handler_path {handler.GetOutputDirectoryFullPath()};
  CHECK(handler_path.length() > 0);
  CHECK_EQUAL(handler.GetIblbmTestOutputDirectory(), handler_path);
  CHECK_EQUAL("", handler.GetRelativePath());

  // Test that IBLBM_TEST_OUTPUT always has a trailing slash
  CHECK_EQUAL('/', *handler_path.rbegin());

  // Make a handler that points to a sub-directory.
  std::string dir {"testhandler"};
  OutputFileHandler handler2(dir);
  auto full_dir {handler2.GetOutputDirectoryFullPath()};
  CHECK_EQUAL(full_dir.substr(full_dir.length() - dir.length() - 1),
      dir + '/');
  CHECK_EQUAL(full_dir.substr(0, full_dir.length() - dir.length() - 1),
      handler_path);
  CHECK_EQUAL(handler2.GetRelativePath(), dir);

  // We can also create handlers from a FileFinder (provided it points to a
  // location in IBLBM_TEST_OUTPUT)
  OutputFileHandler handler3(handler.FindFile("testhandler2"));
  full_dir = handler3.GetOutputDirectoryFullPath();
  CHECK_EQUAL(full_dir.substr(full_dir.length() - dir.length() - 2),
      dir + "2/");
  CHECK_EQUAL(full_dir.substr(0, full_dir.length() - dir.length() - 2),
      handler_path);
  CHECK_EQUAL(handler3.GetRelativePath(), "testhandler2");

  // Check that all three handlers can create files
  out_stream p_fstream;
  p_fstream = handler.OpenOutputFile("test_file", std::ios::out);
  CHECK(FileFinder(handler_path + "test_file").Exists());

  p_fstream = handler.OpenOutputFile("test_file2");
  CHECK(FileFinder(handler_path + "test_file2").Exists());

  p_fstream = handler2.OpenOutputFile("test_file");
  CHECK(FileFinder(handler2.GetOutputDirectoryFullPath() +
      "test_file").Exists());

  p_fstream = handler2.OpenOutputFile("test_", 34, ".txt");
  CHECK(FileFinder(handler2.GetOutputDirectoryFullPath() +
      "test_34.txt").Exists());

  p_fstream = handler3.OpenOutputFile("test_file");
  CHECK(FileFinder(handler3.GetOutputDirectoryFullPath() +
      "test_file").Exists());

  // This should try to write files to /, which isn't allowed (we hope!)
  CHECK_THROW_CONTAINS(OutputFileHandler bad_handler(
      "../../../../../../../../../../../../../../../", false),
      "due to it potentially being above, and cleaning, IBLBM_TEST_OUTPUT.");
  CHECK_THROW_CONTAINS(OutputFileHandler bad_handler( "/", false),
      "The constructor argument to OutputFileHandler must be a relative "
      "path");
  CHECK_THROW_CONTAINS(OutputFileHandler bad_handler(FileFinder("/"), false),
      "The location provided to OutputFileHandler must be inside "
      "IBLBM_TEST_OUTPUT");

  // Check the CopyFileTo method
  FileFinder source_file("unittests/TestOutputFileHandler.cpp",
      RelativeTo::IBLBM_SOURCE_ROOT);
  CHECK(!handler2.FindFile("TestOutputFileHandler.cpp").Exists());
  MpiManager::Instance().Barrier("TestOutputFileHandler-0");
  FileFinder dest_file = handler2.CopyFileTo(source_file);
  CHECK(dest_file.Exists());
  FileFinder missing_file("unittests/no_file", RelativeTo::IBLBM_SOURCE_ROOT);
  CHECK_THROW_CONTAINS(handler2.CopyFileTo(missing_file),
      "Can only copy single files");
  FileFinder global_dir("core", RelativeTo::IBLBM_SOURCE_ROOT);
  CHECK_THROW_CONTAINS(handler2.CopyFileTo(global_dir),
      "Can only copy single files");

  // We don't want other people using IBLBM_TEST_OUTPUT whilst we are messing
  // with it!
  MpiManager::Instance().Barrier("TestOutputFileHandler-1");

  // Test that the environment variable actually influences the location of
  // files
  {
    setenv("IBLBM_TEST_OUTPUT", "", 1/*Overwrite*/);
    // Check this folder is not present
    FileFinder test_folder("testoutput/whatever",
        RelativeTo::IBLBM_SOURCE_ROOT);
    CHECK(!test_folder.Exists());
    MpiManager::Instance().Barrier("TestOutputFileHandler-2");

    // Make a folder and erase it - NB only master can erase files and check
    // it is successful!
    OutputFileHandler handler4("whatever");
    CHECK(test_folder.Exists());
    MpiManager::Instance().Barrier("TestOutputFileHandler-2a");
    if (MpiManager::Instance().AmMaster()) {
      test_folder.Remove();
      // If we've not written anything else to the testoutput folder,
      // remove that too rather than leaving an empty folder lying around
      // in the source tree!
      FileFinder output_root("", RelativeTo::IBLBM_TEST_OUTPUT);
      if (output_root.IsEmpty()) output_root.DangerousRemove();
    }
    MpiManager::Instance().Barrier("TestOutputFileHandler-2b");
  }

  {
    setenv("IBLBM_TEST_OUTPUT", "non__existent__test__output",
        1/*Overwrite*/);
    // Test that IBLBM_TEST_OUTPUT always has a trailing slash even before
    // a class object is instantiated and when the directory does not exist
    const auto nonexistent_test_path {
        OutputFileHandler::GetIblbmTestOutputDirectory()};
    CHECK_EQUAL('/', *nonexistent_test_path.rbegin());
  }

  {
    // Check this folder is not present
    std::string test_folder {"somewhere_without_trailing_forward_slash"};
    CHECK(!FileFinder(test_folder, RelativeTo::CWD).Exists());
    MpiManager::Instance().Barrier("TestOutputFileHandler-3");

    setenv("IBLBM_TEST_OUTPUT", test_folder.c_str(), 1/*Overwrite*/);

    // Make a folder using a FileFinder, for coverage of the case where the
    // root output folder doesn't exist
    FileFinder sub_folder("test_folder", RelativeTo::IBLBM_TEST_OUTPUT);
    CHECK(!sub_folder.Exists());
    MpiManager::Instance().Barrier("TestOutputFileHandler-3a");
    OutputFileHandler creating_handler(sub_folder);
    CHECK(sub_folder.Exists());

    // Make a folder
    OutputFileHandler handler5("whatever");
    CHECK(FileFinder(test_folder, RelativeTo::CWD).Exists());
    MpiManager::Instance().Barrier("TestOutputFileHandler-3b");

    // Erase it
    if (MpiManager::Instance().AmMaster())
        FileFinder(test_folder).DangerousRemove();
    MpiManager::Instance().Barrier("TestOutputFileHandler-3c");
  }

  // Reset the location of IBLBM_TEST_OUTPUT
  setenv("IBLBM_TEST_OUTPUT", iblbm_test_output.c_str(), 1/*Overwrite*/);

  // We don't want other people using IBLBM_TEST_OUTPUT while we are messing
  //with it!
  MpiManager::Instance().Barrier("TestOutputFileHandler-4");

  // Coverage of the case where we can't open a file for writing
  OutputFileHandler handler6("no_write_access");
  if (MpiManager::Instance().AmMaster()) {
    auto dir_path {handler6.GetOutputDirectoryFullPath()};
#ifndef _MSC_VER
    // This test can never pass on modern Windows OS!
    // See: http://support.microsoft.com/kb/326549
    // You can't change DIRECTORY attributes
    chmod(dir_path.c_str(), IBLBM_READONLY);
    CHECK_THROW_CONTAINS(p_fstream = handler6.OpenOutputFile("test_file"),
        "Could not open file");
#endif
    chmod(dir_path.c_str(), IBLBM_READ_WRITE_EXECUTE);
    fs::remove(dir_path + ".iblbm_deletable_folder");
    fs::remove(dir_path);
  }

  // Check behaviour of FindFile("")
  auto handler_self {handler.FindFile("")};
  CHECK_EQUAL(handler_self.GetAbsolutePath(),
      handler.GetOutputDirectoryFullPath());
}

TEST(TestWeCanOnlyDeleteFoldersWeHaveMadeOurselves)
{
  std::string test_folder {"cannot_delete_me"};
  if (MpiManager::Instance().AmMaster()) {
    ABORT_IF_THROWS(fs::create_directories(
        OutputFileHandler::GetIblbmTestOutputDirectory() + test_folder));
  }
  // Wait until directory has been created, and check it exists
  MpiManager::Instance().Barrier(
      "TestWeCanOnlyDeleteFoldersWeHaveMadeOurselves-1");
  FileFinder cannot_delete(test_folder, RelativeTo::IBLBM_TEST_OUTPUT);
  CHECK(cannot_delete.IsDir());

  // Try to use it as an output folder
  CHECK_THROW_CONTAINS(OutputFileHandler bad_handler(test_folder),
      "because signature file \".iblbm_deletable_folder\" is not present");
  // Tidy up
  if (MpiManager::Instance().AmMaster()) {
    CHECK(cannot_delete.Exists());
    cannot_delete.DangerousRemove();
    CHECK(!cannot_delete.Exists());
  }

  // Now create a folder the proper way
  test_folder = "can_delete_me";
  OutputFileHandler handler(test_folder);
  out_stream p_fstream = handler.OpenOutputFile("test_file");
  p_fstream->close();  // Windows does not like deleting open files

  // Test file is present
  FileFinder test_file = handler.FindFile("test_file");
  CHECK(test_file.Exists());
  MpiManager::Instance().Barrier(
      "TestWeCanOnlyDeleteFoldersWeHaveMadeOurselves-2");

  OutputFileHandler handler2(test_folder, /*cleanOutputDirectory=*/false);

  // Test file is still present
  CHECK(test_file.Exists());
  MpiManager::Instance().Barrier(
      "TestWeCanOnlyDeleteFoldersWeHaveMadeOurselves-3");

  OutputFileHandler handler3(test_folder, /*cleanOutputDirectory=*/true);

  // Test file is deleted
  CHECK(!test_file.Exists());
  MpiManager::Instance().Barrier(
      "TestWeCanOnlyDeleteFoldersWeHaveMadeOurselves-4");

  // Check we can delete the test_folder too
  if (MpiManager::Instance().AmMaster()) {
    FileFinder folder = handler.FindFile("");
    CHECK(folder.Exists());
    folder.Remove();
    CHECK(!folder.Exists());
  }

  // Test we can make a directory of folders and delete them all
  OutputFileHandler handler4("what_about_me/and_me/and_me/and_da_da_da",
      true);

  // Check we have made a subdirectory
  FileFinder sub_folder("what_about_me/and_me",
      RelativeTo::IBLBM_TEST_OUTPUT);
  CHECK(sub_folder.IsDir());
  MpiManager::Instance().Barrier(
      "TestWeCanOnlyDeleteFoldersWeHaveMadeOurselves-5");

  OutputFileHandler handler5("what_about_me", true);

  // Check we have wiped the sub-directories
  CHECK(!sub_folder.Exists());
  MpiManager::Instance().Barrier(
      "TestWeCanOnlyDeleteFoldersWeHaveMadeOurselves-6");

  // Check we can delete the main directory too
  if (MpiManager::Instance().AmMaster()) {
    FileFinder folder = handler5.FindFile("");
    CHECK(folder.Exists());
    folder.Remove();
    CHECK(!folder.Exists());
  }
}
}
}  // namespace iblbm
