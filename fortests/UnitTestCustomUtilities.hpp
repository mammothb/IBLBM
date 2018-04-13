#ifndef FORTESTS_SRC_UNITTESTCUSTOMUTILITIES_HPP_
#define FORTESTS_SRC_UNITTESTCUSTOMUTILITIES_HPP_

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "UnitTest++/UnitTest++.h"
#include "UnitTest++/TestReporterStdout.h"
#include "Exception.hpp"

namespace iblbm
{
/**
 * This macro allows us to check exceptions messages contains a certain
 * message. This macro will assign cerr an temporary string buffer so that
 * error message from the test will not pollute output. It will restore the
 * buffer at the end of the macro.
 */
#define CHECK_THROW_CONTAINS(block, message)                    \
  {                                                             \
    std::ostringstream ostring_stream;                          \
    std::streambuf* p_cerr_streambuf = std::cerr.rdbuf();       \
    std::cerr.rdbuf(ostring_stream.rdbuf());                    \
    try {                                                       \
      block;                                                    \
    }                                                           \
    catch (const Exception& exc) {                              \
      CHECK_EQUAL("", exc.CheckShortMessageContains(message));  \
    }                                                           \
    std::cerr.rdbuf(p_cerr_streambuf);                          \
  }

namespace testutil
{
/**
 * Compares two vectors of doubles and ensures that their elementwise values
 * are within delta-% apart
 *
 * @param rActualVector First vector
 * @param rExpectedVector Second vector
 * @param delta Maximum percentage by which elements in the vectors can be
 *        different
 *
 * @return TRUE if all elements in the vectors are within delta=% from each
 *         other
 *         FALSE if vectors are of different length or if the elements are
 *         not within delta-% apart
 */
template<typename VECTOR>
bool CheckCloseVector(
    const VECTOR& rActualVector
  , const VECTOR& rExpectedVector
  , double delta)
{
  // Check that the two array are of equal length
  auto ret = rActualVector.size() == rExpectedVector.size();
  if (ret) {
    for (auto i = 0u; i < rExpectedVector.size(); ++i) {
      ret = ret && std::abs(rActualVector[i] - rExpectedVector[i]) <=
          std::abs(rExpectedVector[i]) * delta;
      if (!ret) break;
    }
  }
  else {
    std::cout << "Vectors have different size!" << std::endl;
  }
  return ret;
}

/**
 * Compares two vectors of doubles and ensures that their elementwise values
 * are within delta-% apart
 *
 * @param rActualVector First vector
 * @param rExpectedVector Second vector
 * @param delta Maximum percentage by which elements in the vectors can be
 *        different
 *
 * @return TRUE if all elements in the vectors are within delta=% from each
 *         other
 *         FALSE if vectors are of different length or if the elements are
 *         not within delta-% apart
 */
template<typename VECTOR>
bool CheckEqualVector(
    const VECTOR& rActualVector
  , const VECTOR& rExpectedVector)
{
  // Check that the two array are of equal length
  auto ret = rActualVector.size() == rExpectedVector.size();
  if (ret) {
    for (auto i = 0u; i < rExpectedVector.size(); ++i) {
      ret = ret && rActualVector[i] == rExpectedVector[i];
      if (!ret) break;
    }
  }
  else {
    std::cout << "Vectors have different size!" << std::endl;
  }
  return ret;
}
}  // namespace testutil

/**
 * \brief Runs all the tests it finds in the project.
 *        It calls RunAllTests() within UnitTests++
 *
 * Behaviours:
 *   It runs all the test it finds. See documentation of
 *   UnitTest::RunAllTests()
 * Intended use:
 *   int return_code = RunAllTheTests();
 *
 * \return the return code of UnitTest::RunAllTests()
 */
inline int RunAllTheTests()
{
  return UnitTest::RunAllTests();
}

#define COVERAGE_IGNORE
/**
 * \brief Runs only one test specified by the name.
 *        It runs nothing if the test does not exists.
 *
 * Behaviours:
 *   it runs one test in isolation.
 *
 * Intended use (assuming you have TEST(name_of_my_test) among your tests):
 *   int return_code = RunOneTest("name_of_my_test");
 *
 * \param rTestName the name of the test you wish to run
 *
 * \return the return code of UnitTest::RunTestsIf()
 */
inline int RunOneTest(const std::string& rTestName)
{
  const UnitTest::TestList& allTests(UnitTest::Test::GetTestList());
  UnitTest::TestList selectedTests;
  UnitTest::Test* p = allTests.GetHead();

  while (p) {
    if (p->m_details.testName == rTestName) selectedTests.Add(p);
    UnitTest::Test* q = p;
    p = p->m_nextTest;
    q->m_nextTest = nullptr;
  }

  UnitTest::TestReporterStdout reporter;
  UnitTest::TestRunner runner(reporter);

  return runner.RunTestsIf(selectedTests, nullptr, UnitTest::True(), 0);
}

/**
 * \brief Runs only one test suite (all the tests in the suite) specified by
 *        the name. It runs nothing if the suite does not exists.
 *
 * Behaviours:
 *   it runs one suite in isolation
 * Intended use:
 *   int return_code = RunOneSuite("name_of_my_suite");
 *
 * \param rSuiteName the name of the suite you wish to run
 *
 * \return the return code of UnitTest::RunTestsIf()
 */
inline int RunOneSuite(const std::string& rSuiteName)
{
  const UnitTest::TestList& allTests(UnitTest::Test::GetTestList());
  UnitTest::TestList selectedTests;
  UnitTest::Test* p = allTests.GetHead();

  while (p) {
    if (p->m_details.suiteName == rSuiteName) selectedTests.Add(p);
    UnitTest::Test* q = p;
    p = p->m_nextTest;
    q->m_nextTest = nullptr;
  }

  UnitTest::TestReporterStdout reporter;
  UnitTest::TestRunner runner(reporter);

  return runner.RunTestsIf(selectedTests, nullptr, UnitTest::True(), 0);
}

/**
 * \brief Runs multiple tests specified by the names.
 *        It runs nothing if all of the tests do not exist.
 *
 * Behaviours:
 *   it runs multiple tests in isolation.
 *
 * Intended use (assuming you have TEST(name_of_my_test) among your tests):
 *   int return_code = RunOneTest(name_of_my_tests);
 *
 * \param rTestNames A vector containing the names of the tests you wish to
 *        run
 *
 * \return the return code of UnitTest::RunTestsIf()
 */
inline int RunMultipleTests(const std::vector<std::string>& rTestNames)
{
  const UnitTest::TestList& allTests(UnitTest::Test::GetTestList());
  UnitTest::TestList selectedTests;
  UnitTest::Test* p = allTests.GetHead();

  while (p) {
    auto is_desired_test = std::find(begin(rTestNames), end(rTestNames),
        p->m_details.testName) != end(rTestNames);
    if (is_desired_test) selectedTests.Add(p);
    UnitTest::Test* q = p;
    p = p->m_nextTest;
    q->m_nextTest = nullptr;
  }

  UnitTest::TestReporterStdout reporter;
  UnitTest::TestRunner runner(reporter);

  return runner.RunTestsIf(selectedTests, nullptr, UnitTest::True(), 0);
}

/**
 * \brief Runs only multiple test suites (all the tests in the suites)
 *        specified by the names. It runs nothing if none of the suites
 *        exists.
 *
 * Behaviours:
 *   it runs multiple suites
 * Intended use:
 *   int return_code = RunMultipleSuites(names_of_my_suites);
 *
 * \param rSuiteNames The names of the suites you wish to run
 *
 * \return the return code of UnitTest::RunTestsIf()
 */
inline int RunMultipleSuites(const std::vector<std::string>& rSuiteNames)
{
  const UnitTest::TestList& allTests(UnitTest::Test::GetTestList());
  UnitTest::TestList selectedTests;
  UnitTest::Test* p = allTests.GetHead();

  while (p) {
    auto is_desired_suite = std::find(begin(rSuiteNames), end(rSuiteNames),
        p->m_details.suiteName) != end(rSuiteNames);
    if (is_desired_suite) selectedTests.Add(p);
    UnitTest::Test* q = p;
    p = p->m_nextTest;
    q->m_nextTest = nullptr;
  }

  UnitTest::TestReporterStdout reporter;
  UnitTest::TestRunner runner(reporter);

  return runner.RunTestsIf(selectedTests, nullptr, UnitTest::True(), 0);
}
#undef COVERAGE_IGNORE
}  // namespace iblbm

#endif  // FORTESTS_SRC_UNITTESTCUSTOMUTILITIES_HPP_
