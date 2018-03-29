#include "OstreamManager.hpp"

#include "IblbmDebug.hpp"
#include "MpiManager.hpp"

namespace iblbm
{
bool OstreamManagerBuffer::msIsMultiOutput = false;

OstreamManagerBuffer::OstreamManagerBuffer()
  : mpOutput{nullptr},
    mName{""}
{}

OstreamManagerBuffer::OstreamManagerBuffer(
    std::ostream& rStream
  , const std::string& rClassName)
  : mpOutput{&rStream},
    mName{rClassName}
{}

OstreamManagerBuffer::~OstreamManagerBuffer()
{}

OstreamManagerBuffer::OstreamManagerBuffer(const OstreamManagerBuffer& rRhs)
  : std::stringbuf{},
    mpOutput{rRhs.mpOutput},
    mName{rRhs.mName}
{}

OstreamManagerBuffer& OstreamManagerBuffer::operator=(
    const OstreamManagerBuffer& rRhs)
{
  mpOutput = rRhs.mpOutput;
  mName = rRhs.mName;

  return *this;
}

void OstreamManagerBuffer::SetIsMultiOutput(bool isMultiOutput/*=true*/)
{
  msIsMultiOutput = isMultiOutput;
}

int OstreamManagerBuffer::sync()
{
  IBLBM_PRECONDITION(mpOutput);
#ifdef IBLBM_PARALLEL_MPI
  if (msIsMultiOutput) {
    *mpOutput << "[" << mName << ":" << MpiManager::Instance().GetRank() <<
        "] " << str();
  }
  else {
    if (MpiManager::Instance().AmMaster())
        *mpOutput << "[" << mName << "] " << str();
  }
#else
  *mpOutput << "[" << mName << "] " << str();
#endif  // IBLBM_PARALLEL_MPI
  str("");
  mpOutput->flush();
  return 0;
}

OstreamManager::OstreamManager(const std::string& rClassName)
  : std::ostream(&mBuffer),
    mBuffer(std::cout, rClassName)
{}

OstreamManager::OstreamManager(
    std::ostream& rStream
  , const std::string& rClassName)
  : std::ostream(&mBuffer),
    mBuffer(rStream, rClassName)
{}

OstreamManager::~OstreamManager()
{}

OstreamManager::OstreamManager(const OstreamManager& rRhs)
  : std::ios{},
    std::ostream(&mBuffer),
    mBuffer(rRhs.mBuffer)
{}

OstreamManager& OstreamManager::operator=(const OstreamManager& rRhs)
{
  mBuffer = rRhs.mBuffer;
  return *this;
}

void OstreamManager::SetIsMultiOutput(bool isMultiOutput/*=true*/)
{
  mBuffer.SetIsMultiOutput(isMultiOutput);
}
}  // namespace iblbm
