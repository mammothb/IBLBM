#ifndef SRC_FORTESTS_UNITTESTCUSTOMUTILITIES_HPP_
#define SRC_FORTESTS_UNITTESTCUSTOMUTILITIES_HPP_

#include <algorithm>
#include <string>
#include <vector>

#include "UnitTest++.h"
#include "TestReporterStdout.h"

namespace iblbm
{
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
 * \param TestName the name of the test you wish to run
 *
 * \return the return code of UnitTest::RunTestsIf()
 */
inline int RunOneTest(std::string TestName)
{
  const UnitTest::TestList& allTests(UnitTest::Test::GetTestList());
  UnitTest::TestList selectedTests;
  UnitTest::Test* p = allTests.GetHead();

  while (p) {
    if (p->m_details.testName == TestName) {
      selectedTests.Add(p);
    }
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
 * \param SuiteName the name of the suite you wish to run
 *
 * \return the return code of UnitTest::RunTestsIf()
 */
inline int RunOneSuite(std::string SuiteName)
{
  const UnitTest::TestList& allTests(UnitTest::Test::GetTestList());
  UnitTest::TestList selectedTests;
  UnitTest::Test* p = allTests.GetHead();

  while (p) {
    if (p->m_details.suiteName == SuiteName) {
      selectedTests.Add(p);
    }
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
 * \param TestName A vector containing the names of the tests you wish to run
 *
 * \return the return code of UnitTest::RunTestsIf()
 */
inline int RunMultipleTests(std::vector<std::string> test_names)
{
  const UnitTest::TestList& allTests(UnitTest::Test::GetTestList());
  UnitTest::TestList selectedTests;
  UnitTest::Test* p = allTests.GetHead();

  while (p) {
    auto is_desired_test = std::find(begin(test_names), end(test_names),
        p->m_details.testName) != end(test_names);
    if (is_desired_test) {
      selectedTests.Add(p);
    }
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
 * \param suite_names The names of the suites you wish to run
 *
 * \return the return code of UnitTest::RunTestsIf()
 */
inline int RunMultipleSuites(std::vector<std::string> suite_names)
{
  const UnitTest::TestList& allTests(UnitTest::Test::GetTestList());
  UnitTest::TestList selectedTests;
  UnitTest::Test* p = allTests.GetHead();

  while (p) {
    auto is_desired_suite = std::find(begin(suite_names), end(suite_names),
        p->m_details.suiteName) != end(suite_names);
    if (is_desired_suite) {
      selectedTests.Add(p);
    }
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

#endif  // SRC_FORTESTS_UNITTESTCUSTOMUTILITIES_HPP_
