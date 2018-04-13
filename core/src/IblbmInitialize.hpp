#ifndef CORE_SRC_IBLBMINITIALIZE_HPP_
#define CORE_SRC_IBLBMINITIALIZE_HPP_

#include <iostream>

#include "MpiManager.hpp"
#include "OstreamManager.hpp"

namespace iblbm
{
inline void IblbmInitialize(
    int* pArgc
  , char*** pArgv
  , bool isVerbose = false)
{
  iblbm::OstreamManager ostream_manager(std::cout, "IblbmInitialize");
  ostream_manager.SetIsMultiOutput(isVerbose);
  MpiManager::Instance().Initialize(pArgc, pArgv);
}
}  // namespace iblbm

#endif  // CORE_SRC_IBLBMINITIALIZE_HPP_
