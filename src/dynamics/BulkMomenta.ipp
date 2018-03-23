namespace iblbm
{
template <typename T, template <typename U> class Lattice>
T BulkMomenta<T, Lattice>::ComputeRho(const Cell<T, Lattice>& rCell) const
{
  return T();
//  return lbHelpers<T,Lattice>::computeRho(cell);
}
}  // namespace iblbm
