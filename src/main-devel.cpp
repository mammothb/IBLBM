#include <string>
#include <vector>

#include "UnitTest++/UnitTest++.h"
#include "IblbmInitialize.hpp"
#include "UnitTestCustomUtilities.hpp"

#include <iostream>

int main(int argc, char** argv)
{
  iblbm::IblbmInitialize(&argc, &argv, /*isVerbose=*/true);

  std::vector<std::string> functionality_suites = {
      "TestBoundary",
      "TestCore",
      "TestDynamics",
      "TestIo"
  };

//  return iblbm::RunAllTheTests();

  /// Run multiple suites
  return iblbm::RunMultipleSuites(functionality_suites);

  /// Run single suite
//  return iblbm::RunOneSuite("TestBoundary");
//  return iblbm::RunOneSuite("TestCore");
//  return iblbm::RunOneSuite("TestDynamics");

  /// Analytical solutions
//  return iblbm::RunOneTest("AnalyticalPoiseuille");

  /// Simulations
//  return iblbm::RunOneTest("Simulation_Poiseuille_BodyForceDriven");

  /// Individual tests
//  return iblbm::RunOneTest("TestLbmHelpers_ComputeEquilibrium");

  // to run only one suite, uncomment the following line
  // and specify the name of the suite. Also, comment out the RunAllTheTests()
  // return iblbm::RunOneSuite("suite_name");

  // to run only one test, uncomment the following line
  // and specify the name of the test.Also, comment out the RunAllTheTests()
  // return iblbm::RunOneTest("test_name");
}
