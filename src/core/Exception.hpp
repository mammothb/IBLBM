#ifndef SRC_CORE_EXCEPTION_HPP_
#define SRC_CORE_EXCEPTION_HPP_

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace iblbm
{
/**
 * Exception class.
 * All exceptions thrown by this code are currently instances of this class.
 *
 */
class Exception
{
 public:
  /**
   * Construct an exception with a message string.
   *
   * \param rMessage the message
   * \param rFilename which source file threw the exception
   * \param lineNumber which line number of the source file threw the
   *        exception
   */
  Exception(
      const std::string& rMessage
    , const std::string& rFilename
    , std::size_t lineNumber);

  /**
   * Get the message associated with the exception with file and line number
   *
   * \return The message set when the exception was thrown including file and
   * line number information
   */
  std::string GetMessage() const;


  /**
   * Helper method for checking we have the right exception.
   *
   * \param expected  the expected value of mShortMessage
   *
   * \return an empty string when the expected message matches.
   * Checks that mShortMessage matches that given, and a suitable error
   * message string if not.
   */
  std::string CheckShortMessage(std::string expected) const;

  /**
   * Helper method for checking we have the right exception.
   *
   * \param expected  some expected substring of #mShortMessage
   *
   * \return an empty string when the message contains the expected string.
   * Checks that #mShortMessage contains the given string, and
   * returns a suitable error message string if not.
   */
  std::string CheckShortMessageContains(std::string expected) const;

  /**
   * Level 4 error (Termination). Execution cannot continue from this point
   * and hence should be terminated (even when running with NDEBUG or in
   * parallel).
   *
   * \param rMessage An error message to appear on the screen
   * \param rFilename which source file produced the termination error
   * \param lineNumber which line number of the source file produced the
   *        termination error
   */
  static void Terminate(
      const std::string& rMessage
    , const std::string& rFilename
    , std::size_t lineNumber);

 protected:
  /**
   * Allow subclasses to reset the exception message after construction of
   * the base class, if desired.
   *
   * \param rMessage the message
   * \param rFilename which source file threw the exception
   * \param lineNumber which line number of the source file threw the
   *        exception
   */
  void SetMessage(
      const std::string& rMessage
    , const std::string& rFilename
    , std::size_t lineNumber);

 private:
  /** Full exception message - includes file and line number. */
  std::string mMessage;
  /** Short exception message - just text of the exception. */
  std::string mShortMessage;
};

/**
 * Convenience macro for throwing an exception, in order to add file and line
 * info.
 *
 * \param message the error message to use, as a streamed expression
 */
#define EXCEPTION(message)                                  \
  do {                                                      \
    std::stringstream msg_stream;                           \
    msg_stream << message;                                  \
    throw Exception(msg_stream.str(), __FILE__, __LINE__);  \
  } while (false)

#include <boost/preprocessor/stringize.hpp>

/**
 * Convenience macro for changing an assert into an exception - has the same
 * calling semantics, but throws.
 *
 * \param test the test that must always be true.
 */
#define EXCEPT_IF_NOT(test)                                                 \
    if (!(test)) EXCEPTION("Assertion tripped: " BOOST_PP_STRINGIZE(test))

/**
 * Terminate execution safely, even when running in parallel. Use for level 4
 * errors: execution cannot continue from this point and hence should be
 * terminated (even when running with NDEBUG).
 *
 * \param message explanatory message
 */
#define TERMINATE(message)                                       \
  do {                                                           \
    std::stringstream msg_stream;                                \
    msg_stream << message;                                       \
    Exception::Terminate(msg_stream.str(), __FILE__, __LINE__);  \
  } while (false)

/**
 * Use for control paths that will never be executed, just to make sure they
 * aren't.
 *
 * The exit statement at the end of NEVER_REACHED is not really needed but
 * prevents g++ from complaining about uninitialized variables when you have
 * code that looks like:
 *
 * \code
 * switch (relativeTo) {
 * case RelativeTo::IBLBM_SOURCE_ROOT:
 *   mAbsPath = IblbmSourceRootDir() + rRelativePath;
 *   break;
 * case RelativeTo::IBLBM_BUILD_ROOT:
 *   mAbsPath = IblbmBuildRootDir() + rRelativePath;
 *   break;
 * case RelativeTo::IBLBM_TEST_OUTPUT:
 *   mAbsPath = OutputFileHandler::GetIblbmTestOutputDirectory() +
 *       rRelativePath;
 *   break;
 * case RelativeTo::CWD:
 *   mAbsPath = GetCurrentWorkingDirectory() + "/" + rRelativePath;
 *   break;
 * case RelativeTo::ABSOLUTE:
 *   mAbsPath = rRelativePath;
 *   break;
 * case RelativeTo::ABSOLUTE_OR_CWD:
 *   if (FileFinder::IsAbsolutePath(rRelativePath)) {
 *     mAbsPath = rRelativePath;
 *   }
 *   else {
 *     mAbsPath = GetCurrentWorkingDirectory() + "/" + rRelativePath;
 *   }
 *   break;
 * default:
 *   NEVER_REACHED;
 * }
 * \endcode
 *
 * relative_to is considered potentially uninitialized in the default branch
 * unless the compiler finds a exit, assert or throw statement.
 */
#define NEVER_REACHED                                      \
  TERMINATE("Should have been impossible to reach here");  \
  exit(EXIT_FAILURE)

/**
 * Convenience function to convert an exception thrown by a single process
 * into termination of the entire program.
 *
 * \param block the block of code to execute
 */
#define ABORT_IF_THROWS(block)                  \
  try {                                         \
    block;                                      \
  }                                             \
  catch (const Exception& exc) {                \
    TERMINATE(exc.GetMessage());                \
  }                                             \
  catch (const std::exception& exc) {           \
    TERMINATE(exc.what());                      \
  }                                             \
  catch (...) {                                 \
    TERMINATE("Unexpected exception thrown.");  \
  }

// Unused parameter macro
#define IBLBM_UNUSED(x) (void)(x)

// Macros to help with debugging
#ifdef IBLBM_DEBUG

#define IBLBM_ASSERT(COND, MSG)                  \
  if (!(COND)) std::cerr << (MSG) << std::endl;  \
  assert(COND);

#define IBLBM_PRECONDITION(COND) assert(COND);
#define IBLBM_POSTCONDITION(COND) assert(COND);
#define IBLBM_STATECHECK(x, y) assert((x) == (y));

#else

#define IBLBM_ASSERT(COND, MSG)
#define IBLBM_PRECONDITION(COND)
#define IBLBM_POSTCONDITION(COND)
#define IBLBM_STATECHECK(x, y)

#endif  // IBLBM_DEBUG

}  // namespace iblbm

#endif  // SRC_CORE_EXCEPTION_HPP_
