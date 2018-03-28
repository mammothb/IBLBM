#include <iostream>

#include "UnitTest++/UnitTest++.h"
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
TEST(TestOstreamManagerBuffer_Constructor_Default)
{
  TestOstreamManagerBuffer tester;
  OstreamManagerBuffer buffer;

  CHECK(tester.pGetOutput(buffer) == nullptr);
  CHECK_EQUAL("", tester.GetName(buffer));
  CHECK(tester.GetIsMultiOutput() == false);
}

TEST(TestOstreamManagerBuffer_Constructor_UserDefined)
{
  auto class_name = "Class Name";
  TestOstreamManagerBuffer tester;
  OstreamManagerBuffer buffer(std::cout, class_name);

  CHECK(tester.pGetOutput(buffer) == &std::cout);
  CHECK_EQUAL(class_name, tester.GetName(buffer));
  CHECK(tester.GetIsMultiOutput() == false);
}

TEST(TestOstreamManagerBuffer_CopyConstructor)
{
  auto class_name = "Class Name";
  auto buffer_string = "Buffer string";
  TestOstreamManagerBuffer tester;
  OstreamManagerBuffer buffer(std::cout, class_name);
  buffer.str(buffer_string);
  OstreamManagerBuffer buffer_2(buffer);

  CHECK(tester.pGetOutput(buffer_2) == &std::cout);
  CHECK_EQUAL(class_name, tester.GetName(buffer_2));
  CHECK_EQUAL(buffer_string, buffer.str());
  CHECK_EQUAL("", buffer_2.str());
}

TEST(TestOstreamManagerBuffer_CopyAssignment)
{
  auto class_name = "Class Name";
  auto buffer_string = "Buffer string";
  TestOstreamManagerBuffer tester;
  OstreamManagerBuffer buffer(std::cout, class_name);
  buffer.str(buffer_string);
  auto buffer_2 = buffer;

  CHECK(tester.pGetOutput(buffer_2) == &std::cout);
  CHECK_EQUAL(class_name, tester.GetName(buffer_2));
  CHECK_EQUAL(buffer_string, buffer.str());
  CHECK_EQUAL("", buffer_2.str());
}

TEST(TestOstreamManagerBuffer_SetIsMultiOutput)
{
  TestOstreamManagerBuffer tester;
  OstreamManagerBuffer buffer;

  buffer.SetIsMultiOutput();

  CHECK(tester.GetIsMultiOutput() == true);
}

TEST(TestOstreamManagerBuffer_sync)
{
  auto class_name = "Class Name";
  TestOstreamManagerBuffer tester;
  OstreamManagerBuffer buffer(std::cout, class_name);


  std::cout << buffer.pubsync() << std::endl;
}
}
}
