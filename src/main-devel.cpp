#include <string>
#include <vector>
#include "UnitTest++.h"
#include "UnitTestCustomUtilities.hpp"

int main()
{
  std::vector<std::string> functionality_suites = {
      "TestBouncebackNodes"
    , "TestCollisionCD"
    , "TestCollisionNS"
    , "TestCollisionNSF"
    , "TestConcentrationBoundaryNodes"
    , "TestConvectiveOutflowNodes"
    , "TestExceptions"
    , "TestImmersedBoundary"
    , "TestLatticeBoltzmann"
    , "TestLatticeD2Q9"
    , "TestMatrixOperations"
    , "TestNonEquilibriumExtrapolationNodes"
    , "TestNonEquilibriumExtrapolationPressureNodes"
    , "TestParticleRigid"
    , "TestPeriodicNodes"
    , "TestStreamD2Q9"
    , "TestStreamPeriodic"
    , "TestZouHeNodes"
    , "TestZouHePressureNodes"};

  return iblbm::RunAllTheTests();

  /// Run multiple suites
//  return iblbm::RunMultipleSuites(functionality_suites);
//  return iblbm::RunOneTest("ParticleRigid_GrowPlaque_BelowThreshold");

  /// Run single suite
//  return iblbm::RunOneSuite("TestAnalyticalSolutions");
//  return iblbm::RunOneSuite("TestBouncebackNodes");
//  return iblbm::RunOneSuite("TestCollisionCD");
//  return iblbm::RunOneSuite("TestCollisionNS");
//  return iblbm::RunOneSuite("TestCollisionNSF");
//  return iblbm::RunOneSuite("TestConcentrationBoundaryNodes");
//  return iblbm::RunOneSuite("TestConvectiveOutflowNodes");
//  return iblbm::RunOneSuite("TestExceptions");
//  return iblbm::RunOneSuite("TestImmersedBoundary");
//  return iblbm::RunOneSuite("TestLatticeBoltzmann");
//  return iblbm::RunOneSuite("TestLatticeD2Q9");
//  return iblbm::RunOneSuite("TestMatrixOperations");
//  return iblbm::RunOneSuite("TestNonEquilibriumExtrapolationNodes");
//  return iblbm::RunOneSuite("TestNonEquilibriumExtrapolationPressureNodes");
//  return iblbm::RunOneSuite("TestParticleRigid");
//  return iblbm::RunOneSuite("TestPeriodicNodes");
//  return iblbm::RunOneSuite("TestSteadyState");
//  return iblbm::RunOneSuite("TestStreamD2Q9");
//  return iblbm::RunOneSuite("TestStreamPeriodic");
//  return iblbm::RunOneSuite("TestZouHeNodes");
//  return iblbm::RunOneSuite("TestZouHePressureNodes");

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
//  return iblbm::RunOneTest("SimulatePoiseuilleFlow");

  /// Individual tests

  // to run only one suite, uncomment the following line
  // and specify the name of the suite. Also, comment out the RunAllTheTests()
  // return iblbm::RunOneSuite("suite_name");

  // to run only one test, uncomment the following line
  // and specify the name of the test.Also, comment out the RunAllTheTests()
  // return iblbm::RunOneTest("test_name");
}
