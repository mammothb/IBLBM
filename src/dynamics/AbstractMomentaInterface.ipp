namespace iblbm
{
template<typename T, template<typename U> class Lattice>
void AbstractMomentaInterface<T, Lattice>::ComputeRhoAndU(
    const Cell<T, Lattice>& rCell
  , T& rRho
  , std::vector<T>& rU) const
{
  rRho = this->ComputeRho(rCell);
  this->ComputeU(rCell, rU);
}
}  // namespace iblbm
