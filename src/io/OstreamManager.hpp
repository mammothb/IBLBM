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

  /** Destructor, do nothing */
  ~OstreamManagerBuffer() override;

  /**
   * Copy constructor, copies mpOutput and mText from rRhs. Content from
   * std::stringbuf is discarded
   */
  OstreamManagerBuffer(const OstreamManagerBuffer& rRhs);

  /** Copy assignment, copies mpOutput and mText from rRhs */
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
  /** flag for output among multiple processes */
  static bool msIsMultiOutput;
};

class OstreamManager : public std::ostream
{
 public:
  /**
   * Constructor. Uses std::cout by default
   *
   * \param rClassName class name for mBuffer
   */
  explicit OstreamManager(const std::string& rClassName);

  /**
   * Constructor. Default usage
   *
   * \param rStream output stream
   * \param rClassName class name for mBuffer
   */
  OstreamManager(
      std::ostream& rStream
    , const std::string& rClassName);

  /** Destructor */
  ~OstreamManager() override;

  /** Copy constructor */
  OstreamManager(const OstreamManager& rRhs);

  /** Copy assignment */
  OstreamManager& operator=(const OstreamManager& rRhs);

  /**
   * Set flag to enable message output from all MPI processes
   *
   * \param isMultiOutput TRUE: enable messages from all MPI processes
   *                      FALSE: message output from master process only
   */
  void SetIsMultiOutput(bool isMultiOutput = true);

 private:
  /** OstreamManager has it's own special overloaded buffer */
  OstreamManagerBuffer mBuffer;
};
}

#endif // SRC_IO_OSTREAMMANAGER_HPP_
