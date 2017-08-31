#if !defined(ADD_IMPL_INIT)

#include "rawbuffer_util.h"
#include "rawbuffer_alignment.h"
#include "rawbuffer_allocator.h"
#include "rawbuffer_output_template.h"

#include <stddef.h>

struct rawbuf_config{
    typedef rawbuf_uint offset_type;
	typedef rawbuf_uint array_count_type;
    const static rawbuf_uint fields_alignment =  rawbuf::rawbuf_property<offset_type>::alignment_result; // The real alignment_size will be max(rawbuf::rawbuf_property<offset_type>::alignment_result, alignment_size). You can redefine both of these property in your packet definition.
};

struct rawbuf_cmd{
    
	enum alloc_cmd{
        alloc = 0
    };

    enum visit_array_cmd{
        length = 0,
		size = 1,
		array_length = 2,
		array_size = 3,
		get_size = 4, 
		get_array_size = 5 //Either is ok. You can choose any name. The author can not satisify all the users..
    };

	//visit_c_string_cmd
	typedef int c_str;
};

#define DEF_PACKET_BEGIN(name) \
struct name : public rawbuf_config, public rawbuf_cmd{ \
public:\
    typedef name rawbuf_struct_type; \
    static char (*member_counter(...))[1]; \
    static char (*required_counter(...))[1]; \
    static char (*optional_counter(...))[1]; \
    template<typename T, rawbuf_int N = -1, bool is_define_finished = true> \
    struct members_iterator{ \
        static const rawbuf_uint sizer = 0; \
        static const rawbuf_uint aligned_1x = 0; \
        static const rawbuf_uint aligned_2x = 0; \
        static const rawbuf_uint aligned_4x = 0; \
        static const rawbuf_uint aligned_8x = 0; \
		static const rawbuf_uint required_alignment = rawbuf::rawbuf_property<offset_type>::alignment_result; \
    }; \
    template<typename T, rawbuf_int N = T::fields_count - 1> \
    struct rawbuf_writer_helper : public rawbuf_writer_helper<T, N-1>{\
    }; \
    template<typename T> \
    struct rawbuf_writer_helper<T, -1> : public rawbuf_writer<T>{\
    }; \
	template<typename T, rawbuf_int N = T::fields_count - 1> \
    struct rawbuf_reader_helper : public rawbuf_reader_helper<T, N-1>{\
        bool valid(){return true;} \
    }; \
    template<typename T> \
	struct rawbuf_reader_helper<T, -1> : public rawbuf_reader<T>{\
        bool valid(){return ((rawbuf_reader_helper<T, 0>&)(*this)).valid();} \
    }; \

#define DEF_PACKET_END(name) \
public: \
	static const rawbuf_uint fields_count = RAW_BUF_INDEXER(member_counter); \
    static const rawbuf_uint optional_fields_count = RAW_BUF_INDEXER(optional_counter); \
	static const rawbuf_uint required_fields_count = RAW_BUF_INDEXER(required_counter); \
    typedef members_iterator<rawbuf_struct_type, fields_count - 1> prev_type; \
	static const rawbuf_uint alignment = fields_alignment  > prev_type::required_alignment ? fields_alignment : prev_type::required_alignment; \
	union{ \
		offset_type real_optional_fields_count; \
		offset_type field_offset[1 + optional_fields_count]; \
	}_;\
    static const rawbuf_uint sizer = prev_type::sizer; \
    static const rawbuf_uint aligned_1x = prev_type::aligned_1x + (alignment == 1 ? 0:1); \
    static const rawbuf_uint aligned_2x = prev_type::aligned_2x + (alignment == 2 ? 0:1); \
    static const rawbuf_uint aligned_4x = prev_type::aligned_4x + (alignment == 4 ? 0:1); \
    static const rawbuf_uint aligned_8x = prev_type::aligned_8x + (alignment == 8 ? 0:1); \
\
    template<typename T> \
    struct members_iterator<T, RAW_BUF_INDEXER(member_counter), true>{ \
		static const bool is_required = false; \
        template<typename This, typename M> \
        static void output(const This&, M&, int depth){ \
        } \
        template<typename M> \
        static void copy(const M& , M& , rawbuf_writer_proto& ){ \
        } \
        template<typename This, typename Func> \
        static void iterate(This &, Func& ){ \
        } \
        template<typename This, typename Func> \
        static void iterate_with_name(This &,Func&){ \
        } \
        template<typename This, typename Func> \
        static void iterate_with_depth(This &, Func&, int){ \
        } \
        template<typename M> \
        static void copy(const M& , const rawbuf_writer<T>&){ \
        } \
    }; \
    template<typename M> \
    void output(M& ost){ \
        rawbuf::json_output<M> output_functor(ost); \
        members_iterator<rawbuf_struct_type, 0>::iterate_with_depth(*this, output_functor, 0); \
    } \
    template<typename Func> \
    void iterate(Func& func_instance) { \
		members_iterator<rawbuf_struct_type, 0 >::iterate(*this, func_instance); \
    } \
    template<typename Func> \
    void iterate_with_name(Func& func_instance) { \
        members_iterator<rawbuf_struct_type, 0 >::iterate_with_name(*this, func_instance); \
    } \
    template<typename Func> \
    void iterate_with_depth(Func& func_instance, int depth = 0) { \
        members_iterator<rawbuf_struct_type, 0 >::iterate_with_depth(*this, func_instance, depth); \
    } \
};

#define ADD_FIELD(input_type, name) \
public:\
    ADD_IMPL_INIT(input_type, name, 1) \
	RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, name) ); \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), true > : public members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
        template<typename This, typename Func> \
        static void iterate(This& this_instance, Func& func_instance){ \
            func_instance(this_instance.name()); \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_name(This& this_instance, Func& func_instance){ \
            func_instance(this_instance.name(), #name); \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate_with_name(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_depth(This& this_instance, Func& func_instance, int depth){ \
            func_instance(this_instance.name(), #name, depth); \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate_with_depth(this_instance, func_instance, depth); \
			}\
        } \
		template<typename M> \
        static void copy(const M& the_src, const rawbuf_writer<T>& packet){ \
            const T& src = the_src; \
            const input_type* ptr = src.name(); \
            if(ptr != 0){ \
                packet->name(*ptr); \
            } else{ \
                T* that = packet(); \
                that->_.field_offset[1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name)] = 0; \
            } \
            next_type::copy(src, packet); \
        } \
    }; \
    input_type* name() const { \
        if(this->_.real_optional_fields_count > (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){ \
            const offset_type *foffset = this->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset != 0 ){ \
                return (input_type*)((char*)foffset + *foffset ); \
            }\
        }\
        return 0; \
    } \
    template<typename T> \
    struct rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
        template <typename M> \
        input_type* name(const M& the_src) { \
            const input_type& src = the_src; \
            T* that = *this; \
            typename T::offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset != 0){ \
               *(input_type*)((char*)foffset + *foffset ) = src; \
               return (input_type*)((char*)foffset + *foffset ); \
            }\
            return this->writer->append(&src, foffset); \
        } \
    }; \
	template<typename T> \
    struct rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
        input_type* name() { \
            T* that = *this; \
            typedef typename T::offset_type offset_type; \
			if(that->_.real_optional_fields_count <= (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){return 0;}            /*   The field was not known by the creator.*/\
			offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset == 0) return 0;																	    /*	 The field was not assigned.*/\
            input_type* result = (input_type*)((char*)foffset + *foffset ); \
            if(this->begin() >= (char*)(result+1) || this->end() < (char*)(result + 1)){				    /* 1.The offset should not be out of bound.*/\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            } \
            if(!RAW_BUF_IS_ALIGNED_PTR(result, rawbuf::rawbuf_property<input_type>::alignment_result))	    /* 2.The location of the optional field should be aligned.*/{	\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field is not aligned!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            }\
			return result; \
        } \
        bool valid(){ \
            this->name(); \
            return (bool(*this)) && ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).valid(); \
        } \
    };\

#define ADD_FIELD_REQUIRED(input_type, name) \
public:\
    ADD_IMPL_INIT(input_type, name, 0) /*only optional field needs it to sum size.*/\
	RAW_BUF_INCREASER(required_counter, RAW_BUF_JOIN(rawbuf_tag_required_, name) ); \
	input_type RAW_BUF_JOIN(rawbuf_data_required_, name); \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), true > : public members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
		typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
		typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false > parent_type; \
        template<typename This, typename Func> \
        static void iterate(This& this_instance, Func& func_instance){ \
            func_instance(this_instance.name()); \
            next_type::iterate(this_instance, func_instance); \
        } \
        template<typename This, typename Func> \
        static void iterate_with_name(This& this_instance, Func& func_instance){ \
            func_instance(this_instance.name(), #name); \
            next_type::iterate_with_name(this_instance, func_instance); \
        } \
        template<typename This, typename Func> \
        static void iterate_with_depth(This& this_instance, Func& func_instance, int depth){ \
            func_instance(this_instance.name(), #name, depth); \
            next_type::iterate_with_depth(this_instance, func_instance, depth); \
        } \
		template<typename M> \
        static void copy(const M& the_src, const rawbuf_writer<T>& packet){ \
            const T& src = the_src; \
            packet()->RAW_BUF_JOIN(rawbuf_data_required_, name) = src.RAW_BUF_JOIN(rawbuf_data_required_, name); \
            next_type::copy(src, packet); \
        } \
		static const rawbuf_uint required_alignment = (parent_type::required_alignment > rawbuf::rawbuf_property<input_type>::alignment_result)  ?  parent_type::required_alignment : rawbuf::rawbuf_property<input_type>::alignment_result; \
    }; \
    inline input_type* name() const { \
        return (input_type*)(&(this->RAW_BUF_JOIN(rawbuf_data_required_, name))); \
    } \
	template <typename M> \
	inline void name(const M& src)  { \
        this->RAW_BUF_JOIN(rawbuf_data_required_, name) = src; \
    } \
    template<typename T> \
    struct rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
        template <typename M> \
        input_type* name(const M& the_src) const{ \
            const input_type& src = the_src; \
            T* that = *this; \
			that->RAW_BUF_JOIN(rawbuf_data_required_, name) = src; \
			return &(that->RAW_BUF_JOIN(rawbuf_data_required_, name)); \
        } \
    };\
	template<typename T> \
    struct rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
        input_type* name() const{ \
            T* that = *this; \
			return that->name(); \
        } \
		bool valid(){ \
            return ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).valid(); \
        } \
    };\

#define ADD_FIELD_VECTOR(input_type, name, size_estimation) \
    ADD_IMPL_INIT(input_type, name, size_estimation) \
	RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, name) ); \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), true > : public members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef typename T::offset_type offset_type; \
		typedef typename T::array_count_type array_count_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false > parent_type; \
        static const rawbuf_uint sizer = parent_type::sizer + sizeof(array_count_type); \
        static const rawbuf_uint aligned_1x = parent_type::aligned_1x + (rawbuf::rawbuf_property<array_count_type>::alignment_result == 1 ? 0:1); \
        static const rawbuf_uint aligned_2x = parent_type::aligned_2x + (rawbuf::rawbuf_property<array_count_type>::alignment_result == 2 ? 0:1); \
        static const rawbuf_uint aligned_4x = parent_type::aligned_4x + (rawbuf::rawbuf_property<array_count_type>::alignment_result == 4 ? 0:1); \
        static const rawbuf_uint aligned_8x = parent_type::aligned_8x + (rawbuf::rawbuf_property<array_count_type>::alignment_result == 8 ? 0:1); \
        template<typename This, typename Func> \
        static void iterate(This& this_instance, Func& func_instance){ \
            array_count_type *psize = this_instance.name<rawbuf_struct_type::array_size>() ; \
			if(psize != 0) {\
                input_type* data = (input_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
                if(func_instance(data, *psize)){ \
                    for(array_count_type i = 0; i < *psize; ++i){ \
                        func_instance(data + i); \
                    }\
                } \
            }else{ \
                func_instance((input_type*)0, 0); \
            } \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_name(This& this_instance, Func& func_instance){ \
            array_count_type *psize = this_instance.name<rawbuf_struct_type::array_size>() ; \
			if(psize != 0) {\
                input_type* data = (input_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
                if(func_instance(data, *psize, #name)){ \
                    for(array_count_type i = 0; i < *psize; ++i){ \
                        func_instance(data + i, 0); \
                    }\
                } \
            }else{ \
                func_instance((input_type*)0, 0, #name); \
            } \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate_with_name(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_depth(This& this_instance, Func& func_instance, int depth){ \
            array_count_type *psize = this_instance.name<rawbuf_struct_type::array_size>() ; \
			if(psize != 0) {\
                input_type* data = (input_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
                if(func_instance(data, *psize, #name, depth)){ \
                    for(array_count_type i = 0; i < *psize; ++i){ \
                        func_instance(data + i, 0, depth + 1); \
                    }\
                } \
            }else{ \
                func_instance((input_type*)0, 0, #name, depth); \
            } \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate_with_depth(this_instance, func_instance, depth); \
			}\
        } \
		template<typename M> \
        static void copy(const M& the_src, const rawbuf_writer<T>& packet){ \
            const T& src = the_src; \
			array_count_type* psize = src->name<rawbuf_struct_type::array_size>() ; \
			if(psize != 0 && *psize != 0 ){ \
				 input_type* ptr = (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), \
							rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
				 packet->name(ptr, *psize); \
            } else{ \
                T* that = packet; \
                that->_.field_offset[1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name)] = 0; \
            } \
            next_type::copy(src, packet); \
        } \
    }; \
    input_type* name() const{ \
		array_count_type *psize = this->name<rawbuf_struct_type::array_size>() ; \
        if(psize != 0 && *psize != 0 ){ \
            return (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), \
                        rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
        }\
        return 0; \
    } \
	template<typename M> /*M can only be c_str*/ \
    input_type* name() const{ \
		array_count_type *psize = this->name<rawbuf_struct_type::array_size>() ; \
        if(psize != 0 && *psize != 0 ){ \
			input_type* result = (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), \
                        rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
            ((char*)(result + *psize))[-1] = '\0'; \
			return result; \
        }\
        return 0; \
    } \
    template <rawbuf_struct_type::visit_array_cmd cmd> \
    array_count_type* name() const{ \
		if(this->_.real_optional_fields_count > (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){ \
			const offset_type *foffset = this->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			if(*foffset != 0){ \
				return (array_count_type*)((char*)foffset + *foffset); \
			}\
		} \
        return 0; \
    } \
    template<typename T> \
    struct rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
		typedef typename T::array_count_type array_count_type; \
        template <typename M> \
        input_type* name(const M* the_src, array_count_type the_array_size) { \
            const input_type* src = the_src; \
            T* that = *this; \
            typename T::offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset != 0){ \
                array_count_type *old_size = (array_count_type*)((char*)foffset + *foffset); \
                if(*old_size > the_array_size){ \
                    *old_size = the_array_size; \
                    return (input_type*)memcpy((input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(old_size + 1)), rawbuf::rawbuf_property<input_type>::alignment_result, size_t)) \
                                  , src, the_array_size * sizeof(input_type)); \
                } \
            }\
            return this->writer->append(src, the_array_size, foffset); \
        } \
        template <typename M, array_count_type the_array_size> \
        input_type* name(const M (&the_src)[the_array_size]) { \
			return this->name(the_src, the_array_size); \
        } \
        template <rawbuf_struct_type::alloc_cmd cmd> \
        input_type* name(array_count_type the_array_size) { \
            T* that = *this; \
            typename T::offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            input_type* result = this->writer->append(0, the_array_size, foffset); \
            if(sizeof(input_type) == 1) { \
                *((char*)result) = 0; \
            }\
            return result; \
        } \
    }; \
	template<typename T> \
    struct rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
		typedef typename T::offset_type offset_type;\
        input_type* name() { \
            array_count_type *psize = this->name<rawbuf_struct_type::array_size>() ; \
            if(psize == 0 || *psize == 0){ \
                return 0; \
            } \
            input_type* result = (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
            input_type* array_end = result + *psize; \
            if(this->end() < ((char*)(array_end)) || array_end <= result){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array out of bound!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            } \
            return result; \
        } \
		template<typename M> /*M can only be c_str*/ \
		input_type* name() const{ \
			array_count_type *psize = this->name<rawbuf_struct_type::array_size>() ; \
            if(psize == 0 || *psize == 0){ \
                return 0; \
            } \
            input_type* result = (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
            input_type* array_end = result + *psize; \
            if(this->end() < ((char*)(array_end)) || array_end <= result){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array out of bound!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            } \
            ((char*)(result + *psize))[-1] = '\0'; \
			return result; \
		} \
		template <rawbuf_struct_type::visit_array_cmd cmd> \
		array_count_type* name() { \
            T* that = *this; \
            if(that->_.real_optional_fields_count <= (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){return 0;}            /*   The field was not known by the creator.*/\
			offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset == 0){return 0;}																    /*   The optional field was not assigned*/\
            array_count_type *psize = (array_count_type*)((char*)foffset + *foffset ); \
            if( this->begin() >= ((char*)(psize + 1)) || this->end() < ((char*)(psize + 1))){				    /*1. The location of the length of the optional field should not be out of the packet.*/\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            } \
            if(!RAW_BUF_IS_ALIGNED_PTR(psize,rawbuf::rawbuf_property<array_count_type>::alignment_result)){	    /*2. The location of the length of the optional field should be aligned.*/	\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field is not aligned!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            }\
            return psize; \
		} \
		bool valid(){ \
            this->name(); \
            return (bool(*this)) && ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).valid(); \
        } \
    };\

#define ADD_FIELD_VECTOR_REQUIRED(input_type, name, real_size) \
    ADD_IMPL_INIT(input_type, name, 0) \
	RAW_BUF_INCREASER(required_counter, RAW_BUF_JOIN(rawbuf_tag_required_, name) ); \
	input_type RAW_BUF_JOIN(rawbuf_data_required_, name)[real_size]; \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), true > : public members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef typename T::offset_type offset_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false > parent_type; \
        template<typename This, typename Func> \
        static void iterate(This& this_instance, Func& func_instance){ \
			input_type* data = this_instance.RAW_BUF_JOIN(rawbuf_data_required_, name); \
			if(func_instance(data, real_size)){\
				for(size_t i = 0; i < real_size; ++i){ \
                    func_instance(data + i); \
                }\
			}\
            next_type::iterate(this_instance, func_instance); \
        } \
        template<typename This, typename Func> \
        static void iterate_with_name(This& this_instance, Func& func_instance){ \
            input_type* data = this_instance.RAW_BUF_JOIN(rawbuf_data_required_, name); \
			if(func_instance(data, real_size, #name)){\
				for(size_t i = 0; i < real_size; ++i){ \
                    func_instance(data + i, 0); \
                }\
			}\
            next_type::iterate_with_name(this_instance, func_instance); \
        } \
        template<typename This, typename Func> \
        static void iterate_with_depth(This& this_instance, Func& func_instance, int depth){ \
            input_type* data = this_instance.RAW_BUF_JOIN(rawbuf_data_required_, name); \
			if(func_instance(data, real_size, #name, depth)){\
				for(size_t i = 0; i < real_size; ++i){ \
                    func_instance(data + i, 0, depth + 1); \
                }\
			}\
            next_type::iterate_with_depth(this_instance, func_instance, depth); \
        } \
		template<typename M> \
        static void copy(const M& the_src, const rawbuf_writer<T>& packet){ \
            const T& src = the_src; \
            memcpy(packet()->RAW_BUF_JOIN(rawbuf_data_required_, name),  src.RAW_BUF_JOIN(rawbuf_data_required_, name), sizeof(input_type) * real_size) ; \
            next_type::copy(src, packet); \
        } \
		static const rawbuf_uint required_alignment = (parent_type::required_alignment > rawbuf::rawbuf_property<input_type>::alignment_result)  ?  parent_type::required_alignment : rawbuf::rawbuf_property<input_type>::alignment_result; \
    }; \
    input_type* name() { \
       return this->RAW_BUF_JOIN(rawbuf_data_required_, name); \
    } \
	template<typename M> /*M can only be c_str*/ \
    input_type* name() { \
	   ((char*)(this->RAW_BUF_JOIN(rawbuf_data_required_, name) + real_size))[-1] = '\0'; \
       return this->RAW_BUF_JOIN(rawbuf_data_required_, name); \
    } \
    template <rawbuf_struct_type::visit_array_cmd cmd> \
    size_t name() const{ \
        return real_size; \
    } \
    template<typename T> \
    struct rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
        template <typename M> \
		input_type* name(const M* src, size_t the_array_size) { \
			if(the_array_size * sizeof(M) <= real_size * sizeof(input_type)){ \
				return (input_type*)memcpy((*this)()->RAW_BUF_JOIN(rawbuf_data_required_, name),  src, the_array_size * sizeof(M)) ; \
			} \
			return 0; \
        } \
        template <typename M, size_t the_array_size> \
        input_type* name(const M (&the_src)[the_array_size]) { \
            return this->name(the_src, the_array_size); \
		}\
    }; \
	template<typename T> \
    struct rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
        input_type* name() const{ \
            T* that = *this; \
			return that->name(); \
        } \
		bool valid(){ \
            return ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).valid(); \
        } \
    };\

#define ADD_PACKET(input_type, name) \
    ADD_IMPL_INIT(input_type, name, 1) \
	RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, name) ); \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), true > : public members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
        template<typename This, typename Func> \
        static void iterate(This& this_instance, Func& func_instance){ \
            input_type* instance = this_instance.name(); \
            bool result = func_instance(instance); \
            if(instance != 0 && result){ \
                instance->iterate(func_instance); \
            }\
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_name(This& this_instance, Func& func_instance){ \
            input_type* instance = this_instance.name(); \
            bool result = func_instance(instance, #name); \
            if(instance != 0 && result){ \
                instance->iterate_with_name(func_instance); \
            }\
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate_with_name(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_depth(This& this_instance, Func& func_instance, int depth){ \
            input_type* instance = this_instance.name(); \
            bool result = func_instance(instance, #name, depth); \
            if(instance != 0 && result){ \
                instance->iterate_with_depth(func_instance, depth + 1); \
            }\
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate_with_depth(this_instance, func_instance, depth); \
			}\
        } \
		template<typename M> \
        static void copy(const M& the_src, const rawbuf_writer<T>& packet){ \
            const T& src = the_src; \
            const input_type* ptr = src.name(); \
            if(ptr != 0){ \
                packet->name(*ptr); \
            } else{ \
                T* that = packet; \
                that->_.field_offset[ 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name)] = 0; \
            } \
            next_type::copy(src, packet); \
        } \
    }; \
    input_type* name() const { \
        if(this->_.real_optional_fields_count > (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){ \
            const offset_type *foffset = this->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset != 0 ){ \
                return (input_type*)((char*)foffset + *foffset); \
            }\
        }\
        return 0; \
    } \
    template<typename T> \
    struct rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
        typedef  typename T::offset_type offset_type; \
        rawbuf_writer<input_type> name(const input_type& src) { \
            T* that = *this; \
            typename T::offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			size_t offset_offset; \
			if(*foffset == 0){ \
				offset_offset = ((char*)(this->writer->template alloc<input_type, offset_type>(foffset)) - (char*)that); \
			} \
			else{ \
				offset_offset = RAW_BUF_OFFSET_OF(T, _.field_offset) +  (1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name)) * sizeof(*foffset) +  *foffset; \
			} \
            rawbuf_writer<input_type> result; \
            result.writer = this->writer; \
            result.offset = this->offset + offset_offset; \
            input_type::members_iterator<input_type, 0, true>::copy(src, result); \
            return result; \
        } \
        template <rawbuf_struct_type::alloc_cmd cmd> \
        rawbuf_writer<input_type> name() { \
            T* that = *this; \
            typename T::offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			size_t offset_offset; \
			if(*foffset == 0){ \
				offset_offset = ((char*)(this->writer->template alloc<input_type, offset_type>(foffset)) - (char*)that); \
			} \
			else{ \
				offset_offset = RAW_BUF_OFFSET_OF(T, _.field_offset) +  (1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name)) * sizeof(*foffset) +  *foffset; \
			} \
            rawbuf_writer<input_type> result; \
            result.writer = this->writer; \
            result.offset = this->offset + offset_offset; \
            return result; \
        } \
    }; \
	template<typename T> \
    struct rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
        rawbuf_reader<input_type> name() { \
            rawbuf_reader<input_type> real_result;  \
			T* that = *this; \
			typedef typename T::offset_type offset_type; \
			if(that->_.real_optional_fields_count <= (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){real_result.reset();  return real_result;} /*   The field was not known by the creator.*/\
			offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset == 0) {real_result.reset();  return real_result;}																	/*   The field was not assigned.*/\
            input_type* result = (input_type*)((char*)foffset + *foffset ); \
            if(this->begin() >= (char*)(result+1) || this->end() < (char*)(result + 1)){														 /* 1.The offset should not be out of bound.*/\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(input_type, name))); \
				real_result->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(input_type, name))); \
                return real_result; \
            } \
            if(!real_result.init(result, this->end())){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field reader init failed!" RAW_BUF_INFO(input_type, name))); \
            } \
            return real_result; \
        } \
		bool valid(){ \
            rawbuf_reader<input_type> result_further = this->name(); \
			bool local_result = bool(result_further); \
			if(local_result){ \
				local_result = rawbuf_check(result_further); \
			} \
			else{ \
				local_result = bool(*this) ; \
			} \
            return local_result && ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).valid(); \
        } \
    };\

#define ADD_PACKET_VECTOR(input_type, name, size_estimation) \
    ADD_IMPL_INIT(input_type, name, size_estimation) \
	RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, name) ); \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), true > : public members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef typename T::offset_type offset_type; \
		typedef typename T::array_count_type array_count_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false > parent_type; \
        static const rawbuf_uint sizer = parent_type::sizer + sizeof(array_count_type); \
        static const rawbuf_uint aligned_1x = parent_type::aligned_1x + (rawbuf::rawbuf_property<array_count_type>::alignment_result == 1 ? 0:1); \
        static const rawbuf_uint aligned_2x = parent_type::aligned_2x + (rawbuf::rawbuf_property<array_count_type>::alignment_result == 2 ? 0:1); \
        static const rawbuf_uint aligned_4x = parent_type::aligned_4x + (rawbuf::rawbuf_property<array_count_type>::alignment_result == 4 ? 0:1); \
        static const rawbuf_uint aligned_8x = parent_type::aligned_8x + (rawbuf::rawbuf_property<array_count_type>::alignment_result == 8 ? 0:1); \
        template<typename This, typename Func> \
        static void iterate(This& this_instance, Func& func_instance){ \
            array_count_type *psize = this_instance.name<rawbuf_struct_type::array_size>(); \
			if(psize != 0) {\
                input_type* data = (input_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
				if(func_instance(data, *psize)){ \
					size_t data_real_size = sizeof(input_type) + (data->_.real_optional_fields_count - input_type::optional_fields_count) * sizeof(input_type::offset_type); \
					char* pdata = (char*)data; \
                    for(array_count_type i = 0; i < *psize; ++i, pdata+=data_real_size){ \
						((input_type*)pdata)->iterate(func_instance); \
                    }\
                } \
            }else{ \
                func_instance((input_type*)0, 0); \
            } \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_name(This& this_instance, Func& func_instance){ \
            array_count_type *psize = this_instance.name<rawbuf_struct_type::array_size>() ; \
			if(psize != 0) {\
                input_type* data = (input_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
                if(func_instance(data, *psize, #name)){ \
                    size_t data_real_size = sizeof(input_type) + (data->_.real_optional_fields_count - input_type::optional_fields_count) * sizeof(input_type::offset_type); \
					char* pdata = (char*)data; \
                    for(array_count_type i = 0; i < *psize; ++i, pdata+=data_real_size){ \
						((input_type*)pdata)->iterate_with_name(func_instance); \
                    }\
                } \
            }else{ \
                func_instance((input_type*)0, 0, #name); \
            } \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate_with_name(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_depth(This& this_instance, Func& func_instance, int depth){ \
            array_count_type *psize = this_instance.name<rawbuf_struct_type::array_size>() ; \
			if(psize != 0) {\
                input_type* data = (input_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
                if(func_instance(data, *psize, #name, depth)){ \
                    size_t data_real_size = sizeof(input_type) + (data->_.real_optional_fields_count - input_type::optional_fields_count) * sizeof(input_type::offset_type); \
					char* pdata = (char*)data; \
                    for(array_count_type i = 0; i < *psize; ++i, pdata+=data_real_size){ \
						((input_type*)pdata)->iterate_with_depth(func_instance, depth + 1); \
                    }\
                } \
            }else{ \
                func_instance((input_type*)0, 0, #name, depth); \
            } \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate_with_depth(this_instance, func_instance, depth); \
			}\
        } \
		template<typename M> \
        static void copy(const M& the_src, const rawbuf_writer<T>& packet){ \
            const T& src = the_src; \
			array_count_type* psize = src->name<rawbuf_struct_type::array_size>() ; \
			if(psize != 0 && *psize != 0 ){ \
				input_type* ptr = (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), \
							rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
				packet->name(ptr, *psize); \
            } else{ \
                T* that = packet; \
                that->_.field_offset[1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name)] = 0; \
            } \
            next_type::copy(src, packet); \
        } \
    }; \
    input_type* name() const{ \
		array_count_type *psize = this->name<rawbuf_struct_type::array_size>() ; \
        if(psize != 0 && *psize != 0 ){ \
            return (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), \
                        rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
        }\
        return 0; \
    } \
    template <rawbuf_struct_type::visit_array_cmd cmd> \
    array_count_type* name() const{ \
		if(this->_.real_optional_fields_count > (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){ \
			const offset_type *foffset = this->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			if(*foffset != 0){ \
				return (array_count_type*)((char*)foffset + *foffset); \
			}\
		} \
        return 0; \
    } \
    template<typename T> \
    struct rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_writer_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
		typedef typename T::array_count_type array_count_type; \
		typedef typename T::offset_type offset_type; \
        template <typename M> \
        rawbuf_writer_iterator<input_type> name(const M* the_src, array_count_type the_array_size) { \
            rawbuf_writer_iterator<input_type> result; \
			const input_type* src = the_src; \
			if(0 == the_array_size) {\
				result.writer = 0; \
				return result; \
			}\
			result = this->name<alloc>(the_array_size); \
			size_t data_real_size = sizeof(input_type) + (src->_.real_optional_fields_count - input_type::optional_fields_count) * sizeof(input_type::offset_type); \
			char* pdata = (char*)src; \
            for(array_count_type i = 0; i < the_array_size; ++i, pdata+=data_real_size, ++result){ \
				input_type::members_iterator<input_type, 0, true>::copy(*((input_type*)pdata), result); \
            }\
            result -= the_array_size; \
            return result; \
        } \
        template <typename M, array_count_type the_array_size> \
        rawbuf_writer_iterator<input_type> name(const M (&the_src)[the_array_size]) { \
			return this->name(the_src, the_array_size); \
        } \
        template <rawbuf_struct_type::alloc_cmd cmd> \
        rawbuf_writer_iterator<input_type> name(array_count_type the_array_size) { \
			rawbuf_writer_iterator<input_type> result; \
			if(0 == the_array_size){ \
				result.writer = 0; \
				return result; \
			} \
			T* that = *this; \
            typename T::offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			size_t offset_offset; \
			input_type* pdata; \
			if(*foffset == 0){ \
				pdata = this->writer->template alloc<input_type, offset_type, array_count_type>(the_array_size, foffset); \
			} \
			else{ \
				array_count_type *old_size = (array_count_type*)((char*)foffset + *foffset); \
                if(*old_size > the_array_size){ \
                    *old_size = the_array_size; \
					pdata = (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(old_size + 1)), rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
                } \
				else{ \
					pdata = this->writer->template alloc<input_type, offset_type, array_count_type>(the_array_size, foffset); \
				} \
			} \
			offset_offset = ((char*)(pdata) - (char*)that); \
            result.writer = this->writer; \
            result.offset = this->offset + offset_offset; \
            return result; \
        } \
    }; \
	template<typename T> \
    struct rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
		typedef typename T::offset_type offset_type;\
        rawbuf_reader_iterator<input_type> name() { \
            array_count_type *psize = this->name<rawbuf_struct_type::array_size>() ; \
			rawbuf_reader_iterator<input_type> real_result; \
            if(psize == 0 || *psize == 0){ \
				real_result.reset(); \
                return real_result; \
            } \
            input_type* result = (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), rawbuf::rawbuf_property<input_type>::alignment_result, size_t)); \
			size_t data_real_size = sizeof(input_type) + (result->_.real_optional_fields_count - input_type::optional_fields_count) * sizeof(input_type::offset_type); \
            char* array_end = ((char*)result) + (*psize)*data_real_size; \
            if(((char*)this->end()) < array_end || array_end <= (char*)result){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array out of bound!" RAW_BUF_INFO(input_type, name))); \
                real_result.reset(); \
                return real_result; \
            } \
			if(!real_result.init(result, array_end)){ \
				this->invalidate(RAW_BUF_ERROR_MSG("First element invalid!" RAW_BUF_INFO(input_type, name))); \
				return real_result; \
			}\
			real_result.element_size = data_real_size; \
            return real_result; \
        } \
		template <rawbuf_struct_type::visit_array_cmd cmd> \
		array_count_type* name() { \
            T* that = *this; \
            if(that->_.real_optional_fields_count <= (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){return 0;}            /*   The field was not known by the creator.*/\
			offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset == 0){return 0;}																    /*   The optional field was not assigned*/\
            array_count_type *psize = (array_count_type*)((char*)foffset + *foffset ); \
            if( this->begin() >= ((char*)(psize + 1)) || this->end() < ((char*)(psize + 1))){				    /*1. The location of the length of the optional field should not be out of the packet.*/\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            } \
            if(!RAW_BUF_IS_ALIGNED_PTR(psize,rawbuf::rawbuf_property<array_count_type>::alignment_result)){	    /*2. The location of the length of the optional field should be aligned.*/	\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field is not aligned!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            }\
            return psize; \
		} \
		bool valid(){ \
			array_count_type* psize = this->name<rawbuf_struct_type::array_size>() ; \
			if(!(*this)){ \
				return false; \
			}\
			if(psize != 0) { \
				rawbuf_reader_iterator<input_type>  it = this->name(); \
				if(!(*this)){ \
					return false; \
				}\
				if(it){ \
					size_t real_size = *psize; ++it; \
					for(size_t i = 1 ; i < real_size; ++i,++it){ \
						if(!it){ \
							return false; \
						} \
					} \
				} \
			} \
            return ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).valid(); \
        } \
    };\


#define ADD_IMPL_INIT(input_type, name, count) \
    RAW_BUF_INCREASER(member_counter, RAW_BUF_JOIN(rawbuf_tag_, name) ); \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 > prev_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
		static const bool is_required = (count == 0); \
		static const rawbuf_uint tmp_aligned_coefficient = is_required ? 1: 0; \
        static const rawbuf_uint sizer         = (count)*rawbuf::rawbuf_property<input_type>::size_result + prev_type::sizer;\
        static const rawbuf_uint aligned_1x    = prev_type::aligned_1x + tmp_aligned_coefficient*rawbuf::rawbuf_property<input_type>::aligned_1x; \
        static const rawbuf_uint aligned_2x    = prev_type::aligned_2x + tmp_aligned_coefficient*rawbuf::rawbuf_property<input_type>::aligned_2x; \
        static const rawbuf_uint aligned_4x    = prev_type::aligned_4x + tmp_aligned_coefficient*rawbuf::rawbuf_property<input_type>::aligned_4x; \
        static const rawbuf_uint aligned_8x    = prev_type::aligned_8x + tmp_aligned_coefficient*rawbuf::rawbuf_property<input_type>::aligned_8x; \
		static const rawbuf_uint required_alignment = ((rawbuf::rawbuf_property<input_type>::alignment_result > prev_type::required_alignment ) && is_required)  ? rawbuf::rawbuf_property<input_type>::alignment_result : prev_type::required_alignment; \
    }; \

#endif
