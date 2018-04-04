#include <string>

#include "UnitTest++/UnitTest++.h"
#include "Exception.hpp"

namespace iblbm
{
SUITE(TestCore)
{
TEST(TestException_GetMessage)
{
  std::ostringstream ostring_stream;
  std::streambuf* p_cerr_streambuf = std::cerr.rdbuf();
  // set ostring_stream stream buffer as the stream buffer associated with
  // cerr
  std::cerr.rdbuf(ostring_stream.rdbuf());
  std::string msg {"This is an exception"};

  try {
    EXCEPTION(msg);
  }
  catch (Exception& exc)
  {
    auto e_msg {exc.GetMessage()};
    auto e_len {e_msg.length()};
    auto len {msg.length()};
    CHECK_EQUAL(msg, e_msg.substr(e_len - len));
  }

  CHECK_THROW(EXCEPTION("Hello. I'm an exception"), Exception);

  // Restore cerr's stream buffer so we can resume normal printing
  std::cerr.rdbuf(p_cerr_streambuf);
}

TEST(TestException_Streaming)
{
  std::ostringstream ostring_stream;
  std::streambuf* p_cerr_streambuf = std::cerr.rdbuf();
  // set ostring_stream stream buffer as the stream buffer associated with
  // cerr
  std::cerr.rdbuf(ostring_stream.rdbuf());

  auto rule {42};
  try {
    EXCEPTION("This is a rule " << rule << " exception");
  }
  catch (const Exception& exc) {
    CHECK_EQUAL("", exc.CheckShortMessage("This is a rule 42 exception"));
  }

  // Restore cerr's stream buffer so we can resume normal printing
  std::cerr.rdbuf(p_cerr_streambuf);
}

TEST(TestException_CheckMethods)
{
  std::ostringstream ostring_stream;
  std::streambuf* p_cerr_streambuf = std::cerr.rdbuf();
  // set ostring_stream stream buffer as the stream buffer associated with
  // cerr
  std::cerr.rdbuf(ostring_stream.rdbuf());

  std::string msg {"This is our message"};

  // If another process threw an exception, this is what this process will
  // actually throw
  std::string parallel_msg {"Another process threw an exception; bailing "
      "out."};

  try {
    EXCEPTION(msg);
  }
  catch (const Exception& exc) {
    CHECK_EQUAL("", exc.CheckShortMessage(msg));
    CHECK(exc.CheckShortMessage("This is not our message") != "");
  }

  CHECK_THROW(EXCEPTION(msg), Exception);
  CHECK_THROW(EXCEPTION(parallel_msg), Exception);
  try {
    EXCEPTION(parallel_msg);
  }
  catch (const Exception& exc) {
    CHECK_EQUAL("", exc.CheckShortMessage(msg));
    // Of course, in this case we don't know what the real message would have
    // been...
    CHECK_EQUAL("", exc.CheckShortMessage("This is not our message"));
  }

  // Now check for messages containing a string
  try {
    EXCEPTION(msg + " extra bit");
  }
  catch (const Exception& exc) {
    CHECK_EQUAL("", exc.CheckShortMessageContains(msg));
    CHECK(exc.CheckShortMessageContains("This is not our message") != "");
  }

  try {
    EXCEPTION(parallel_msg);
  }
  catch (const Exception& exc) {
    CHECK_EQUAL("", exc.CheckShortMessageContains(msg));
    // Of course, in this case we don't know what the real message would have
    // been...
    CHECK_EQUAL("", exc.CheckShortMessageContains("This is not our message"));
  }

  try {
    EXCEPT_IF_NOT(false);
  }
  catch (const Exception& exc) {
    CHECK_EQUAL("", exc.CheckShortMessage("Assertion tripped: false"));
  }

  try {
    EXCEPT_IF_NOT(true);
  }
  catch (...) {
    // This should be be reached
    CHECK(false);
  }

  // Restore cerr's stream buffer so we can resume normal printing
  std::cerr.rdbuf(p_cerr_streambuf);
}
}
}  // namespace iblbm
