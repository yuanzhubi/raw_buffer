#if !defined(DECL_IMPL_INIT)

#include "rawbuffer_util.h"
#include "rawbuffer_alignment.h"
#include "rawbuffer_header.h"
#include "rawbuffer_required.h"
#include "rawbuffer_output_template.h"

#define DEF_STRUCT_BEGIN(name) \
struct name{ \
    typedef name rawbuf_struct_type; \
    typedef rawbuf::top_header header_type; \
    static char (*member_counter(...))[1]; \
    static char (*required_counter(...))[1]; \
    static char (*alignment_counter(...))[4]; \
    template<typename T, rawbuf_int N = -1> \
    struct required_members_iterator{}; \
    template<typename T, rawbuf_int N = -1, bool is_define_finished = true> \
    struct members_iterator{ \
        static const rawbuf_uint sizer = 0; \
        static const rawbuf_uint aligned_1x = 0; \
        static const rawbuf_uint aligned_2x = 0; \
        static const rawbuf_uint aligned_4x = 0; \
        static const rawbuf_uint aligned_8x = 0; \
        static const rawbuf_uint aligned_16x = 0; \
    };


#define DEF_STRUCT_END(name) \
    static const rawbuf_uint field_count = RAW_BUF_INDEXER(member_counter); \
    static const rawbuf_uint required_field_count = RAW_BUF_INDEXER(required_counter); \
    static const rawbuf_uint alignment = sizeof(rawbuf_uint); \
    typedef members_iterator<rawbuf_int, field_count - 1> prev_type; \
    static const rawbuf_uint sizer = prev_type::sizer + 4*(field_count + 1); \
    static const rawbuf_uint aligned_1x = prev_type::aligned_1x; \
    static const rawbuf_uint aligned_2x = prev_type::aligned_2x; \
    static const rawbuf_uint aligned_4x = prev_type::aligned_4x + 1;\
    static const rawbuf_uint aligned_8x = prev_type::aligned_8x; \
    static const rawbuf_uint aligned_16x = prev_type::aligned_16x; \
    rawbuf_uint this_offset; \
    rawbuf_uint field_offset[field_count]; \
\
    template <typename T> \
    struct required_members_iterator<T, required_field_count>{ \
        template<typename This> \
        static const char* check(const This& ){ \
            return 0; \
        } \
        template<typename This, typename Func> \
        static void iterator(This &, Func& ){ \
        } \
    }; \
    const char* check_required() const{ \
        return required_members_iterator<rawbuf_int, 0>::check(*this); \
    } \
    template< typename Func> \
    void iterate_required(Func& func_instance) const{ \
        required_members_iterator<rawbuf_int, 0 >::iterator(*this, func_instance); \
    } \
\
    template<typename T> \
    struct members_iterator<T, RAW_BUF_INDEXER(member_counter)>{ \
        template<typename This, typename M> \
        static void output(const This& this_instance, M& ost, const char* begin, const char* sep, const char* end){ \
        } \
        template<typename M> \
        static void copy(const M& src, M& dest){ \
        } \
        template<typename This, typename Func> \
        static void iterator(This &, Func& ){ \
        } \
    }; \
    template<typename M> \
    void output(M& ost, const char* begin, const char* sep, const char* end ){ \
        ost << begin; \
        members_iterator<rawbuf_int, 0>::output(*this, ost, begin, sep, end); \
        ost << end; \
    } \
    template<typename Func> \
    void iterate_all(Func& func_instance) { \
        members_iterator<rawbuf_int, 0 >::iterator(*this, func_instance); \
    } \
};

#define DECL_FIELD(is_required, input_type, name) \
    DECL_IMPL_INIT(is_required, input_type, name, 1, _member) \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), true > : public members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
        template<typename This, typename Func> \
        static void iterator(This& this_instance, Func& func_instance){ \
            func_instance(this_instance, this_instance.name()); \
            next_type::iterator(this_instance, func_instance); \
        } \
    }; \
    template <typename M> \
    void name(const M& src) { \
        rawbuf::top_header* that_ptr= (rawbuf::top_header* )((char*)this - this->this_offset); \
        const input_type& new_src = src; \
        that_ptr->set_value(this->field_offset + RAW_BUF_JOIN(rawbuf_tag_, name), &new_src); \
    }

#define DECL_STRUCT_FIELD(is_required, input_type, name) \
    DECL_IMPL_INIT(is_required, input_type, name, 1, _struct) \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), true > : public members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
        RAW_BUF_OUPUT_TEMPLATE_STRUCT(is_required, input_type, name); \
        template<typename This, typename Func> \
        static void iterator(This& this_instance, Func& func_instance){ \
            input_type* instance = this_instance.name(); \
            func_instance(this_instance, instance); \
            if(instance != 0){ \
                instance->iterate_all(func_instance); \
            }\
            next_type::iterator(this_instance, func_instance); \
        } \
    }; \
    template <rawbuf::ctrl_cmd cmd> \
    void name() { \
        if(cmd == rawbuf::allocate){ \
            rawbuf::top_header* that_ptr= (rawbuf::top_header* )((char*)this - this->this_offset);\
            rawbuf_uint *offset_pos = this->field_offset + RAW_BUF_JOIN(rawbuf_tag_, name); \
            that_ptr->set_value(this->field_offset + RAW_BUF_JOIN(rawbuf_tag_, name), (input_type*)0);\
            input_type* my_ptr = (input_type*)(((char*)that_ptr) + (*offset_pos)); \
            rawbuf_uint *dest = my_ptr->field_offset; \
            for(rawbuf_uint i = 0; i < input_type::field_count; ++i){ \
                dest[i] = 0; \
            } \
            my_ptr->this_offset = *offset_pos; \
        } \
    } \
    template <typename M> \
    void name(const M& src) { \
        input_type* dest = this->name() ; \
        const input_type& new_src = src; \
        if(dest == 0){ \
            rawbuf::top_header* that_ptr= (rawbuf::top_header* )((char*)this - this->this_offset);\
            that_ptr->set_value(this->field_offset + RAW_BUF_JOIN(rawbuf_tag_, name), (input_type*)0);\
            dest = this->name() ; \
        } \
        input_type::members_iterator<rawbuf_int, 0>::copy(new_src, *dest); \
    }

#define DECL_IMPL_INIT(is_required, input_type, name, count, is_member) \
    RAW_BUF_INCREASER(member_counter, RAW_BUF_JOIN(rawbuf_tag_, name) ); \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 > prev_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
        static const rawbuf_uint sizer         = rawbuf::rawbuf_property<input_type>::size_result + prev_type::sizer;\
        static const rawbuf_uint aligned_1x    = prev_type::aligned_1x + rawbuf::rawbuf_property<input_type>::aligned_1x; \
        static const rawbuf_uint aligned_2x    = prev_type::aligned_2x + rawbuf::rawbuf_property<input_type>::aligned_2x; \
        static const rawbuf_uint aligned_4x    = prev_type::aligned_4x + rawbuf::rawbuf_property<input_type>::aligned_4x; \
        static const rawbuf_uint aligned_8x    = prev_type::aligned_8x + rawbuf::rawbuf_property<input_type>::aligned_8x; \
        static const rawbuf_uint aligned_16x   = prev_type::aligned_16x + rawbuf::rawbuf_property<input_type>::aligned_16x; \
        RAW_BUF_OUPUT_TEMPLATE(is_required, input_type, name); \
        template<typename M> \
        static void copy(const M& src, M& dest){ \
            const input_type* ptr = src.name(); \
            if(ptr != 0){ \
                dest.name(*ptr); \
            } \
            next_type::copy(src, dest); \
        } \
    }; \
    RAW_BUF_JOIN3(RAW_BUF_REQUIRE_CHECK_, is_required, is_member)(input_type, name, count) \
    input_type* name() const { \
        rawbuf_uint foffset = this->field_offset[RAW_BUF_JOIN(rawbuf_tag_, name)]; \
        if(foffset != 0){ \
             return (input_type*)(((char*)this - this->this_offset) + foffset); \
        }\
        return 0; \
    }

#endif
