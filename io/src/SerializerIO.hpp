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
 */
void ConvertSerializerToOstream(
    Serializer& rSerializer
  , std::ostream& rOstream);

/**
 * Processes an rIstream to a serializer, always in parallel
 *
 * \param rSerializer a serializer
 * \param rIstream input stream
 */
void ConvertIstreamToSerializer(
    Serializer& rSerializer
  , std::istream& rIstream);
}  // namespace iblbm

#endif  // IO_SRC_SERIALIZERIO_HPP_
