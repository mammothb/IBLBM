/** \file
 * Descriptor for various lattice arrangement.
 *  -- template implementation
 */
namespace iblbm
{
namespace descriptor
{
/***************************************************************************
 * D2Q9
 ***************************************************************************/
// Proper initialization of constexpr member in template
// https://stackoverflow.com/questions/14395967
template <typename T>
constexpr std::size_t D2Q9DescriptorBase<T>::sD;

template <typename T>
constexpr std::size_t D2Q9DescriptorBase<T>::sQ;

template <typename T>
const std::size_t D2Q9DescriptorBase<T>::sVicinity = 1u;

template <typename T>
const std::vector<std::vector<T>> D2Q9DescriptorBase<T>::sE = {
    {0, 0},
    {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}
};

template <typename T>
const std::vector<std::size_t> D2Q9DescriptorBase<T>::sOpposite = {
    0, 5, 6, 7, 8, 1, 2, 3, 4
};

template <typename T>
const std::vector<T> D2Q9DescriptorBase<T>::sWeight = {
    static_cast<T>(4) / static_cast<T>(9),
    static_cast<T>(1) / static_cast<T>(36),
    static_cast<T>(1) / static_cast<T>(9),
    static_cast<T>(1) / static_cast<T>(36),
    static_cast<T>(1) / static_cast<T>(9),
    static_cast<T>(1) / static_cast<T>(36),
    static_cast<T>(1) / static_cast<T>(9),
    static_cast<T>(1) / static_cast<T>(36),
    static_cast<T>(1) / static_cast<T>(9)
};

template <typename T>
const T D2Q9DescriptorBase<T>::sInvCsSqr = static_cast<T>(3);
}  // namespace descriptor
}  // namespace iblbm
