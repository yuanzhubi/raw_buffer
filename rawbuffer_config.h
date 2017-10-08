#if !defined(RAW_BUFFER_CONFIG_H_)
#define RAW_BUFFER_CONFIG_H_

#ifdef _MSC_VER

typedef unsigned __int32 rawbuf_uint32;
typedef __int32 rawbuf_int32;

typedef unsigned __int64 rawbuf_uint64;
typedef __int64 rawbuf_int64;

#if defined(_M_IX86) || defined(_M_IA64) || defined(_M_AMD64)
#define RAWBUF_LITTLE_ENDIAN
#endif

//Condition expression in "if" is const to reduce the use of template
__pragma(warning(disable:4127)) 
//You can redefine the offset type, array size type by typedef them in the packet, hidden the default definition in parent class.
__pragma(warning(disable:4458))
#else
#include <stdint.h>
typedef uint32_t rawbuf_uint32;
typedef int32_t rawbuf_int32;
typedef uint64_t rawbuf_uint64;
typedef int64_t rawbuf_int64;
#endif

#ifdef __GNUC__

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN
#define RAWBUF_LITTLE_ENDIAN
#endif

#endif //__GNUC__


#define RAWBUF_ENABLE_TEMPLATE_PACKET

//Following compiler and version can not support template packet 
#if defined(__GNUC__)  && (!defined(__ICC)) && (!defined(__INTEL_COMPILER)) && (!defined(__clang__))
#if GCC_VERSION < 40500     //The compiler will generate internel error until gcc-4.5.
#undef RAWBUF_ENABLE_TEMPLATE_PACKET
#endif
#endif //__GNUC__

#if defined(_MSC_VER) && (!defined(__ICC)) && (!defined(__INTEL_COMPILER)) && (!defined(__clang__))
//MSVC will lose the member function of the base class of the inner class rawbuf_writer_helper and rawbuf_reader_helper, only can be visited from raw pointer.
//So in the test demo, we disable the template packet example for MSVC. 
#undef RAWBUF_ENABLE_TEMPLATE_PACKET    
#endif



//Template nesting 168 times may be safe for most compiler. You can increase it if your packet has more field and your compiler can afford
#define MAX_FIELDS_COUNT 168

#endif

