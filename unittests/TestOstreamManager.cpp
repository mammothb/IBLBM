#include <iostream>

#include "UnitTest++/UnitTest++.h"
#include "MpiManager.hpp"
#include "OstreamManager.hpp"

namespace iblbm
{
class TestOstreamManagerBuffer
{
 public:
  std::ostream* pGetOutput(const OstreamManagerBuffer& rBuffer)
  {
    return rBuffer.mpOutput;
  }

  std::string GetName(const OstreamManagerBuffer& rBuffer)
  {
    return rBuffer.mName;
  }

  bool GetIsMultiOutput()
  {
    return OstreamManagerBuffer::msIsMultiOutput;
  }
};

SUITE(TestIo)
{
// Test values
static const std::string g_class_name = "Class Name";
static const std::string g_buffer_string = "Buffer string";

TEST(TestOstreamManagerBuffer_Constructor_Default)
{
  TestOstreamManagerBuffer tester;
  OstreamManagerBuffer buffer;

  CHECK(tester.pGetOutput(buffer) == nullptr);
  CHECK_EQUAL("", tester.GetName(buffer));
}

TEST(TestOstreamManagerBuffer_Constructor_UserDefined)
{
  TestOstreamManagerBuffer tester;
  OstreamManagerBuffer buffer(std::cout, g_class_name);

  CHECK(tester.pGetOutput(buffer) == &std::cout);
  CHECK_EQUAL(g_class_name, tester.GetName(buffer));
}

TEST(TestOstreamManagerBuffer_CopyConstructor)
{
  TestOstreamManagerBuffer tester;
  OstreamManagerBuffer buffer(std::cout, g_class_name);
  buffer.str(g_buffer_string);
  OstreamManagerBuffer buffer_2(buffer);

  CHECK(tester.pGetOutput(buffer_2) == &std::cout);
  CHECK_EQUAL(g_class_name, tester.GetName(buffer_2));
  CHECK_EQUAL(g_buffer_string, buffer.str());
  CHECK_EQUAL("", buffer_2.str());
}

TEST(TestOstreamManagerBuffer_CopyAssignment)
{
  TestOstreamManagerBuffer tester;
  OstreamManagerBuffer buffer(std::cout, g_class_name);
  buffer.str(g_buffer_string);
  auto buffer_2 = buffer;

  CHECK(tester.pGetOutput(buffer_2) == &std::cout);
  CHECK_EQUAL(g_class_name, tester.GetName(buffer_2));
  CHECK_EQUAL(g_buffer_string, buffer.str());
  CHECK_EQUAL("", buffer_2.str());
}

TEST(TestOstreamManagerBuffer_SetIsMultiOutput)
{
  TestOstreamManagerBuffer tester;
  OstreamManagerBuffer buffer;

  buffer.SetIsMultiOutput();
  CHECK(tester.GetIsMultiOutput() == true);

  buffer.SetIsMultiOutput(false);
  CHECK(tester.GetIsMultiOutput() == false);
}

TEST(TestOstreamManagerBuffer_sync)
{
  if (MpiManager::Instance().AmMaster()) {
    // output from buffer gets written to this
    std::ostringstream ostring_stream;
    OstreamManagerBuffer buffer(ostring_stream, g_class_name);

    buffer.str(g_buffer_string);
    buffer.pubsync();

    CHECK_EQUAL("[" + g_class_name + "] " + g_buffer_string,
        ostring_stream.str());
  }
}

TEST(TestOstreamManager_Constructor_Default)
{
  if (MpiManager::Instance().AmMaster()) {
    // From https://stackoverflow.com/questions/4191089
    std::ostringstream ostring_stream;
    std::streambuf* p_cout_streambuf = std::cout.rdbuf();
    // set ostring_stream stream buffer as the stream buffer associated with
    // cout
    std::cout.rdbuf(ostring_stream.rdbuf());

    OstreamManager manager(g_class_name);
    manager << g_buffer_string;
    manager.flush();

    // Restore cout's stream buffer so we can resume normal printing
    std::cout.rdbuf(p_cout_streambuf);
    CHECK_EQUAL("[" + g_class_name + "] " + g_buffer_string,
        ostring_stream.str());
  }
}

TEST(TestOstreamManager_Constructor_UserDefined)
{
  if (MpiManager::Instance().AmMaster()) {
    std::ostringstream ostring_stream;
    OstreamManager manager(ostring_stream, g_class_name);

    manager << g_buffer_string;
    manager.flush();

    CHECK_EQUAL("[" + g_class_name + "] " + g_buffer_string,
        ostring_stream.str());
  }
}

TEST(TestOstreamManager_CopyConstructor)
{
  if (MpiManager::Instance().AmMaster()) {
    std::ostringstream ostring_stream;
    OstreamManager manager(ostring_stream, g_class_name);
    manager << g_buffer_string;

    OstreamManager manager_2(manager);
    manager_2.flush();

    // Buffer string not copied over
    CHECK_EQUAL("[" + g_class_name + "] ", ostring_stream.str());
  }
}

TEST(TestOstreamManager_CopyAssignment)
{
  TestOstreamManagerBuffer tester;
  std::ostringstream ostring_stream;
  OstreamManager manager(ostring_stream, g_class_name);

  manager.SetIsMultiOutput();
  CHECK(tester.GetIsMultiOutput() == true);

  manager.SetIsMultiOutput(false);
  CHECK(tester.GetIsMultiOutput() == false);
}
}
}
