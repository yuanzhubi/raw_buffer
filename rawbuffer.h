#if !defined(ADD_IMPL_INIT)


#include "rawbuffer_util.h"
#include "rawbuffer_alignment.h"
#include "rawbuffer_allocator.h"

#include <stddef.h>

struct rawbuf_immutable_config {
    static const size_t field_index_begin = 1;
};

struct rawbuf_config : public rawbuf_immutable_config {
    typedef rawbuf_uint32 offset_type;
    typedef rawbuf_uint32 array_count_type;
};

struct rawbuf_cmd : public rawbuf_config {

    enum alloc_cmd {
        alloc = 0
    };

    enum visit_array_cmd {
        get_count = 0
    };

    enum check_cmd {
        check_packet = 0
    };
    
    typedef int check_graph;

    //visit c_string command.
    typedef int c_str;

    //We can "reference" a packet with higher address.
    typedef int add_ref;
};

template<typename T, typename M>
inline M rawbuf_get_varint(const T* that, typename T::offset_type filed_tag) {
    if (that->get_optional_fields_count() > filed_tag) {
        const typename T::offset_type *foffset = that->_.field_offset + T::field_index_begin + filed_tag;
        if (*foffset != 0) {
            M result;
            rawbuf::rawbuf_varint<M>::decode(result, ((const char*)foffset) + *foffset);
            return result;
        }
    }
    return 0;
}

template<typename T, typename M>
inline M* rawbuf_get_array_pointer(typename T::array_count_type& the_array_count, const T* that, typename T::offset_type filed_tag, const char* data_end, bool& is_not_out_of_bound) {
    is_not_out_of_bound = true;
    if (that->get_optional_fields_count() > filed_tag) {
        const typename T::offset_type *foffset = that->_.field_offset + T::field_index_begin + filed_tag;
        if (*foffset != 0) {
            const char* pos = ((const char*)foffset) + *foffset;
            if (pos < data_end && pos > (const char*)foffset){
                M* result = (M*)RAW_BUF_ALIGN_TYPE((size_t)(rawbuf::rawbuf_varint<typename T::array_count_type>::decode(the_array_count, pos)), rawbuf::rawbuf_alignment<ARGS_LIST(M)>::result, size_t);
                if (rawbuf::is_rawbuf_struct<M>::result || (result + the_array_count < (M*)data_end && result + the_array_count > result && the_array_count != 0)) {
                    return result;
                }
            }
            is_not_out_of_bound = false;
        }
    }
    the_array_count = 0;
    return 0;
}

template<typename T, typename M>
inline M* rawbuf_get_array_pointer(typename T::array_count_type& the_array_count, const T* that, typename T::offset_type filed_tag) {
    if (that->get_optional_fields_count() > filed_tag) {
        const typename T::offset_type *foffset = that->_.field_offset + T::field_index_begin + filed_tag;
        if (*foffset != 0) {
            return (M*)RAW_BUF_ALIGN_TYPE((size_t)(rawbuf::rawbuf_varint<typename T::array_count_type>::decode\
                (the_array_count, ((const char*)foffset) + *foffset)), rawbuf::rawbuf_alignment<ARGS_LIST(M)>::result, size_t);
        }
    }
    the_array_count = 0;
    return 0;
}

template<typename T, typename M>
inline M* rawbuf_get_array_pointer(const T* that, typename T::offset_type filed_tag) {
    if (that->get_optional_fields_count() > filed_tag) {
        const typename T::offset_type *foffset = that->_.field_offset + T::field_index_begin + filed_tag;
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

#include "rawbuffer_interface.h"
#include "rawbuffer_output_template.h"

#endif