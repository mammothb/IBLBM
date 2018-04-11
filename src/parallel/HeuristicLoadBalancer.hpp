#ifndef SRC_PARALLEL_HEURISTICLOADBALANCER_HPP_
#define SRC_PARALLEL_HEURISTICLOADBALANCER_HPP_

#include "CuboidGeometry2D.hpp"
#include "LoadBalancer.hpp"
#include "MpiManager.hpp"

namespace iblbm
{
/**
 * This class has a virtual method call in its destructor and should
 * therefore not be used as a base class.
 */
template<typename T>
class HeuristicLoadBalancer final : public LoadBalancer<T>
{
  friend class TestHeuristicLoadBalancer;
 public:
  /**
   * Default (empty) constructor
   */
  HeuristicLoadBalancer();

  /**
   * Constructs a load balancer from a given cuboid geometry using a
   * heuristic.
   * \param rCuboidGeometry2D cuboid geometry to base the load balance on
   * \param ratioFullEmpty time it takes for full cells in relation to empty
   *        cells
   * \param emptyCellWeight weight of empty cells
   */
  HeuristicLoadBalancer(
      CuboidGeometry2D<T>& rCuboidGeometry2D
    , const double ratioFullEmpty = 3.7
    , const double emptyCellWeight = 0.0);

  /**
   * Destructor
   */
  ~HeuristicLoadBalancer() override;

  /**
   * Initialize the load balancer
   */
  void Initialize(
      CuboidGeometry2D<T>& rCuboidGeometry2D
    , const double ratioFullEmpty = 3.7
    , const double emptyCellWeight = 0.0) override;

 private:
#ifdef IBLBM_PARALLEL_MPI
#endif  // IBLBM_PARALLEL_MPI
  /** Pointer to cuboid geometry */
  CuboidGeometry2D<T>* mpCuboidGeometry2D;
  /** time it takes for full cells in relation to empty cells */
  double mRatioFullEmpty;
};
}  // namespace iblbm

#endif  // SRC_PARALLEL_HEURISTICLOADBALANCER_HPP_
