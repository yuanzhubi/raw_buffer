#if !defined(RAW_BUFFER_ALIGNMENT_H_)
#define RAW_BUFFER_ALIGNMENT_H_

#include "rawbuffer_util.h"

namespace rawbuf{

    template <rawbuf_int aligned_1x, rawbuf_int aligned_2x, rawbuf_int aligned_4x, rawbuf_int aligned_8x,
                /*rawbuf_int aligned_16x,*/ rawbuf_int offset > //16bytes alignment was not supported.
    struct max_align_cost{
        //either (padding)  min, max, min, max, min, max .....or (padding)  max, min, max, min, max,min ..... layout cost the most spaces.
        //For example:  char(1), long double(16), char(1), long long(8), short(2), int(4),,,,,int(4), short(2), int(4), int(4)
        //The padding cost for alignment is padding and sum of (max-min) = (16-1) + (8-1) + (4-2)...

        //we assume the data is saved in address % 16 = offset. So padding = max - offset  at most

        static const rawbuf_int sum1  = aligned_1x;
        static const rawbuf_int sum2  = aligned_2x + aligned_1x;
        static const rawbuf_int sum4  = aligned_4x + aligned_2x + aligned_1x;
        static const rawbuf_int sum8  = aligned_8x + aligned_4x + aligned_2x + aligned_1x;

        static const rawbuf_int max_sum  = sum8;
        static const rawbuf_int padding = (
                                             ((aligned_8x != 0 && 8 > offset) ? (8 - offset)  :
                                              ((aligned_4x != 0 && 4 > offset) ? (4 - offset)  :
                                               ((aligned_2x != 0 && 2 > offset) ? (2 - offset)  :
                                                0
                                            ))));

        //half of the align should be minus weighted add.
        static const rawbuf_int half_total_count  = max_sum/2;
        static const rawbuf_int mod2  = max_sum%2;

        static const rawbuf_int result = padding + (
                            (half_total_count <= sum1) ? (1*(sum1 - 2*half_total_count - mod2)        + aligned_2x*2 + aligned_4x*4 + aligned_8x*8 ):(
                            (half_total_count <= sum2) ? (2*(sum2 - 2*half_total_count + sum1 - mod2) - aligned_1x*1 + aligned_4x*4 + aligned_8x*8 ):(
                            (half_total_count <= sum4) ? (4*(sum4 - 2*half_total_count + sum2 - mod2) - aligned_1x*1 - aligned_2x*2 + aligned_8x*8 ):(
                                                         (8*(sum8 - 2*half_total_count + sum4 - mod2) - aligned_1x*1 - aligned_2x*2 - aligned_4x*4 )
                                                ))));
    };

    template <typename T, bool is_rawbuf_struct_result = is_rawbuf_struct<T>::result>
    struct rawbuf_property{
        static const rawbuf_uint alignment_result = (rawbuf_uint)T::alignment;
        static const rawbuf_uint size_result = T::sizer + sizeof(T);
        static const rawbuf_uint aligned_1x = T::aligned_1x;
        static const rawbuf_uint aligned_2x = T::aligned_2x;
        static const rawbuf_uint aligned_4x = T::aligned_4x;
        static const rawbuf_uint aligned_8x = T::aligned_8x;
    };

    template <rawbuf_int N, bool checker = is_rawbuf_power2<N>::result >
    struct alignment_hint;

    template <rawbuf_int N>
    struct alignment_hint<N, true> {
        static const rawbuf_int result = ((N > 8) ? 8 : N);
    };

    template<rawbuf_uint N> n_int<alignment_hint<N, true>::result> alignment_hint_tester(alignment_hint<N, true>*);
    char alignment_hint_tester(...);

    template <typename T>
    struct rawbuf_property<T, false>{
        static const rawbuf_uint type_size = sizeof(T);
        static const rawbuf_uint alignment_result = (!is_raw_struct<T>::result) ?
        (
            is_rawbuf_power2<type_size>::result ?
            type_size :    //
            8              //We do not recommend you use the basic type that is not power of two(not portable).
        ) :
        (
            (sizeof(alignment_hint_tester((T*)0)) != sizeof(char)) ?
            (sizeof(alignment_hint_tester((T*)0)) / sizeof(rawbuf_int)) : // The struct is subclass of alignment_hint<N>
            (   //your struct does not provide alignment_hint, so we have to guess from the size.
                (type_size >= 8) ? 8 :
                (type_size >= 4) ? 4 :
                (type_size >= 2) ? 2 : 1
            )
        );
        static const rawbuf_uint size_result = type_size;
        static const rawbuf_uint aligned_1x = ((alignment_result == 1) ? 1: 0);
        static const rawbuf_uint aligned_2x = ((alignment_result == 2) ? 1: 0);
        static const rawbuf_uint aligned_4x = ((alignment_result == 4) ? 1: 0);
        static const rawbuf_uint aligned_8x = ((alignment_result == 8) ? 1: 0);
    };

    template <typename T>
    struct rawbuf_property<T*, false>{
        //do not user pointer type!
    };
}

#endif
