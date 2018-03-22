#include <string>
#include <vector>
#include "UnitTest++.h"
#include "UnitTestCustomUtilities.hpp"

int main()
{
  std::vector<std::string> functionality_suites = {
      "TestBoundaryCondition",
      "TestCollisionModel",
      "TestLatticeBoltzmannMethod",
      "TestLatticeModel",
      "TestStreamModel",
      "TestUtilities"};

//  return iblbm::RunAllTheTests();

  /// Run multiple suites
  return iblbm::RunMultipleSuites(functionality_suites);

  /// Run single suite
//  return iblbm::RunOneSuite("TestBoundaryCondition");
//  return iblbm::RunOneSuite("TestCollisionModel");
//  return iblbm::RunOneSuite("TestLatticeBoltzmannMethod");
//  return iblbm::RunOneSuite("TestLatticeModel");
//  return iblbm::RunOneSuite("TestStreamModel");
//  return iblbm::RunOneSuite("TestUtilities");

  /// Analytical solutions
//  return iblbm::RunOneTest("AnalyticalDiffusion_PointSource");
//  return iblbm::RunOneTest("AnalyticalDiffusion_BouncebackBoundaryCondition");
//  return iblbm::RunOneTest("AnalyticalDiffusion_NonEquilibriumExtrapolation");
//  return iblbm::RunOneTest("AnalyticalDiffusion_ZouHePressureNodes");
//  return iblbm::RunOneTest("AnalyticalPoiseuille");
//  return iblbm::RunOneTest("AnalyticalPoiseuilleZH");
//  return iblbm::RunOneTest("AnalyticalPoiseuilleWithPorosity");
//  return iblbm::RunOneTest("AnalyticalTaylorVortex");
//  return iblbm::RunOneTest("AnalyticalTaylorVortexForce");
//  return iblbm::RunOneTest("AnalyticalWomersleyFlow");

  /// Simulations
//  return iblbm::RunOneTest("SimulatePoiseuilleFlow_BodyForce_FullwayBounceBack");

  /// Individual tests

  // to run only one suite, uncomment the following line
  // and specify the name of the suite. Also, comment out the RunAllTheTests()
  // return iblbm::RunOneSuite("suite_name");

  // to run only one test, uncomment the following line
  // and specify the name of the test.Also, comment out the RunAllTheTests()
  // return iblbm::RunOneTest("test_name");
}
