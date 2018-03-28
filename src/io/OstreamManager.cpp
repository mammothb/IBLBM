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
//    *mpOutput << "[" << mName << ":" << singleton::mpi().getRank() << "]" <<
//        str();
  }
  else {
//    if (singleton::mpi().getRank() == 0)
//        *mpOutput << "[" << mName << "]" << str();
  }
#else
  *mpOutput << "[" << mName << "]" << str();
#endif  // IBLBM_PARALLEL_MPI
  str("");
  std::endl(*mpOutput);
  return 0;
}
}  // namespace iblbm
