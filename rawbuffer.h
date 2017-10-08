#if !defined(ADD_IMPL_INIT)


#include "rawbuffer_util.h"
#include "rawbuffer_alignment.h"
#include "rawbuffer_allocator.h"

#include <stddef.h>

struct rawbuf_immutable_config{
    const static int field_index_begin = 1;
};

struct rawbuf_config : public rawbuf_immutable_config{
    typedef rawbuf_uint32 offset_type;
    typedef rawbuf_uint32 array_count_type;
};

struct rawbuf_cmd : public rawbuf_config{
    
    enum alloc_cmd{
        alloc = 0
    };

    enum visit_array_cmd{
        get_count = 0
    };

    enum check_cmd{
        check_packet = 0
    };

    //visit_c_string_cmd
    typedef int c_str;

    //We can "reference" a packet with higher address.
    typedef int add_ref;
};

template<typename T, typename M>
inline M* rawbuf_get_array_pointer(typename T::array_count_type& the_array_count, const T* that, typename T::offset_type filed_tag ){
    if(that->_.real_optional_fields_count > filed_tag ){
        const typename T::offset_type *foffset = that->_.field_offset + 1 + filed_tag; 
        if(*foffset != 0){ 
            return (M*)RAW_BUF_ALIGN_TYPE((size_t)(rawbuf::rawbuf_varint<typename T::array_count_type>::decode\
                (the_array_count, ((const char*)foffset) + *foffset)), rawbuf::rawbuf_alignment<ARGS_LIST(M)>::result, size_t);
        }
    }
    the_array_count = 0;
    return 0;
} 

template<typename T, typename M>
inline M* rawbuf_get_array_pointer(const T* that, typename T::offset_type filed_tag) {
    if (that->_.real_optional_fields_count > filed_tag) {
        const typename T::offset_type *foffset = that->_.field_offset + 1 + filed_tag;
        if (*foffset != 0) {
            const unsigned char* buf = ((const unsigned char*)foffset) + *foffset;
            size_t max_count = sizeof(typename T::array_count_type) + 1;
            while (max_count != 0 && *buf++ > (unsigned char)(1 << 7)) {
                --max_count;
            }
            return (M*)RAW_BUF_ALIGN_TYPE((size_t)buf, rawbuf::rawbuf_alignment<ARGS_LIST(M)>::result, size_t);
        }
    }
    return 0;
}


template<typename T>
inline typename T::array_count_type* rawbuf_get_array_count_pointer(const T* that, typename T::offset_type filed_tag) {
    if (that->_.real_optional_fields_count > filed_tag) {
        const typename T::offset_type *foffset = that->_.field_offset + 1 + filed_tag;
        if (*foffset != 0) {
            return (typename T::array_count_type*)((char*)foffset + *foffset);
        }
    }
    return 0;
}

#include "rawbuffer_interface.h"
#include "rawbuffer_output_template.h"

#endif