#ifndef SRC_PARALLEL_HEURISTICLOADBALANCER_HPP_
#define SRC_PARALLEL_HEURISTICLOADBALANCER_HPP_

#include "LoadBalancer.hpp"

namespace iblbm
{
/** Constructs a load balancer from a given cuboid geometry using a heuristic.
 * \param cGeometry     cuboid geometry to base the load balance on
 * \param blockGeometry used to determine number of full and empty cells if given
 * \param ratioFullEmpty time it takes for full cells in relation to empty cells
 *
 * This class has a virtual method call in its destructor and should therefore
 * not be used as a base class.
 */
template<typename T>
class HeuristicLoadBalancer final : public LoadBalancer<T>
{
 public:
  /**
   *
   */
  HeuristicLoadBalancer();

  ~HeuristicLoadBalancer() override;
};
}  // namespace iblbm

#endif  // SRC_PARALLEL_HEURISTICLOADBALANCER_HPP_
