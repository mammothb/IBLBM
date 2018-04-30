#ifndef PARALLEL_SRC_SUPERSTRUCTURE2D_HPP_
#define PARALLEL_SRC_SUPERSTRUCTURE2D_HPP_

#include <cstddef>

#include "Communicator2D.hpp"
#include "CuboidGeometry2D.hpp"
#include "LoadBalancer.hpp"

namespace iblbm
{
//template<typename T> class Communicator2D;

template<typename T>
class SuperStructure2D
{
 public:
  /**
   * Default constructor for empty SuperStructure2D
   */
  explicit SuperStructure2D(std::size_t overlap = 1);

  /**
   * Constructor
   *
   * \param rCuboidGeometry Reference to CuboidGeometry which holds the grid
   *        structure
   * \param rLoadBalancer Reference to LoadBalancer to evenly distribute the
   *        load across processes
   * \param overlap Size of ghost cell layer (must be greater than 1 and
   *        BC overlap)
   */
  SuperStructure2D(
      CuboidGeometry2D<T>& rCuboidGeometry
    , LoadBalancer<T>& rLoadBalancer
    , std::size_t overlap = 1);

  /**
   * Virtual destructor
   */
  virtual ~SuperStructure2D();

  /**
   * Write access to the memory of the data of the super structure where
   * (xIndex, yIndex) is the point providing the data dataIndex in the block
   * localCuboidIndex
   */
  virtual bool* operator()(
      gsl::index localCuboidIndex
    , gsl::index xIndex
    , gsl::index yIndex
    , gsl::index dataIndex) = 0;

  /**
   * \return Read only access to the dim of the data of the super structure
   */
  virtual std::size_t GetDataSize() const = 0;

  /**
   * \return Read only access to the data type dim of the data of the super
   * structure
   */
  virtual std::size_t GetSizeofDataType() const = 0;

  /** Communicates the data in the overlap */
  virtual void Communicate(bool isVerbose = false);

  /** Initialize communicator */
  void InitializeCommunication();

  /** \return read and write access to CuboidGeometry */
  CuboidGeometry2D<T>& rGetCuboidGeometry();

  /** \return read-only access to CuboidGeometry */
  const CuboidGeometry2D<T>& rGetCuboidGeometry() const;

  /** \return Read and write access to the load balancer */
  LoadBalancer<T>& rGetLoadBalancer();

  /** \return Read-only access to the load balancer */
  const LoadBalancer<T>& rGetLoadBalancer() const;

  /** \return  Read only access to the overlap */
  std::size_t GetOverlap() const;

 protected:
  /** Grid structure */
  CuboidGeometry2D<T>& mrCuboidGeometry;
  /** Even distribution of cuboid (load) to processes */
  LoadBalancer<T>& mrLoadBalancer;
  /** Size of ghost cell layer (must be greater than 1 and BC overlap) */
  std::size_t mOverlap;
  /** This communicator handles the communication of the overlap */
  Communicator2D<T> mCommunicator;
  /**
   * Flag to indicate if there has been some data updated which requires
   * communication
   */
  bool mNeedCommunication;
  /** Class specific output stream */
  mutable OstreamManager mOstream;
};
}  // namespace iblbm

#endif  // PARALLEL_SRC_SUPERSTRUCTURE2D_HPP_
