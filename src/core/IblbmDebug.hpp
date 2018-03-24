#ifndef SRC_CORE_IBLBMDEBUG_HPP_
#define SRC_CORE_IBLBMDEBUG_HPP_

#include <cassert>
#include <iostream>

// Macros to help with debugging
#ifdef IBLBM_DEBUG

#define IBLBM_ASSERT(COND, MSG)                  \
  if (!(COND)) std::cerr << (MSG) << std::endl;  \
  assert(COND);

#define IBLBM_PRECONDITION(COND) assert(COND);
#define IBLBM_POSTCONDITION(COND) assert(COND);
#define IBLBM_STATECHECK(x, y) assert((x) == (y));

#else

#define IBLBM_ASSERT(COND, MSG)
#define IBLBM_PRECONDITION(COND)
#define IBLBM_POSTCONDITION(COND)
#define IBLBM_STATECHECK(x, y)

#endif  // IBLBM_DEBUG

#endif  // SRC_CORE_IBLBMDEBUG_HPP_
