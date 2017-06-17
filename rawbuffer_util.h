#if !defined(RAW_BUFFER_UTIL_H_)
#define RAW_BUFFER_UTIL_H_

#include "rawbuffer_config.h"

#include <stdlib.h>

#define RAW_BUF_ALIGN(offset, size)  (((offset) + (size) -1 )&(~((size) - 1)) )

#define RAW_BUF_JOIN2(x, y) x##y
#define RAW_BUF_JOIN(x, y) RAW_BUF_JOIN2(x, y)
#define RAW_BUF_JOIN0(x, y) RAW_BUF_JOIN(x, y)

#define RAW_BUF_JOIN3(x, y, z) RAW_BUF_JOIN0(RAW_BUF_JOIN(x, y),z)

#define RAW_BUF_STRING2(x) #x
#define RAW_BUF_STRING(x) RAW_BUF_STRING2(x)


namespace rawbuf{

    template <rawbuf_int N>
    struct n_int{
        rawbuf_int _[N];
    };
    typedef n_int<2> two_int;

    template<typename T> two_int is_struct_tester(rawbuf_int T::*);
    template<typename T> char is_struct_tester(...);

    template<typename T, rawbuf_int N> two_int is_array_tester(T (*)[N]);
    char is_array_tester(...);

    template<typename T> two_int is_rawbuf_struct_tester(typename T::rawbuf_struct_type*);
    template<typename T> char is_rawbuf_struct_tester(...);

    template <typename T>
    struct is_rawbuf_struct{
        static const bool result = sizeof(is_rawbuf_struct_tester<T>(0)) != sizeof(char);
    };

    template <typename T>
    struct is_raw_struct{
        static const bool result = sizeof(is_struct_tester<T>(0)) != sizeof(char);
    };

    template <rawbuf_uint size, rawbuf_uint moder = (size % 2)>
    struct is_rawbuf_power2{
        static const bool result = false;
    };

    template <rawbuf_uint size>
    struct is_rawbuf_power2<size, 0>: public is_rawbuf_power2<size / 2>{
    };

    template <rawbuf_uint moder >
    struct is_rawbuf_power2<1, moder>{
        static const bool result = true;
    };

    template <rawbuf_uint lhs, rawbuf_uint rhs>
    struct rawbuf_comp{
        static const bool is_equal = (lhs == rhs);
        static const rawbuf_uint greater   = (lhs < rhs) ? rhs : lhs;
        static const rawbuf_uint less      = (lhs > rhs) ? rhs : lhs;
    };

    template<unsigned int N>
    struct struct_int : struct_int<N - 1> {};
    template<>
    struct struct_int<0> {};

    enum ctrl_cmd{
        allocate = 0
    };
#define RAW_BUF_INDEXER(counter) (sizeof(*counter((rawbuf::struct_int<MAX_MEMBER_COUNT>*)0)) - sizeof(*counter((void*)0)))

#define RAW_BUF_INCREASER(counter, name)  static const int name = RAW_BUF_INDEXER(counter); \
    static char (*counter(rawbuf::struct_int<sizeof(*counter((void*)0)) + name + 1>*))[sizeof(*counter((void*)0)) + name + 1]
}

#endif
