#if !defined(RAW_BUFFER_UTIL_H_)
#define RAW_BUFFER_UTIL_H_

#include "rawbuffer_config.h"

#include <stdlib.h>

#define RAWBUF_EMPTY 

#define RAW_BUF_ALIGN(offset, size)  ( ((offset) + (size) -1 ) & ((~((size) - 1))) )
#define RAW_BUF_ALIGN_TYPE(offset, size, type)  ( ((type)(offset) + (type)(size) - (type)1 ) & ((type)(~((type)(size) - (type)1))) )
#define RAW_BUF_IS_ALIGNED_PTR(ptr, size)  ( (((size_t)ptr) & (size_t(size) - size_t(1))) == 0  )

#define ARGS_LIST(...) __VA_ARGS__

#define RAW_BUF_JOIN2(x, y) x##y
#define RAW_BUF_JOIN(x, y) RAW_BUF_JOIN2(x, y)
#define RAW_BUF_JOIN0(x, y) RAW_BUF_JOIN(x, y)

#define RAW_BUF_JOIN3(x, y, z) RAW_BUF_JOIN0(RAW_BUF_JOIN(x, y),z)

#define RAW_BUF_STRING2(x) #x
#define RAW_BUF_STRING(x) RAW_BUF_STRING2(ARGS_LIST(x))
#define RAW_BUF_ERROR_MSG(x) x " File: " __FILE__ ", line: " RAW_BUF_STRING(__LINE__) "."


#define RAW_BUF_INFO2(input_type, name)  " Type: " RAW_BUF_STRING(ARGS_LIST(input_type)) ", name: " RAW_BUF_STRING(name)  "."
#define RAW_BUF_INFO(input_type, name) RAW_BUF_INFO2(ARGS_LIST(input_type), name)

#define RAW_BUF_OFFSET_OF(s, m)  ((size_t)&(((s *)64)->m) - 64)

namespace rawbuf{

    template <rawbuf_int32 N>
    struct n_int{
        rawbuf_int32 _[N];
    };
    typedef n_int<2> two_int;

    template<typename T> two_int is_castable_tester(T);
    template<typename T> char is_castable_tester(...);

    template<bool result>
    struct result_check;


    template<>
    struct result_check<true>{
        typedef int type;
    };

    template <typename T>
    struct is_num{
        static const bool result = sizeof(is_castable_tester<T>(0)) != sizeof(char);
    };

    template<typename T> two_int is_struct_tester(rawbuf_int32 T::*);
    template<typename T> char is_struct_tester(...);

    template<typename T, rawbuf_int32 N> two_int is_array_tester(T (*)[N]);
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

    template <rawbuf_uint32 size, rawbuf_uint32 moder = (size % 2)>
    struct is_rawbuf_power2{
        static const bool result = false;
    };

    template <rawbuf_uint32 size>
    struct is_rawbuf_power2<size, 0>: public is_rawbuf_power2<size / 2>{
    };

    template <rawbuf_uint32 moder >
    struct is_rawbuf_power2<1, moder>{
        static const bool result = true;
    };

    template <rawbuf_uint32 lhs, rawbuf_uint32 rhs>
    struct rawbuf_comp{
        static const bool is_equal = (lhs == rhs);
        static const rawbuf_uint32 greater   = (lhs < rhs) ? rhs : lhs;
        static const rawbuf_uint32 less      = (lhs > rhs) ? rhs : lhs;
    };

    template <typename lhs, typename rhs, bool lhs_larger = (sizeof(lhs) > sizeof(rhs))>
    struct rawbuf_max_type{
        typedef lhs type;
    };

    template <typename lhs, typename rhs>
    struct rawbuf_max_type<lhs, rhs, false>{
        typedef rhs type;
    };

    template <typename lhs, typename rhs>
    struct rawbuf_type_equal{
        static const bool result = false;
    };

    template <typename lhs>
    struct rawbuf_type_equal<lhs, lhs>{
        static const bool result = true;
    };

    template <unsigned int N>
    struct struct_int : struct_int<N - 1> {};
    template<>
    struct struct_int<0> {};

    template <typename T>
    struct rawbuf_unsigned;

    template <>
    struct rawbuf_unsigned<char> {
        typedef unsigned char type;
    };

    template <>
    struct rawbuf_unsigned<short> {
        typedef unsigned short type;
    };

    template <>
    struct rawbuf_unsigned<int> {
        typedef unsigned int type;
    };

    template <>
    struct rawbuf_unsigned<long> {
        typedef unsigned long type;
    };

    template <>
    struct rawbuf_unsigned<long long> {
        typedef unsigned long long type;
    };

#define RAW_BUF_INDEXER(counter) (sizeof(*counter((rawbuf::struct_int<MAX_FIELDS_COUNT>*)0)) - sizeof(*counter((void*)0)))

#define RAW_BUF_INCREASER(counter, name)  static const int name = RAW_BUF_INDEXER(counter); \
    static char (*counter(rawbuf::struct_int<sizeof(*counter((void*)0)) + name + 1>*))[sizeof(*counter((void*)0)) + name + 1]
}

#endif
