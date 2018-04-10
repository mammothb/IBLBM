#include <iostream>
#include <sstream>

#include "UnitTest++/UnitTest++.h"
#include "Base64.hpp"
#include "LoadBalancer.hpp"
#include "MpiManager.hpp"
#include "Serializer.hpp"
#include "SerializerIO.hpp"

namespace iblbm
{
SUITE(TestIo)
{
std::map<gsl::index, gsl::index> g_local_index {{0, 1}, {2, 3}};
std::vector<gsl::index> g_global_index {12, 13};
std::map<gsl::index, std::size_t> g_rank_map {{18, 0}, {19, 1}};

TEST(TestSerializerIO_ConvertSerializerToOstream_Default)
{
  std::size_t size {1};
  LoadBalancer<double> balancer(size, g_local_index, g_global_index,
      g_rank_map);
  Serializer serializer {balancer};
  std::ostringstream ostr;

  serializer.ComputeSize();
  ConvertSerializerToOstream(serializer, ostr);

  std::ostringstream exp_ostr;
  // Binary size
  exp_ostr << "cAAAAAAAAAA=";
  // balancer size, first 6 bytes
  exp_ostr << "AQAAAAAA";
  // balancer size, last 2 bytes + mLocalIndex size, first 7 bytes
  exp_ostr << "AAACAAAAAAAA";
  // mLocalIndex size, last byte + mLocalIndex first element, first 14 bytes
  exp_ostr << "AAAAAAAAAAAAAQAAAAAA";
  // mLocalIndex first element, last 2 bytes + mLocalIndex second element,
  // 16 bytes
  exp_ostr << "AAACAAAAAAAAAAMAAAAAAAAA";
  // mGlobalIndex size, first 6 bytes
  exp_ostr << "AgAAAAAA";
  // mGlobalIndex size, last 2 bytes + mGlobalIndex first element, first
  // 7 bytes
  exp_ostr << "AAAMAAAAAAAA";
  // mGlobalIndex first element, last byte + mGlobalIndex second element,
  // 8 bytes
  exp_ostr << "AA0AAAAAAAAA";
  // mRank size, first 6 bytes
  exp_ostr << "AgAAAAAA";
  // mRank size, last 2 bytes + mRank first element, 16 bytes
  exp_ostr << "AAASAAAAAAAAAAAAAAAAAAAA";
  // mRank second element, 16 bytes + padding
  exp_ostr << "EwAAAAAAAAABAAAAAAAAAA==";

  CHECK_EQUAL(exp_ostr.str(), ostr.str());
}
}
}  // namespace iblbm

