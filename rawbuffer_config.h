#if !defined(RAW_BUFFER_CONFIG_H_)
#define RAW_BUFFER_CONFIG_H_

#ifdef _MSC_VER
typedef unsigned __int32 uint32_t;
typedef __int32 int32_t;
#else
#include <stdint.h>
#endif

typedef uint32_t rawbuf_uint;
typedef int32_t rawbuf_int;

//Template nesting 168 times may be safe for most compiler. You can increase it if your compiler can afford
#define MAX_MEMBER_COUNT 168

#endif

