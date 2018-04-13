#ifndef SRC_CORE_BLOCKSTRUCTURE2D_HPP_
#define SRC_CORE_BLOCKSTRUCTURE2D_HPP_

#include <cstddef>

namespace iblbm
{
class BlockStructure2D
{
 public:
  /**
   * Constructor. Create an empty shell with lower left corner at (0, 0)
   *
   * \param nx extension in x-direction
   * \param ny extension in y-direction
   */
  BlockStructure2D(
      std::size_t nx
    , std::size_t ny);

  /** \return read-only acces to mNx */
  std::size_t GetNx() const;

  /** \return read-only acces to mNy */
  std::size_t GetNy() const;

 protected:
  /** extension in x-direction */
  std::size_t mNx;
  /** extension in y-direction */
  std::size_t mNy;
};
}  // namespace iblbm

#endif // SRC_CORE_BLOCKSTRUCTURE2D_HPP_
