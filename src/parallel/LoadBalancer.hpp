#ifndef SRC_PARALLEL_LOADBALANCER_HPP_
#define SRC_PARALLEL_LOADBALANCER_HPP_

#include <map>
#include <vector>

#include "gsl/gsl"
#include "BufferSerializable.hpp"

namespace iblbm
{
/**
 * Base class for all LoadBalancer.
 * Sketch: assume we have 6 cuboids and 2 threads. Thread number 1 owns
 * cuboid 0 and 1. Thread number 2 owns cuboid 2, 3, 4 and 5.
 * Then we get the following configuration:
 *
 * global cuboid number:               0   1   2   3   4   5
 * local cuboid number of thread 0:    0   1
 * local cuboid number of thread 1:            0   1   2   3
 *
 * \param mLocalIndex indicates local cuboid number in actual thread, for
 *        given global cuboid number
 * \param mGlobalIndex is a vector from 0, 1, ..., numberOfThreads - 1
 * \param mRank indicates the processing thread of a global cuboid number
 */
template<typename T>
class LoadBalancer : public BufferSerializable
{
 public:
  /**
   * Default empty constructor
   */
  explicit LoadBalancer(std::size_t size = 1);

  /**
   * Load constructor
   *
   * \param size Number of threads
   * \param rLocalIndex maps global cuboid to (local) thread cuboid
   * \param rGlobalIndex The global cuboid indices held by the thread
   * \param rRank maps global cuboid number to the processing thread
   */
  LoadBalancer(
      std::size_t size
    , std::map<gsl::index, gsl::index>& rLocalIndex
    , std::vector<gsl::index>& rGlobalIndex
    , std::map<gsl::index, std::size_t>& rRank);

  /** Default empty destructor */
  virtual ~LoadBalancer();

  /**
   * Swap method, not capitalized to keep consistency with standard swap
   * method
   */
  void swap(LoadBalancer<T>& rLoadBalancer);

  /** \return read-only access to mSize */
  std::size_t GetSize() const;

  /**
   * Return a pointer to the memory of the current block and its size for the
   * serializable interface. Block sequence for LoadBalancer is typically:
   * 0: mSize
   * 1: mLocalIndex.size()
   *    <elements in mLocalIndex>
   * 2: mGlobalIndex.size()
   *    <elements in mGlobalIndex>
   * 3: mRank.size()
   *    <elements in mRank>
   *
   * \param blockIndex Index of the block to be returned
   * \param blockSize Reference to the size of the returned block
   * \param isLoad flag to decide if we are loading or saving
   *
   * \return Pointer to the current block
   */
  bool* GetBlock(
      gsl::index blockIndex
    , std::size_t& rBlockSize
    , bool isLoad = false) override;

  /** \return the number of blocks. */
  std::size_t GetNumBlock() const override;

  /** \return Binary size for the serializer */
  std::size_t GetSerializableSize() const override;

  /** Print the contents of load balancer for debugging */
  void Print(bool multiOutput = false) const;

 private:
  /** Number of cuboids assigned */
  std::size_t mSize;
  /**
   * maps global cuboid to (local) thread cuboid. Using the illustration
   * above,
   * - mLocalIndex of load_balancer in thread 0:
   *   {{0, 0}, {1, 1}}
   * - mLocalIndex of load_balancer in thread 1:
   *   {{2, 0}, {3, 1}, {4, 2}, {5, 3}}
   */
  std::map<gsl::index, gsl::index> mLocalIndex;
  /**
   * The global cuboid indices held by the thread. Using the illustration
   * above,
   * - mGlobalIndex of load_balancer in thread 0:
   *   {0, 1}
   * - mGlobalIndex of load_balancer in thread 1:
   *   {2, 3, 4, 5}
   */
  std::vector<gsl::index> mGlobalIndex;
  /** maps global cuboid number to the processing thread. Using the
   * illustration above, mRank will be:
   *   {{0, 0}, {1, 0}{2, 1}, {3, 1}, {4, 1}, {5, 1}}*/
  std::map<gsl::index, std::size_t> mRank;
};
}  // namespace iblbm

#endif  // SRC_PARALLEL_LOADBALANCER_HPP_
