//#include "Serializable.hpp"
//
//namespace iblbm
//{
//template<typename TYPE>
//void Serializable::RegisterVar(
//    const gsl::index blockIndex
//  , std::size_t& rBlockSize
//  , gsl::index& rCurrentBlockIndex
//  , bool*& rpData
//  , const TYPE& rData
//  , const std::size_t length/*=1*/) const
//{
//  if (blockIndex == rCurrentBlockIndex) {
//    rBlockSize = sizeof(TYPE) * length;
//    // Since we are casting a const of a different type
//    rpData = reinterpret_cast<bool*>(const_cast<TYPE*>(&rData));
//  }
//  ++rCurrentBlockIndex;
//}
//
//
//}  // namespace iblbm
