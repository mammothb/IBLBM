#ifndef SRC_IO_OSTREAMMANAGER_HPP_
#define SRC_IO_OSTREAMMANAGER_HPP_

#include <sstream>

namespace iblbm
{
/**
 * User defined stream buffer for OstreamManager
 */
class OstreamManagerBuffer : public std::stringbuf
{
  friend class TestOstreamManagerBuffer;
 public:
  /**
   * Constructor. Initialize mpOutput as a nullptr and mText as an empty
   * string
   */
  OstreamManagerBuffer();

  /**
   * Constructor. Provide a stream to write output to and sets the name of
   * the owner class
   *
   * \param rStream stream to write output to
   * \param rClassName name of owner class
   */
  OstreamManagerBuffer(
      std::ostream& rStream
    , const std::string& rClassName);

  /**
   * Destructor, do nothing
   */
  ~OstreamManagerBuffer() override;

  /**
   * Copy constructor, copies mpOutput and mText from rRhs. Content from
   * std::stringbuf is discarded
   *
   * \param rRhs the OstreamManagerBuffer to be copied from
   */
  OstreamManagerBuffer(const OstreamManagerBuffer& rRhs);

  /**
   * Copy assignment, copies mpOutput and mText from rRhs
   */
  OstreamManagerBuffer& operator=(const OstreamManagerBuffer& rRhs);

  /** Set flag for output among multiple processes */
  void SetIsMultiOutput(bool isMultiOutput = true);

 protected:
  int sync() override;

 private:
  /** Stream to write output to */
  std::ostream* mpOutput;
  /** Name of the owner class */
  std::string mName;
  static bool msIsMultiOutput;
};
}

#endif // SRC_IO_OSTREAMMANAGER_HPP_
