namespace iblbm
{
template <typename T, template <typename U> class Lattice>
AbstractDynamics<T, Lattice>::AbstractDynamics(
    AbstractMomentaInterface<T, Lattice>& rMomenta)
  : mrMomenta(rMomenta)
{}

template <typename T, template <typename U> class Lattice>
T AbstractDynamics<T, Lattice>::ComputeRho(
    const Cell<T, Lattice>& rCell) const
{
  return 0.0;
//  return mrMomenta.ComputeRho(rCell);
}
}  // namespace iblbm
