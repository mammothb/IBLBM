#include <vector>

#include "BounceBackBoundary.hpp"
#include "CollisionNsf.hpp"
#include "DataWriter.hpp"
#include "Lattice2D.hpp"
#include "LatticeBoltzmannMethod.hpp"
#include "PeriodicBoundary.hpp"
#include "Stream2D.hpp"
#include "UnitTest++.h"

namespace iblbm
{
TEST(SimulatePoiseuilleFlow_BodyForce_FullwayBounceBack)
{
  auto ny = 18u;
  auto nx = 34u;
  auto initial_density = 1.0;
  std::vector<double> initial_velocity = {0.0, 0.0};
  std::vector<double> body_force = {0.001, 0.0};
  auto sim_time = 10001u;
  auto sampling_interval = sim_time / 100;

  Lattice2D lattice(nx, ny, initial_velocity);
  CollisionNsf collision(lattice, initial_density);
  Stream2D stream(lattice);
  BounceBackBoundary bounce_back(lattice, &collision);
  PeriodicBoundary periodic(lattice, collision);

  LatticeBoltzmannMethod lbm(lattice, collision, stream);
  DataWriter writer(lattice);

  for (auto x = 0u; x < nx; ++x) {
    bounce_back.AddNode(x, 0u);
    bounce_back.AddNode(x, ny - 1);
  }
  for (auto y = 1u; y < ny - 1; ++y) {
    periodic.AddNode(0u, y);
    periodic.AddNode(nx - 1, y);
  }

  collision.SetForce(body_force);
  collision.SetRelaxationTime(0.8);

  lbm.AddBoundaryCondition(&bounce_back);
  lbm.AddBoundaryCondition(&periodic);

  writer.RegisterNsEquation(&lbm, &collision);

  for (auto t = 0u; t < sim_time; ++t) {
    lbm.TakeStep();
    if (t % sampling_interval == 0) {
      writer.WriteResult(t);
      std::cout << t << std::endl;
    }
  }
  auto velocity = lattice.rGetVelocity();
  for (auto y = 1u; y < ny - 1; ++y) {
    auto n = nx / 2 + y * nx;
    std::cout << velocity[n][0] << std::endl;
  }
}
}  // namespace iblbm
