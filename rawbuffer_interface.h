#if !defined(ADD_IMPL_INIT)

#include "rawbuffer_util.h"
#include "rawbuffer_alignment.h"
#include "rawbuffer_allocator.h"
#include "rawbuffer_output_template.h"

#include <stddef.h>

struct rawbuf_config{
    typedef rawbuf_uint offset_type;
	typedef rawbuf_uint array_count_type;
    const static rawbuf_uint fields_alignment =  rawbuf::rawbuf_alignment<offset_type>::result; // The real alignment_size will be max(rawbuf::rawbuf_alignment<offset_type>::result, alignment_size). You can redefine both of these property in your packet definition.
};

struct rawbuf_cmd : public rawbuf_config{
    
	enum alloc_cmd{
        alloc = 0
    };

    enum visit_array_cmd{
		get_array_size = 0
    };

	enum check_cmd{
		check_packet = 0
	};

	//visit_c_string_cmd
	typedef int c_str;

	//get element size of packet array
	typedef int get_element_size;

	//We can "reference" a packet with higher address.
	typedef int add_ref;
};

#define DEF_PACKET_BEGIN(name) \
struct name : public rawbuf_cmd{ \
private: \
	name();\
	name(const name&);\
	name& operator =(const name&); \
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
		static const rawbuf_uint required_alignment = rawbuf::rawbuf_alignment<offset_type>::result; \
    }; \
	template<typename T, rawbuf_int N> \
	struct type_indexer{ \
		typedef void* type; \
	}; \
    template<typename T, rawbuf_int N =  MAX_FIELDS_COUNT - 1> \
    struct rawbuf_writer_helper : public rawbuf_writer_helper<T, N-1>{\
    }; \
    template<typename T> \
    struct rawbuf_writer_helper<T, -1> : public rawbuf_writer<T>{\
		template<typename M> \
		void copy(const M& src){ \
			T::template members_iterator<T, 0, true>::copy(src, *this); \
		}\
    }; \
	template<typename T, rawbuf_int N =  MAX_FIELDS_COUNT - 1> \
    struct rawbuf_reader_helper : public rawbuf_reader_helper<T, N-1>{\
        template<rawbuf_cmd::check_cmd cmd> \
		const char* check(){return 0;} \
    }; \
    template<typename T> \
	struct rawbuf_reader_helper<T, -1> : public rawbuf_reader<T>{\
        template<rawbuf_cmd::check_cmd cmd> \
		const char* check(){return ((rawbuf_reader_helper<T, 0>&)(*this)).check<cmd>();} \
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
        template<typename M> \
        bool name(const input_type* src) { \
            T* that = (*this)(); \
            offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			const char* dp = (const char*)(src);\
			if(dp < (char*)foffset || dp > this->writer->end()) { \
				return false; \
			} \
			size_t diff = dp - (char*)foffset; \
			if(sizeof(offset_type) <= 2 && diff > size_t((offset_type)(-1)) ){ \
				return false; \
			} \
			*foffset = (offset_type)diff; \
            return true; \
        } \
		template <typename M> \
        input_type* name(const M& the_src) { \
            const input_type& src = the_src; \
            T* that = (*this)(); \
            typename T::offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset != 0){ \
				input_type* result = (input_type*)(((char*)foffset) + *foffset ); \
				*result = src; \
				return result; \
            }\
            return this->writer->append(&src, foffset); \
        } \
    }; \
	template<typename T> \
    struct rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
        input_type* name() { \
            T* that = (*this)(); \
            typedef typename T::offset_type offset_type; \
			if(that->_.real_optional_fields_count <= (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){return 0;}            /*   The field was not known by the creator.*/\
			offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset == 0) return 0;																	    /*	 The field was not assigned.*/\
            input_type* result = (input_type*)((char*)foffset + *foffset ); \
            if(this->begin() >= (char*)(result+1) || this->end() < (char*)(result + 1)){				    /* 1.The offset should not be out of bound.*/\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            } \
            if(!RAW_BUF_IS_ALIGNED_PTR(result, rawbuf::rawbuf_alignment<input_type>::result))	    /* 2.The location of the optional field should be aligned.*/{	\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field is not aligned!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            }\
			return result; \
        } \
		template<rawbuf_cmd::check_cmd cmd> \
        const char* check(){ \
            this->name(); \
			if(!(*this)){ \
				return this->error_msg(); \
			} \
            return  ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).check<cmd>(); \
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
		static const rawbuf_uint required_alignment = (parent_type::required_alignment > rawbuf::rawbuf_alignment<input_type>::result)  ?  parent_type::required_alignment : rawbuf::rawbuf_alignment<input_type>::result; \
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
            T* that = (*this)(); \
			that->RAW_BUF_JOIN(rawbuf_data_required_, name) = src; \
			return &(that->RAW_BUF_JOIN(rawbuf_data_required_, name)); \
        } \
    };\
	template<typename T> \
    struct rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
        input_type* name() const{ \
            T* that = (*this)(); \
			return that->name(); \
        } \
		template<rawbuf_cmd::check_cmd cmd> \
		const char* check(){ \
            return ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).check<cmd>(); \
        } \
    };\

#define ADD_FIELD_ARRAY(input_type, name, size_estimation) \
    ADD_IMPL_INIT(input_type, name, size_estimation) \
	RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, name) ); \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), true > : public members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef typename T::offset_type offset_type; \
		typedef typename T::array_count_type array_count_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false > parent_type; \
        static const rawbuf_uint sizer = parent_type::sizer + sizeof(array_count_type); \
        static const rawbuf_uint aligned_1x = parent_type::aligned_1x + (rawbuf::rawbuf_alignment<array_count_type>::result == 1 ? 0:1); \
        static const rawbuf_uint aligned_2x = parent_type::aligned_2x + (rawbuf::rawbuf_alignment<array_count_type>::result == 2 ? 0:1); \
        static const rawbuf_uint aligned_4x = parent_type::aligned_4x + (rawbuf::rawbuf_alignment<array_count_type>::result == 4 ? 0:1); \
        static const rawbuf_uint aligned_8x = parent_type::aligned_8x + (rawbuf::rawbuf_alignment<array_count_type>::result == 8 ? 0:1); \
        template<typename This, typename Func> \
        static void iterate(This& this_instance, Func& func_instance){ \
            array_count_type *psize = this_instance.name<rawbuf_struct_type::get_array_size>() ; \
			if(psize != 0) {\
                input_type* data = (input_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<input_type>::result, size_t)); \
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
            array_count_type *psize = this_instance.name<rawbuf_struct_type::get_array_size>() ; \
			if(psize != 0) {\
                input_type* data = (input_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<input_type>::result, size_t)); \
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
            array_count_type *psize = this_instance.name<rawbuf_struct_type::get_array_size>() ; \
			if(psize != 0) {\
                input_type* data = (input_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<input_type>::result, size_t)); \
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
			array_count_type* psize = src.name<rawbuf_struct_type::get_array_size>() ; \
			if(psize != 0 && *psize != 0 ){ \
				 input_type* ptr = (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), \
							rawbuf::rawbuf_alignment<input_type>::result, size_t)); \
				 packet->name(ptr, *psize); \
            } else{ \
                T* that = packet(); \
                that->_.field_offset[1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name)] = 0; \
            } \
            next_type::copy(src, packet); \
        } \
    }; \
    input_type* name() const{ \
		array_count_type *psize = this->name<rawbuf_struct_type::get_array_size>() ; \
        if(psize != 0 && *psize != 0 ){ \
            return (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), \
                        rawbuf::rawbuf_alignment<input_type>::result, size_t)); \
        }\
        return 0; \
    } \
	template<typename M> /*M can only be c_str*/ \
    input_type* name() const{ \
		array_count_type *psize = this->name<rawbuf_struct_type::get_array_size>() ; \
        if(psize != 0 && *psize != 0 ){ \
			input_type* result = (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), \
                        rawbuf::rawbuf_alignment<input_type>::result, size_t)); \
			if(sizeof(input_type) != 1 || ((char*)(result + *psize))[-1] == '\0'){ \
				return result; \
			} \
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
        input_type* name(const M* the_src, array_count_type the_array_size) { \
            const input_type* src = the_src; \
            T* that = (*this)(); \
            offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset != 0){ \
                array_count_type *old_size = (array_count_type*)((char*)foffset + *foffset); \
                if(*old_size > the_array_size){ \
                    *old_size = the_array_size; \
                    return (input_type*)memcpy((input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(old_size + 1)), rawbuf::rawbuf_alignment<input_type>::result, size_t)) \
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
            T* that = (*this)(); \
            offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
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
            array_count_type *psize = this->name<rawbuf_struct_type::get_array_size>() ; \
            if(psize == 0 || *psize == 0){ \
                return 0; \
            } \
            input_type* result = (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), rawbuf::rawbuf_alignment<input_type>::result, size_t)); \
            input_type* array_end = result + *psize; \
            if(this->end() < ((char*)(array_end)) || array_end <= result){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array out of bound!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            } \
            return result; \
        } \
		template<typename M> /*M can only be c_str*/ \
		input_type* name() const{ \
			array_count_type *psize = this->name<rawbuf_struct_type::get_array_size>() ; \
            if(psize == 0 || *psize == 0){ \
                return 0; \
            } \
            input_type* result = (input_type*)(RAW_BUF_ALIGN_TYPE(((size_t)(psize + 1)), rawbuf::rawbuf_alignment<input_type>::result, size_t)); \
            input_type* array_end = result + *psize; \
            if(this->end() < ((char*)(array_end)) || array_end <= result){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array out of bound!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            } \
            if(sizeof(input_type) != 1 || ((char*)(result + *psize))[-1] == '\0'){ \
				return result; \
			} \
			return 0; \
		} \
		template <rawbuf_struct_type::visit_array_cmd cmd> \
		array_count_type* name() { \
            T* that = (*this)(); \
            if(that->_.real_optional_fields_count <= (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){return 0;}            /*   The field was not known by the creator.*/\
			offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset == 0){return 0;}																    /*   The optional field was not assigned*/\
            array_count_type *psize = (array_count_type*)((char*)foffset + *foffset ); \
            if( this->begin() >= ((char*)(psize + 1)) || this->end() < ((char*)(psize + 1))){				    /*1. The location of the length of the optional field should not be out of the packet.*/\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            } \
            if(!RAW_BUF_IS_ALIGNED_PTR(psize,rawbuf::rawbuf_alignment<array_count_type>::result)){	    /*2. The location of the length of the optional field should be aligned.*/	\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field is not aligned!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            }\
            return psize; \
		} \
		template<rawbuf_cmd::check_cmd cmd> \
		const char* check(){ \
            this->name(); \
			if(!(*this)){ \
				return this->error_msg(); \
			} \
            return ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).check<cmd>(); \
        } \
    };\

#define ADD_FIELD_ARRAY_REQUIRED(input_type, name, real_size) \
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
		static const rawbuf_uint required_alignment = (parent_type::required_alignment > rawbuf::rawbuf_alignment<input_type>::result)  ?  parent_type::required_alignment : rawbuf::rawbuf_alignment<input_type>::result; \
    }; \
    input_type* name() { \
       return this->RAW_BUF_JOIN(rawbuf_data_required_, name); \
    } \
	template<typename M> /*M can only be c_str*/ \
    input_type* name() { \
		if(sizeof(input_type) == 1 && strnlen((char*)(this->RAW_BUF_JOIN(rawbuf_data_required_, name)), real_size) == real_size){ \
			 return 0; \
		}\
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
            T* that = (*this)(); \
			return that->name(); \
        } \
		template<rawbuf_cmd::check_cmd cmd> \
		const char* check(){ \
            return ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).check<cmd>(); \
        } \
    };\


#define ADD_PACKET_ANY(input_type, name) ADD_PACKET_COMMON(input_type, name, ADD_IMPL_INIT_ANY)

#define ADD_PACKET(input_type, name) ADD_PACKET_COMMON(input_type, name, ADD_IMPL_INIT)

#define ADD_PACKET_COMMON(input_type, name, init_func) \
    init_func(input_type, name, 1) \
	RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, name) ); \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), true > : public members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
        template<typename This, typename Func> \
        static void iterate(This& this_instance, Func& func_instance){ \
			typedef typename T::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type local_type; \
            local_type* instance = this_instance.name(); \
            bool result = func_instance(instance); \
            if(instance != 0 && result){ \
				instance->iterate(result); \
            }\
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_name(This& this_instance, Func& func_instance){ \
			typedef typename T::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type local_type; \
            local_type* instance = this_instance.name(); \
            bool result = func_instance(instance, #name); \
            if(instance != 0 && result){ \
				instance->iterate_with_name(result); \
            }\
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate_with_name(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_depth(This& this_instance, Func& func_instance, int depth){ \
			typedef typename T::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type local_type; \
            local_type* instance = this_instance.name(); \
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
                T* that = packet(); \
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
        typedef typename T::offset_type offset_type; \
		typedef typename T::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type local_type; \
		template<typename M> \
        bool name(const local_type* src) { \
            T* that = (*this)(); \
            offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			const char* dp = (const char*)(src);\
			if(dp < (char*)foffset || dp > this->writer->end()) { \
				return false; \
			} \
			size_t diff = dp - (char*)foffset; \
			if(sizeof(offset_type) <= 2 && diff > size_t((offset_type)(-1)) ){ \
				return false; \
			} \
			*foffset = (offset_type)diff; \
            return true; \
        } \
		template<typename M> \
        rawbuf_writer<M> name(const M& src) { \
            T* that = (*this)(); \
            offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			rawbuf_writer<M> result; \
            result.writer = this->writer; \
			char* dp; \
			if(*foffset == 0){ \
				dp = (char*)(this->writer->template alloc<M, offset_type>(foffset));\
				if(sizeof(offset_type) <= 2 && dp == 0){ \
					result.writer = 0; \
					return result; \
				} \
				result.offset = dp - this->writer->data_ptr; \
				M::template members_iterator<M, 0, true>::copy(src, result); \
			} \
			else{ \
				dp = ((char*)foffset) + *foffset; \
				result.offset = dp - this->writer->data_ptr; \
				M::template members_iterator<M, 0, true>::copy(src, result); \
			} \
            return result; \
        } \
        template <rawbuf_struct_type::alloc_cmd cmd> \
        rawbuf_writer<local_type> name() { \
            T* that = (*this)(); \
            offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			rawbuf_writer<typename T::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type> result; \
            result.writer = this->writer; \
			char* dp; \
			if(*foffset == 0){ \
				dp = (char*)(this->writer->template alloc<local_type, offset_type>(foffset));\
				if(sizeof(offset_type) <= 2 && dp == 0){ \
					result.writer = 0; \
					return result; \
				} \
			} \
			else{ \
				dp = ((char*)foffset) + *foffset; \
			} \
			result.offset = dp - this->writer->data_ptr; \
            return result; \
        } \
    }; \
	template<typename T> \
    struct rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
		rawbuf_reader<typename T::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type > name() { \
			typedef typename T::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type local_type; \
            rawbuf_reader<local_type> real_result;  \
			T* that = (*this)(); \
			typedef typename T::offset_type offset_type; \
			if(that->_.real_optional_fields_count <= (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){real_result.reset();  return real_result;} /*   The field was not known by the creator.*/\
			offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset == 0) {real_result.reset();  return real_result;}																	/*   The field was not assigned.*/\
            local_type* result = (input_type*)((char*)foffset + *foffset ); \
            if(this->begin() >= (char*)(result+1) || this->end() < (char*)(result + 1)){														 /* 1.The offset should not be out of bound.*/\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(input_type, name))); \
				real_result.invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(input_type, name))); \
                return real_result; \
            } \
            if(!real_result.init(result, this->end())){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field reader init failed!" RAW_BUF_INFO(input_type, name))); \
            } \
            return real_result; \
        } \
		template<rawbuf_cmd::check_cmd cmd> \
		const char* check(){ \
			typedef typename T::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type local_type; \
            rawbuf_reader<local_type> result_further = this->name(); \
			bool local_result = bool(result_further); \
			if(local_result){ \
				const char* result = rawbuf_has_error(result_further); \
				if(result) {return result;} \
			} \
			else{ \
				if(!(*this)){ \
					return this->error_msg(); \
				} \
			} \
            return ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).check<cmd>(); \
        } \
    };\


#define ADD_PACKET_ARRAY_ANY(input_type, name, size_estimation) ADD_PACKET_ARRAY_COMMON(input_type, name, size_estimation, ADD_IMPL_INIT_ANY, true, ARRAY_VISIT_FUNC_ANY)

#define ADD_PACKET_ARRAY(input_type, name, size_estimation) ADD_PACKET_ARRAY_COMMON(input_type, name, size_estimation, ADD_IMPL_INIT, false, ARRAY_VISIT_FUNC)

#define ARRAY_VISIT_FUNC(rawbuf_type, name) \
	rawbuf_packet_iterator<rawbuf_type> name() const{ \
		array_count_type *psize = this->name<rawbuf_struct_type::get_array_size>() ; \
		rawbuf_packet_iterator<rawbuf_type> result; \
        if(psize != 0 && *psize != 0 ){ \
			typedef rawbuf_type::offset_type data_size_type;\
			data_size_type* pdata_real_size = (data_size_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<data_size_type>::result, size_t)); \
            rawbuf_type* data = (rawbuf_type*)(RAW_BUF_ALIGN_TYPE((size_t)(pdata_real_size + 1), rawbuf::rawbuf_alignment<rawbuf_type>::result, size_t)); \
			result.data_ptr = data; \
			result.data_real_size = *pdata_real_size; \
			return result; \
        }\
		result.data_ptr = 0; \
        return result; \
    } \
	template <typename T> /*get_element_size*/\
	size_t name(typename rawbuf::result_check<!rawbuf::is_rawbuf_struct<T>::result>::type *p = 0) const{ (void)p;\
		if(this->_.real_optional_fields_count > (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){ \
			typedef typename rawbuf_type::offset_type data_size_type;\
			const offset_type *foffset = this->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			if(*foffset != 0){ \
				array_count_type* psize = (array_count_type*)((char*)foffset + *foffset); \
				data_size_type* pdata_real_size = (data_size_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<data_size_type>::result, size_t)); \
				return *pdata_real_size; \
			}\
		} \
        return 0; \
    } \

#define ARRAY_VISIT_FUNC_ANY(rawbuf_type, name) \
	template <typename T> \
	rawbuf_packet_iterator<T> name(typename rawbuf::result_check<rawbuf::is_rawbuf_struct<T>::result>::type *p = 0) const{(void)p; \
		array_count_type *psize = this->name<rawbuf_struct_type::get_array_size>() ; \
		rawbuf_packet_iterator<rawbuf_type> result; \
        if(psize != 0 && *psize != 0 ){ \
			typedef typename T::offset_type data_size_type;\
			data_size_type* pdata_real_size = (data_size_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<data_size_type>::result, size_t)); \
            rawbuf_type* data = (T*)(RAW_BUF_ALIGN_TYPE((size_t)(pdata_real_size + 1), rawbuf::rawbuf_alignment<T>::result, size_t)); \
			result.data_ptr = data; \
			result.data_real_size = *pdata_real_size; \
			return result; \
        }\
		result.data_ptr = 0; \
        return result; \
    } \
	template <typename T> /*get_element_size*/\
	size_t name(typename rawbuf::result_check<!rawbuf::is_rawbuf_struct<T>::result>::type *p = 0) const{ (void)p;\
        if(this->_.real_optional_fields_count > (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){ \
			typedef typename rawbuf_struct_type::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type local_type; \
			typedef typename local_type::offset_type data_size_type;\
			const offset_type *foffset = this->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			if(*foffset != 0){ \
				array_count_type* psize = (array_count_type*)((char*)foffset + *foffset); \
				data_size_type* pdata_real_size = (data_size_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<data_size_type>::result, size_t)); \
				return *pdata_real_size; \
			}\
		} \
        return 0; \
    } \

#define ADD_PACKET_ARRAY_COMMON(input_type, name, size_estimation, init_func, is_rawbuf_ref, visit_func) \
    init_func(input_type, name, size_estimation) \
	RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, name) ); \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), true > : public members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef typename T::offset_type offset_type; \
		typedef typename T::array_count_type array_count_type; \
		typedef typename T::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type local_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false > parent_type; \
        static const rawbuf_uint sizer = parent_type::sizer + sizeof(array_count_type); \
        static const rawbuf_uint aligned_1x = parent_type::aligned_1x + (rawbuf::rawbuf_alignment<array_count_type>::result == 1 ? 0:1); \
        static const rawbuf_uint aligned_2x = parent_type::aligned_2x + (rawbuf::rawbuf_alignment<array_count_type>::result == 2 ? 0:1); \
        static const rawbuf_uint aligned_4x = parent_type::aligned_4x + (rawbuf::rawbuf_alignment<array_count_type>::result == 4 ? 0:1); \
        static const rawbuf_uint aligned_8x = parent_type::aligned_8x + (rawbuf::rawbuf_alignment<array_count_type>::result == 8 ? 0:1); \
        template<typename This, typename Func> \
        static void iterate(This& this_instance, Func& func_instance){ \
			typedef typename local_type::offset_type data_size_type;\
            array_count_type *psize = this_instance.name<rawbuf_struct_type::get_array_size>(); \
			if(psize != 0) {\
				data_size_type* pdata_real_size = (data_size_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<data_size_type>::result, size_t)); \
                local_type* data = (local_type*)(RAW_BUF_ALIGN_TYPE((size_t)(pdata_real_size + 1), rawbuf::rawbuf_alignment<local_type>::result, size_t)); \
				if(func_instance(data, *psize)){ \
					size_t data_real_size = *pdata_real_size; \
					char* pdata = (char*)data; \
                    for(array_count_type i = 0; i < *psize; ++i, pdata+=data_real_size){ \
						bool result = func_instance(((local_type*)pdata), 0); /*{}*/\
						if(result){ \
							((local_type*)pdata)->iterate(func_instance); \
						}\
                    }\
                } \
            }else{ \
                func_instance((local_type*)0, 0); \
            } \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_name(This& this_instance, Func& func_instance){ \
			typedef typename local_type::offset_type data_size_type;\
            array_count_type *psize = this_instance.name<rawbuf_struct_type::get_array_size>() ; \
			if(psize != 0) {\
                data_size_type* pdata_real_size = (data_size_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<data_size_type>::result, size_t)); \
                local_type* data = (local_type*)(RAW_BUF_ALIGN_TYPE((size_t)(pdata_real_size + 1), rawbuf::rawbuf_alignment<local_type>::result, size_t)); \
				if(func_instance(data, *psize, #name)){ \
					size_t data_real_size = *pdata_real_size; \
					char* pdata = (char*)data; \
                    for(array_count_type i = 0; i < *psize; ++i, pdata+=data_real_size){ \
						bool result = func_instance(((local_type*)pdata), 0); /*{}*/\
						if(result){ \
							((local_type*)pdata)->iterate_with_name(func_instance); \
						}\
                    }\
                } \
            }else{ \
                func_instance((local_type*)0, 0, #name); \
            } \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate_with_name(this_instance, func_instance); \
			}\
        } \
        template<typename This, typename Func> \
        static void iterate_with_depth(This& this_instance, Func& func_instance, int depth){ \
			typedef typename local_type::offset_type data_size_type;\
            array_count_type *psize = this_instance.name<rawbuf_struct_type::get_array_size>() ; \
			if(psize != 0) {\
                data_size_type* pdata_real_size = (data_size_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<data_size_type>::result, size_t)); \
                local_type* data = (local_type*)(RAW_BUF_ALIGN_TYPE((size_t)(pdata_real_size + 1), rawbuf::rawbuf_alignment<local_type>::result, size_t)); \
				if(func_instance(data, *psize, #name, depth)){ \
					size_t data_real_size = *pdata_real_size; \
					char* pdata = (char*)data; \
                    for(array_count_type i = 0; i < *psize; ++i, pdata+=data_real_size){ \
						bool result = func_instance(((local_type*)pdata), 0, depth + 1); /*{}*/\
						if(result){ \
							((local_type*)pdata)->iterate_with_depth(func_instance, depth + 2); \
						}\
                    }\
                } \
            }else{ \
                func_instance((local_type*)0, 0, #name, depth); \
            } \
			if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, name) + 1){ \
				next_type::iterate_with_depth(this_instance, func_instance, depth); \
			}\
        } \
		template<typename M> \
        static void copy(const M& the_src, const rawbuf_writer<T>& packet){ \
			typedef typename local_type::offset_type data_size_type;\
            const T& src = the_src; \
			array_count_type* psize = src.name<rawbuf_struct_type::get_array_size>() ; \
			if(psize != 0 && *psize != 0 ){ \
				data_size_type* pdata_real_size = (data_size_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<data_size_type>::result, size_t)); \
                local_type* data = (local_type*)(RAW_BUF_ALIGN_TYPE((size_t)(pdata_real_size + 1), rawbuf::rawbuf_alignment<local_type>::result, size_t)); \
				packet->name(data, *psize, *pdata_real_size); \
            } else{ \
                T* that = packet(); \
                that->_.field_offset[1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name)] = 0; \
            } \
            next_type::copy(src, packet); \
        } \
    }; \
	visit_func(input_type, name) \
    template <rawbuf_struct_type::visit_array_cmd cmd> /*get_array_size*/\
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
		typedef typename T::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type local_type; \
        template <typename M> \
        rawbuf_writer_iterator<local_type> name(const M* the_src, array_count_type the_array_size, size_t data_real_size = sizeof(local_type)) { \
            rawbuf_writer_iterator<local_type> result; \
			const local_type* src = the_src; \
			if(0 == the_array_size) {\
				result.writer = 0; \
				return result; \
			}\
			result = this->name<alloc>(the_array_size); \
			if(sizeof(offset_type) <= 2 && !result){ \
				return result; \
			} \
			char* pdata = (char*)src; \
            for(array_count_type i = 0; i < the_array_size; ++i, pdata+=data_real_size, ++result){ \
				local_type::template members_iterator<local_type, 0, true>::copy(*((local_type*)pdata), result); \
            }\
            result -= the_array_size; \
            return result; \
        } \
        template <typename M, array_count_type the_array_size> \
        rawbuf_writer_iterator<local_type> name(const M (&the_src)[the_array_size]) { \
			return this->name(the_src, the_array_size, sizeof(local_type)); \
        } \
        template <rawbuf_struct_type::alloc_cmd cmd> \
        rawbuf_writer_iterator<local_type> name(array_count_type the_array_size) { \
			rawbuf_writer_iterator<local_type> result; \
			result.offset = 0; /* To avoid a gcc 4.4 warning bug : https://gcc.gnu.org/bugzilla/show_bug.cgi?id=40146 */\
			if(0 == the_array_size){ \
				result.writer = 0; \
				return result; \
			} \
			T* that = (*this)(); \
            typename T::offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
			result.writer = this->writer; \
			char* dp; \
			if(*foffset == 0){ \
				dp = (char*)(this->writer->template alloc<local_type, offset_type, array_count_type>(the_array_size, foffset)); \
			} \
			else{ \
				array_count_type *psize = (array_count_type*)((char*)foffset + *foffset); \
                if(*psize > the_array_size){ \
                    *psize = the_array_size; \
					offset_type* pdata_real_size = (offset_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<offset_type>::result, size_t)); \
					dp = (char*)(RAW_BUF_ALIGN_TYPE((size_t)(pdata_real_size + 1), rawbuf::rawbuf_alignment<local_type>::result, size_t)); \
                } \
				else{ \
					dp = (char*)(this->writer->template alloc<local_type, offset_type, array_count_type>(the_array_size, foffset)); \
					if(sizeof(offset_type) <= 2 && (dp == 0)){ \
						result.writer = 0; \
						return result; \
					} \
				} \
			} \
            result.offset = dp - this->writer->data_ptr; \
            return result; \
        } \
    }; \
	template<typename T> \
    struct rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name)> : public rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 >{\
		typedef typename T::template type_indexer<T,RAW_BUF_JOIN(rawbuf_tag_, name)>::type local_type; \
		typedef typename T::offset_type offset_type;\
        rawbuf_reader_iterator<local_type> name() { \
            array_count_type *psize = this->name<rawbuf_struct_type::get_array_size>() ; \
			rawbuf_reader_iterator<local_type> real_result; \
            if(psize == 0 || *psize == 0){ \
				real_result.reset(); \
                return real_result; \
            } \
			typedef typename local_type::offset_type data_size_type; \
			data_size_type* pdata_real_size = (data_size_type*)(RAW_BUF_ALIGN_TYPE((size_t)(psize + 1), rawbuf::rawbuf_alignment<data_size_type>::result, size_t)); \
			local_type* data = (local_type*)(RAW_BUF_ALIGN_TYPE((size_t)(pdata_real_size + 1), rawbuf::rawbuf_alignment<local_type>::result, size_t)); \
			if(!RAW_BUF_IS_ALIGNED_PTR(pdata_real_size, rawbuf::rawbuf_alignment<data_size_type>::result)){	    /*2. The location of the size of element be aligned.*/	\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field sizeof element is not aligned!" RAW_BUF_INFO(input_type, name))); \
				real_result.reset(); \
				return real_result; \
            }\
			if((*pdata_real_size) % rawbuf::rawbuf_alignment<T>::result != 0){	    /*3. The size should mod the alignment.*/	\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field sizeof element is not aligned!" RAW_BUF_INFO(input_type, name))); \
				real_result.reset(); \
				return real_result; \
            }\
			data_size_type data_real_size = *pdata_real_size; \
            char* array_end = ((char*)data) + (*psize)*data_real_size; \
            if(((char*)this->end()) < array_end || array_end <= (char*)data){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array out of bound!" RAW_BUF_INFO(input_type, name))); \
                real_result.reset(); \
                return real_result; \
            } \
			if(!real_result.init(data, this->end())){ \
				this->invalidate(RAW_BUF_ERROR_MSG("First element invalid!" RAW_BUF_INFO(input_type, name))); \
				return real_result; \
			}\
			real_result.element_size = data_real_size; \
            return real_result; \
        } \
		template <rawbuf_struct_type::visit_array_cmd cmd> \
		array_count_type* name() { \
            T* that = (*this)(); \
            if(that->_.real_optional_fields_count <= (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, name)){return 0;}            /*   The field was not known by the creator.*/\
			offset_type *foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, name); \
            if(*foffset == 0){return 0;}																    /*   The optional field was not assigned*/\
            array_count_type *psize = (array_count_type*)((char*)foffset + *foffset ); \
            if( this->begin() >= ((char*)(psize + 1)) || this->end() < ((char*)(psize + 1))){				    /*1. The location of the length of the optional field should not be out of the packet.*/\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            } \
            if(!RAW_BUF_IS_ALIGNED_PTR(psize, rawbuf::rawbuf_alignment<array_count_type>::result)){	    /*2. The location of the length of the optional field should be aligned.*/	\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field count of elements is not aligned!" RAW_BUF_INFO(input_type, name))); \
                return 0; \
            }\
            return psize; \
		} \
		template<rawbuf_cmd::check_cmd cmd> \
		const char* check(){ \
			array_count_type* psize = this->name<rawbuf_struct_type::get_array_size>() ; \
			if(!(*this)){ \
				return this->error_msg(); \
			} \
			if(psize != 0) { \
				rawbuf_reader_iterator<local_type>  it = this->name(); \
				if(!(*this)){ \
					return this->error_msg(); \
				} \
				if(it){ \
					const char* result = rawbuf_has_error(it);\
					if(result){\
						return result; \
					}\
					size_t real_optional_fields_count = (*it)._.real_optional_fields_count; \
					size_t real_size = *psize; \
					for(size_t i = 1 ; i < real_size; ++i){ \
						++it;\
						if(!it){ \
							return it.error_msg();  \
						} \
						if( (real_optional_fields_count != (*it)._.real_optional_fields_count) ){ \
							this->invalidate(RAW_BUF_ERROR_MSG("Optional field is not aligned!" RAW_BUF_INFO(input_type, name))); \
							return this->error_msg();  \
						} \
						result = rawbuf_has_error(it); \
						if(result) { \
							return result; \
						} \
					} \
				} \
			} \
            return ((rawbuf_reader_helper<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1>&)(*this)).check<cmd>(); \
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
		static const rawbuf_uint required_alignment = ((rawbuf::rawbuf_alignment<input_type>::result > prev_type::required_alignment ) && (is_required)) ? rawbuf::rawbuf_alignment<input_type>::result : prev_type::required_alignment; \
    }; \
	template<typename T>\
	struct type_indexer<T, RAW_BUF_JOIN(rawbuf_tag_, name)>{ \
		typedef input_type type; \
	}; \

#define ADD_IMPL_INIT_ANY(input_type, name, count) \
    RAW_BUF_INCREASER(member_counter, RAW_BUF_JOIN(rawbuf_tag_, name) ); \
    template <typename T> \
    struct members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name), false >{ \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) - 1 > prev_type; \
        typedef members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 > next_type; \
		static const bool is_required = false; \
        static const rawbuf_uint sizer         = prev_type::sizer;\
        static const rawbuf_uint aligned_1x    = prev_type::aligned_1x ; \
        static const rawbuf_uint aligned_2x    = prev_type::aligned_2x ; \
        static const rawbuf_uint aligned_4x    = prev_type::aligned_4x ; \
        static const rawbuf_uint aligned_8x    = prev_type::aligned_8x ; \
		static const rawbuf_uint required_alignment = prev_type::required_alignment; \
    }; \
	template<typename T>\
	struct type_indexer<T, RAW_BUF_JOIN(rawbuf_tag_, name)>{ \
		typedef input_type type; \
	}; \

#endif
