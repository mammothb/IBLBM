#ifndef SRC_IO_DATAWRITER_HPP_
#define SRC_IO_DATAWRITER_HPP_

#include <boost/filesystem.hpp>

#include "AbstractCollisionModel.hpp"
#include "AbstractLatticeModel.hpp"
#include "LatticeBoltzmannMethod.hpp"

namespace iblbm
{
namespace fs = boost::filesystem;

class DataWriter
{
 public:
  explicit DataWriter(AbstractLatticeModel& rLatticeModel);

  ~DataWriter() = default;

  void InitialzeCleanFolder();

  void RegisterNsEquation(
      LatticeBoltzmannMethod* pLatticeBoltzmannMethod
    , AbstractCollisionModel* pCollisionModel);

  void WriteResult(unsigned time);

 private:
  const fs::path mRoot;
  const fs::path mResultDir;
  const fs::path mVtkFluidDir;
  AbstractLatticeModel& mrLatticeModel;

  LatticeBoltzmannMethod* mpLbmNs;
  LatticeBoltzmannMethod* mpLbmCd;
  AbstractCollisionModel* mpNs;
  AbstractCollisionModel* mpCd;

  unsigned mFieldNum;
  std::vector<std::string> mFieldNames;
  std::vector<unsigned> mFieldNums;

};
}  // namespace iblbm

#endif  // SRC_IO_DATAWRITER_HPP_
