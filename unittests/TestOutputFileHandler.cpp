#include <string>

#include "UnitTest++/UnitTest++.h"
#include "Exception.hpp"
#include "FileFinder.hpp"
#include "IblbmSyscall.hpp"
#include "MpiManager.hpp"
#include "OutputFileHandler.hpp"

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
  out_stream p_file_stream;
  p_file_stream = handler.OpenOutputFile("test_file", std::ios::out);
  CHECK(FileFinder(handler_path + "test_file").Exists());

  p_file_stream = handler.OpenOutputFile("test_file2");
  CHECK(FileFinder(handler_path + "test_file2").Exists());

  p_file_stream = handler2.OpenOutputFile("test_file");
  CHECK(FileFinder(handler2.GetOutputDirectoryFullPath() +
      "test_file").Exists());

  p_file_stream = handler2.OpenOutputFile("test_", 34, ".txt");
  CHECK(FileFinder(handler2.GetOutputDirectoryFullPath() +
      "test_34.txt").Exists());

  p_file_stream = handler3.OpenOutputFile("test_file");
  CHECK(FileFinder(handler3.GetOutputDirectoryFullPath() +
      "test_file").Exists());

  std::ostringstream ostring_stream;
  std::streambuf* p_cerr_streambuf = std::cerr.rdbuf();
  // set ostring_stream stream buffer as the stream buffer associated with
  // cerr
  std::cerr.rdbuf(ostring_stream.rdbuf());
  // This should try to write files to /, which isn't allowed (we hope!)
  try {
    OutputFileHandler bad_handler(
        "../../../../../../../../../../../../../../../", false);
  }
  catch (const Exception& exc) {
    CHECK_EQUAL("", exc.CheckShortMessageContains("due to it potentially "
        "being above, and cleaning, IBLBM_TEST_OUTPUT."));
  }
  try {
    OutputFileHandler bad_handler( "/", false);
  }
  catch (const Exception& exc) {
    CHECK_EQUAL("", exc.CheckShortMessageContains("The constructor argument "
        "to OutputFileHandler must be a relative path"));
  }
  try {
    OutputFileHandler bad_handler(FileFinder("/"), false);
  }
  catch (const Exception& exc) {
    CHECK_EQUAL("", exc.CheckShortMessageContains("The location provided to "
        "OutputFileHandler must be inside IBLBM_TEST_OUTPUT"));
  }

  // Check the CopyFileTo method
  FileFinder source_file("unittests/TestOutputFileHandler.cpp",
      RelativeTo::IBLBM_SOURCE_ROOT);
  CHECK(!handler2.FindFile("TestOutputFileHandler.cpp").Exists());
  MpiManager::Instance().Barrier("TestOutputFileHandler-0");
  FileFinder dest_file = handler2.CopyFileTo(source_file);
  CHECK(dest_file.Exists());
  FileFinder missing_file("unittests/no_file", RelativeTo::IBLBM_SOURCE_ROOT);
  try {
    handler2.CopyFileTo(missing_file);
  }
  catch (const Exception& exc) {
    CHECK_EQUAL("", exc.CheckShortMessageContains(
        "Can only copy single files"));
  }
  FileFinder global_dir("core", RelativeTo::IBLBM_SOURCE_ROOT);
  try {
    handler2.CopyFileTo(global_dir);
  }
  catch (const Exception& exc) {
    CHECK_EQUAL("", exc.CheckShortMessageContains(
        "Can only copy single files"));
  }

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
    try {
      p_file_stream = handler6.OpenOutputFile("test_file");
    }
    catch (const Exception& exc) {
      CHECK_EQUAL("", exc.CheckShortMessageContains(
          "Could not open file"));
    }
#endif
    chmod(dir_path.c_str(), IBLBM_READ_WRITE_EXECUTE);
    fs::remove(dir_path + ".iblbm_deletable_folder");
    fs::remove(dir_path);
  }
  // Restore cerr's stream buffer so we can resume normal printing
  std::cerr.rdbuf(p_cerr_streambuf);

  // Check behaviour of FindFile("")
  auto handler_self {handler.FindFile("")};
  CHECK_EQUAL(handler_self.GetAbsolutePath(),
      handler.GetOutputDirectoryFullPath());
}
}
}  // namespace iblbm
