#include "Exception.hpp"

#include "IblbmBuildRoot.hpp"
#include "MpiManager.hpp"
#include "PosixPathFixer.hpp"

namespace iblbm
{
Exception::Exception(
    const std::string& rMessage
  , const std::string& rFilename
  , std::size_t lineNumber)
{
  SetMessage(rMessage, rFilename, lineNumber);
}

std::string Exception::GetMessage() const
{
  return mMessage;
}

std::string Exception::CheckShortMessage(std::string expected) const
{
  std::string error;
  if (mShortMessage != expected &&
      mShortMessage != "Another process threw an exception; bailing out.") {
    error = "Incorrect exception message thrown: expected (" + expected +
        "); got (" + mShortMessage + ").";
  }
  return error;
}

std::string Exception::CheckShortMessageContains(std::string expected) const
{
  std::string error;
  if (mShortMessage.find(expected) == std::string::npos &&
      mShortMessage != "Another process threw an exception; bailing out.") {
    error = "Incorrect exception message thrown: expected it to contain (" +
        expected + "); got (" + mShortMessage + ").";
  }
  return error;
}

void Exception::Terminate(
    const std::string& rMessage
  , const std::string& rFilename
  , std::size_t lineNumber)
{
  std::stringstream error_message;

  error_message << "\nIBLBM termination: " << rFilename << ":" <<
      lineNumber << ": " << rMessage << "\n";
  std::cerr << error_message.str();

  // Check if we're running in parallel.
  if (MpiManager::Instance().IsOk()) {
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  else {
    exit(EXIT_FAILURE);
  }
}

void Exception::SetMessage(
    const std::string& rMessage
  , const std::string& rFilename
  , std::size_t lineNumber)
{
  // Strip off source root dir if exists
  std::string filename(rFilename);
  const size_t root_dir_length = std::char_traits<char>::length(IblbmSourceRootDir());
  if (filename.compare(0,root_dir_length,IblbmSourceRootDir()) == 0)
  {
    filename.replace(0,root_dir_length, "./");
  }

  std::string posix_filename(PosixPathFixer::Fix(fs::path(filename)));
  mShortMessage = rMessage;
  std::stringstream line_number_stream;
  line_number_stream << lineNumber;
  mMessage = "\nChaste error: " + posix_filename + ":" +
      line_number_stream.str() + ": " + mShortMessage;

  std::cerr << mMessage;
}
}  // namespace iblbm
