#ifndef __UTIL_MISC_H__
#define __UTIL_MISC_H__

#ifndef MAX
  #define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
  #define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#define MACRO_STR_HELPER(x) #x
#define MACRO_STR(x) MACRO_STR_HELPER(x)

#endif
