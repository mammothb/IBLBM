#ifndef IO_SRC_SERIALIZERIO_HPP_
#define IO_SRC_SERIALIZERIO_HPP_

#include <iostream>

namespace iblbm
{
class Serializer;

/**
 * Processes data from a serializer to a given rOstream, always in parallel
 *
 * \param rSerializer a serializer
 * \param rOstream output stream
 * \param forceUnsigned flag to indicate usage of unsigned int
 */
void ConvertSerializerToOstream(
    Serializer& rSerializer
  , std::ostream& rOstream
  , bool forceUnsigned = false);

/**
 * Processes an rIstream to a serializer, always in parallel
 *
 * \param rSerializer a serializer
 * \param rIstream input stream
 * \param forceUnsigned flag to indicate usage of unsigned int
 */
void ConvertIstreamToSerializer(
    Serializer& rSerializer
  , std::istream& rIstream
  , bool forceUnsigned = false);
}  // namespace iblbm

#endif  // IO_SRC_SERIALIZERIO_HPP_
