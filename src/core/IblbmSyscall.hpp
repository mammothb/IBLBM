#ifndef SRC_CORE_IBLBMSYSCALL_HPP_
#define SRC_CORE_IBLBMSYSCALL_HPP_

#ifdef _MSC_VER

#include <direct.h>
#define chdir _chdir

#include <process.h>
#define getpid _getpid

#include <io.h>
#define chmod _chmod
#include <sys/stat.h>
#define IBLBM_READONLY _S_IREAD
#define IBLBM_READ_EXECUTE _S_IREAD | _S_IEXEC
#define IBLBM_READ_WRITE _S_IREAD | _S_IWRITE
#define IBLBM_READ_WRITE_EXECUTE _S_IREAD | _S_IWRITE | _S_IEXEC

/**
 * Windows version of setenv call.  Note that under Linux we always pass 1
 * (overwrite) for the third arg.
 *
 * \param name  environment variable to set
 * \param value  value to give the variable
 * \param mode  not used on Windows
 */
#define setenv(name, value, mode) _putenv_s(name, value)

#else

#include <unistd.h>  // For chdir() and getpid()
#include <sys/stat.h>  // For chmod()
/** Mode for chmod() to set readonly permissions for everyone. */
#define IBLBM_READONLY 0444
/** Mode for chmod() to set read & execute permissions for everyone. */
#define IBLBM_READ_EXECUTE 0555
/**
 * Mode for chmod() to set read-write permissions for owner, and readonly for
 * group.
 */
#define IBLBM_READ_WRITE 0640
/**
 * Mode for chmod() to set full permissions for owner, and read-execute for
 * everyone else.
 */
#define IBLBM_READ_WRITE_EXECUTE 0755

#endif // _MSC_VER
#endif // SRC_CORE_IBLBMSYSCALL_HPP_
