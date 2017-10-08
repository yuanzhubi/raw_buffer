#if !defined(ADD_IMPL_INIT)

#define DEF_PACKET_BEGIN(rawbuf_name) \
struct rawbuf_name : public rawbuf_cmd{ \
private: \
    rawbuf_name();\
    rawbuf_name(const rawbuf_name&);\
    rawbuf_name& operator =(const rawbuf_name&); \
public:\
    typedef rawbuf_name rawbuf_struct_type; \
    static char (*member_counter(...))[1]; \
    static char (*required_counter(...))[1]; \
    static char (*optional_counter(...))[1]; \
    template<typename RAWBUF_T, rawbuf_int32 RAWBUF_N = -1, bool is_define_finished = true> \
    struct members_iterator{ \
        static const rawbuf_uint32 sizer = 0; \
        static const rawbuf_uint32 aligned_1x = 0; \
        static const rawbuf_uint32 aligned_2x = 0; \
        static const rawbuf_uint32 aligned_4x = 0; \
        static const rawbuf_uint32 aligned_8x = 0; \
        static const rawbuf_uint32 required_alignment = 0; \
    }; \
    template<typename RAWBUF_T, rawbuf_int32 RAWBUF_N> \
    struct type_indexer{ \
        typedef void* type; \
    }; \
    template<typename RAWBUF_T, rawbuf_int32 RAWBUF_N =  MAX_FIELDS_COUNT - 1> \
    struct rawbuf_writer_helper : public rawbuf_writer_helper<RAWBUF_T, RAWBUF_N-1>{\
    }; \
    template<typename RAWBUF_T> \
    struct rawbuf_writer_helper<RAWBUF_T, -1> : public rawbuf_writer<RAWBUF_T>{\
        template<typename RAWBUF_M> \
        void copy(const RAWBUF_M& rawbuf_src){ \
            RAWBUF_T::template members_iterator<RAWBUF_T, 0, true>::copy(rawbuf_src, *this); \
        }\
    }; \
    template<typename RAWBUF_T, rawbuf_int32 RAWBUF_N =  MAX_FIELDS_COUNT - 1> \
    struct rawbuf_reader_helper : public rawbuf_reader_helper<RAWBUF_T, RAWBUF_N-1>{\
        template<rawbuf_cmd::check_cmd RAWBUF_cmd> \
        const char* check(){return 0;} \
    }; \
    template<typename RAWBUF_T> \
    struct rawbuf_reader_helper<RAWBUF_T, -1> : public rawbuf_reader<RAWBUF_T>{\
        template<rawbuf_cmd::check_cmd RAWBUF_cmd> \
        const char* check(){return ((rawbuf_reader_helper<RAWBUF_T, 0>*)this)->template check<RAWBUF_cmd>(RAWBUF_EMPTY);} \
    }; \


#define DEF_PACKET_END \
public: \
    static const rawbuf_uint32 fields_count = RAW_BUF_INDEXER(member_counter); \
    static const rawbuf_uint32 optional_fields_count = RAW_BUF_INDEXER(optional_counter); \
    static const rawbuf_uint32 required_fields_count = RAW_BUF_INDEXER(required_counter); \
    typedef members_iterator<int, fields_count - 1> prev_type; \
    static const rawbuf_uint32 fields_alignment =  rawbuf::rawbuf_alignment<offset_type>::result; \
    static const rawbuf_uint32 alignment = (fields_alignment  > prev_type::required_alignment) ? fields_alignment : prev_type::required_alignment; \
    union{ \
        offset_type real_optional_fields_count; \
        offset_type field_offset[1 + optional_fields_count]; \
    }_;\
    static const rawbuf_uint32 sizer = prev_type::sizer; \
    static const rawbuf_uint32 aligned_1x = prev_type::aligned_1x + (alignment == 1 ? 0:1); \
    static const rawbuf_uint32 aligned_2x = prev_type::aligned_2x + (alignment == 2 ? 0:1); \
    static const rawbuf_uint32 aligned_4x = prev_type::aligned_4x + (alignment == 4 ? 0:1); \
    static const rawbuf_uint32 aligned_8x = prev_type::aligned_8x + (alignment == 8 ? 0:1); \
\
    template<typename RAWBUF_T> \
    struct members_iterator<RAWBUF_T, fields_count, true>{ \
        static const bool is_required = false; \
        template<typename RAWBUF_This, typename RAWBUF_M> \
        static void output(const RAWBUF_This&, RAWBUF_M&, int rawbuf_depth){ \
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate(RAWBUF_This &, RAWBUF_Func& ){ \
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_name(RAWBUF_This &,RAWBUF_Func&){ \
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_depth(RAWBUF_This &, RAWBUF_Func&, int){ \
        } \
        template<typename RAWBUF_M> \
        static void copy(const RAWBUF_M& , const rawbuf_writer<RAWBUF_T>&){ \
        } \
    }; \
    template<typename RAWBUF_M> \
    void output(RAWBUF_M& ost){ \
        rawbuf::json_output<RAWBUF_M> output_functor(ost); \
        members_iterator<int, 0>::iterate_with_depth(*this, output_functor, 0); \
    } \
    template<typename RAWBUF_Func> \
    void iterate(RAWBUF_Func& func_instance) { \
        members_iterator<int, 0 >::iterate(*this, func_instance); \
    } \
    template<typename RAWBUF_Func> \
    void iterate_with_name(RAWBUF_Func& func_instance) { \
        members_iterator<int, 0 >::iterate_with_name(*this, func_instance); \
    } \
    template<typename RAWBUF_Func> \
    void iterate_with_depth(RAWBUF_Func& func_instance, int rawbuf_depth = 0) { \
        members_iterator<int, 0 >::iterate_with_depth(*this, func_instance, rawbuf_depth); \
    } \
};

#define ADD_FIELD(rawbuf_input_type, rawbuf_name) \
public:\
    ADD_IMPL_INIT(ARGS_LIST(rawbuf_input_type), rawbuf_name, 1) \
    RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) ); \
    template <typename RAWBUF_T> \
    struct members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), true > : public members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false >{ \
        typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1 > next_type; \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            func_instance(this_instance.rawbuf_name()); \
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate(this_instance, func_instance); \
            }\
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_name(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            func_instance(this_instance.rawbuf_name(), #rawbuf_name); \
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate_with_name(this_instance, func_instance); \
            }\
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_depth(RAWBUF_This& this_instance, RAWBUF_Func& func_instance, int rawbuf_depth){ \
            func_instance(this_instance.rawbuf_name(), #rawbuf_name, rawbuf_depth); \
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate_with_depth(this_instance, func_instance, rawbuf_depth); \
            }\
        } \
        template<typename RAWBUF_M> \
        static void copy(const RAWBUF_M& the_src, const rawbuf_writer<RAWBUF_T>& rawbuf_packet){ \
            const RAWBUF_T& rawbuf_src = the_src; \
            const rawbuf_input_type* ptr = rawbuf_src.rawbuf_name(); \
            if(ptr != 0){ \
                rawbuf_packet->rawbuf_name(*ptr); \
            } else{ \
                RAWBUF_T* that = rawbuf_packet(); \
                that->_.field_offset[1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)] = 0; \
            } \
            next_type::copy(rawbuf_src, rawbuf_packet); \
        } \
    }; \
    rawbuf_input_type* rawbuf_name() const { \
        if(this->_.real_optional_fields_count > (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)){ \
            const offset_type *rawbuf_foffset = this->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            if(*rawbuf_foffset != 0 ){ \
                return (rawbuf_input_type*)((char*)rawbuf_foffset + *rawbuf_foffset ); \
            }\
        }\
        return 0; \
    } \
    template<typename RAWBUF_T> \
    struct rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        template<typename RAWBUF_M> \
        bool rawbuf_name(const rawbuf_input_type* rawbuf_src) { \
            RAWBUF_T* that = (*this)(); \
            offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            const char* dp = (const char*)(rawbuf_src);\
            if(dp < (char*)rawbuf_foffset || dp > this->writer->end()) { \
                return false; \
            } \
            size_t diff = dp - (char*)rawbuf_foffset; \
            if(sizeof(offset_type) <= 2 && diff > size_t((offset_type)(-1)) ){ \
                return false; \
            } \
            *rawbuf_foffset = (offset_type)diff; \
            return true; \
        } \
        template <typename RAWBUF_M> \
        rawbuf_input_type* rawbuf_name(const RAWBUF_M& the_src) { \
            const rawbuf_input_type& rawbuf_src = the_src; \
            RAWBUF_T* that = (*this)(); \
            typename RAWBUF_T::offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            if(*rawbuf_foffset != 0){ \
                rawbuf_input_type* rawbuf_data = (rawbuf_input_type*)(((char*)rawbuf_foffset) + *rawbuf_foffset ); \
                *rawbuf_data = rawbuf_src; \
                return rawbuf_data; \
            }\
            return this->writer->append(&rawbuf_src, rawbuf_foffset); \
        } \
    }; \
    template<typename RAWBUF_T> \
    struct rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        rawbuf_input_type* rawbuf_name() { \
            RAWBUF_T* that = (*this)(); \
            typedef typename RAWBUF_T::offset_type offset_type; \
            if(that->_.real_optional_fields_count <= (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)){return 0;}            /*   The field was not known by the creator.*/\
            offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            if(*rawbuf_foffset == 0) return 0;                                                                      /*   The field was not assigned.*/\
            rawbuf_input_type* rawbuf_data = (rawbuf_input_type*)((char*)rawbuf_foffset + *rawbuf_foffset ); \
            if(this->begin() >= (char*)(rawbuf_data) || this->end() < (char*)(rawbuf_data + 1)){                    /* 1.The offset should not be out of bound.*/\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                return 0; \
            } \
            if(!RAW_BUF_IS_ALIGNED_PTR(rawbuf_data, rawbuf::rawbuf_alignment<ARGS_LIST(rawbuf_input_type)>::result))        /* 2.The location of the optional field should be aligned.*/{   \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field is not aligned!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                return 0; \
            }\
            return rawbuf_data; \
        } \
        template<rawbuf_cmd::check_cmd RAWBUF_cmd> \
        const char* check(){ \
            this->rawbuf_name(); \
            if(!(*this)){ \
                return this->error_msg(); \
            } \
            return  ((rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1>*)this)->template check<RAWBUF_cmd>(RAWBUF_EMPTY); \
        } \
    };\

#define ADD_FIELD_REQUIRED(rawbuf_input_type, rawbuf_name) \
public:\
    ADD_IMPL_INIT(ARGS_LIST(rawbuf_input_type), rawbuf_name, 0) /*only optional field needs it to sum size.*/\
    RAW_BUF_INCREASER(required_counter, RAW_BUF_JOIN(rawbuf_tag_required_, rawbuf_name) ); \
    rawbuf_input_type RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name); \
    template <typename RAWBUF_T> \
    struct members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), true > : public members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false >{ \
        typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1 > next_type; \
        typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false > parent_type; \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            func_instance(this_instance.rawbuf_name()); \
            next_type::iterate(this_instance, func_instance); \
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_name(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            func_instance(this_instance.rawbuf_name(), #rawbuf_name); \
            next_type::iterate_with_name(this_instance, func_instance); \
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_depth(RAWBUF_This& this_instance, RAWBUF_Func& func_instance, int rawbuf_depth){ \
            func_instance(this_instance.rawbuf_name(), #rawbuf_name, rawbuf_depth); \
            next_type::iterate_with_depth(this_instance, func_instance, rawbuf_depth); \
        } \
        template<typename RAWBUF_M> \
        static void copy(const RAWBUF_M& the_src, const rawbuf_writer<RAWBUF_T>& rawbuf_packet){ \
            const RAWBUF_T& rawbuf_src = the_src; \
            rawbuf_packet()->RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name) = rawbuf_src.RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name); \
            next_type::copy(rawbuf_src, rawbuf_packet); \
        } \
        static const rawbuf_uint32 required_alignment = (parent_type::required_alignment > rawbuf::rawbuf_alignment<ARGS_LIST(rawbuf_input_type)>::result)  ?  parent_type::required_alignment : rawbuf::rawbuf_alignment<ARGS_LIST(rawbuf_input_type)>::result; \
    }; \
    inline rawbuf_input_type* rawbuf_name() const { \
        return (rawbuf_input_type*)(&(this->RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name))); \
    } \
    template <typename RAWBUF_M> \
    inline void rawbuf_name(const RAWBUF_M& rawbuf_src)  { \
        this->RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name) = rawbuf_src; \
    } \
    template<typename RAWBUF_T> \
    struct rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        template <typename RAWBUF_M> \
        rawbuf_input_type* rawbuf_name(const RAWBUF_M& the_src) const{ \
            const rawbuf_input_type& rawbuf_src = the_src; \
            RAWBUF_T* that = (*this)(); \
            that->RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name) = rawbuf_src; \
            return &(that->RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name)); \
        } \
    };\
    template<typename RAWBUF_T> \
    struct rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        rawbuf_input_type* rawbuf_name() const{ \
            RAWBUF_T* that = (*this)(); \
            return that->rawbuf_name(); \
        } \
        template<rawbuf_cmd::check_cmd RAWBUF_cmd> \
        const char* check(){ \
            return ((rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1>*)this)->template check<RAWBUF_cmd>(RAWBUF_EMPTY); \
        } \
    };\

#define ADD_FIELD_ARRAY(rawbuf_input_type, rawbuf_name, size_estimation) \
    ADD_IMPL_INIT(ARGS_LIST(rawbuf_input_type), rawbuf_name, size_estimation) \
    RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) ); \
    template <typename RAWBUF_T> \
    struct members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), true > : public members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false >{ \
        typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1 > next_type; \
        typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false > parent_type; \
        static const rawbuf_uint32 sizer = parent_type::sizer + 5; \
        static const rawbuf_uint32 aligned_1x = parent_type::aligned_1x + 1; \
        static const rawbuf_uint32 aligned_2x = parent_type::aligned_2x + 0; \
        static const rawbuf_uint32 aligned_4x = parent_type::aligned_4x + 0; \
        static const rawbuf_uint32 aligned_8x = parent_type::aligned_8x + 0; \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            typename RAWBUF_This::array_count_type rawbuf_size; \
            rawbuf_input_type* rawbuf_data = this_instance.rawbuf_name(rawbuf_size);\
            if(rawbuf_size != 0 && func_instance(rawbuf_data, rawbuf_data)) {\
                for(array_count_type i = 0; i < rawbuf_size; ++i){ \
                    func_instance(rawbuf_data + i); \
                }\
            }else{ \
                func_instance((rawbuf_input_type*)0, 0); \
            } \
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate(this_instance, func_instance); \
            }\
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_name(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            typename RAWBUF_This::array_count_type rawbuf_size; \
            rawbuf_input_type* rawbuf_data = this_instance.rawbuf_name(rawbuf_size);\
            if(rawbuf_size != 0 && func_instance(rawbuf_data, rawbuf_data, #rawbuf_name)) {\
                for(typename RAWBUF_This::array_count_type i = 0; i < rawbuf_size; ++i){ \
                    func_instance(rawbuf_data + i, 0); \
                }\
            }else{ \
                func_instance((rawbuf_input_type*)0, 0, #rawbuf_name); \
            } \
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate_with_name(this_instance, func_instance); \
            }\
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_depth(RAWBUF_This& this_instance, RAWBUF_Func& func_instance, int rawbuf_depth){ \
            typename RAWBUF_This::array_count_type rawbuf_size; \
            rawbuf_input_type* rawbuf_data = this_instance.rawbuf_name(rawbuf_size);\
            if(rawbuf_size != 0 && func_instance(rawbuf_data, rawbuf_size, #rawbuf_name, rawbuf_depth)) {\
                for(typename RAWBUF_This::array_count_type i = 0; i < rawbuf_size; ++i){ \
                    func_instance(rawbuf_data + i, 0, rawbuf_depth + 1); \
                }\
            }else{ \
                func_instance((rawbuf_input_type*)0, 0, #rawbuf_name, rawbuf_depth); \
            } \
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate_with_depth(this_instance, func_instance, rawbuf_depth); \
            }\
        } \
        template<typename RAWBUF_M> \
        static void copy(const RAWBUF_M& the_src, const rawbuf_writer<RAWBUF_T>& rawbuf_packet){ \
            typename RAWBUF_T::array_count_type rawbuf_size; \
            rawbuf_input_type* rawbuf_data = rawbuf_packet()->rawbuf_name(rawbuf_size);\
            if(rawbuf_size != 0){ \
                 rawbuf_packet->rawbuf_name(rawbuf_data, rawbuf_size); \
            } else{ \
                RAWBUF_T* that = rawbuf_packet(); \
                that->_.field_offset[1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)] = 0; \
            } \
            next_type::copy(the_src, rawbuf_packet); \
        } \
    }; \
    rawbuf_input_type* rawbuf_name() const{ \
       return  rawbuf_get_array_pointer<rawbuf_struct_type, rawbuf_input_type>( this, \
                (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)); \
    } \
    rawbuf_input_type* rawbuf_name(array_count_type& rawbuf_size) const{ \
       return  rawbuf_get_array_pointer<rawbuf_struct_type, rawbuf_input_type>( rawbuf_size, this, \
                (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)); \
    } \
    template<typename RAWBUF_M> /*RAWBUF_M can only be c_str*/ \
    rawbuf_input_type* rawbuf_name() const{ \
        array_count_type rawbuf_size; \
        rawbuf_input_type* rawbuf_data = this->rawbuf_name(rawbuf_size); \
        if(rawbuf_size != 0){ \
            if(sizeof(rawbuf_input_type) != 1 || ((char*)(rawbuf_data + rawbuf_size))[-1] == '\0'){ \
                return rawbuf_data; \
            } \
        }\
        return 0; \
    } \
    template<typename RAWBUF_M> /*RAWBUF_M can only be c_str*/ \
    rawbuf_input_type* rawbuf_name(array_count_type& rawbuf_size) const{ \
        rawbuf_input_type* rawbuf_data = this->rawbuf_name(rawbuf_size); \
        if(rawbuf_size != 0){ \
            if(sizeof(rawbuf_input_type) != 1 || ((char*)(rawbuf_data + rawbuf_size))[-1] == '\0'){ \
                return rawbuf_data; \
            } \
        }\
        return 0; \
    } \
    template <rawbuf_cmd::visit_array_cmd RAWBUF_cmd> \
    array_count_type rawbuf_name() const{ \
        array_count_type rawbuf_size; \
        this->rawbuf_name(rawbuf_size); \
        return rawbuf_size; \
    } \
    template<typename RAWBUF_T> \
    struct rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        typedef typename RAWBUF_T::array_count_type array_count_type; \
        typedef typename RAWBUF_T::offset_type offset_type; \
        template <typename RAWBUF_M> \
        rawbuf_input_type* rawbuf_name(const RAWBUF_M* the_src, array_count_type the_array_size) { \
            const rawbuf_input_type* rawbuf_src = the_src; \
            RAWBUF_T* that = (*this)(); \
            offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            return this->writer->append(rawbuf_src, the_array_size, rawbuf_foffset); \
        } \
        template <typename RAWBUF_M, array_count_type the_array_size> \
        rawbuf_input_type* rawbuf_name(const RAWBUF_M (&the_src)[the_array_size]) { \
            return this->rawbuf_name(the_src, the_array_size); \
        } \
        template <rawbuf_cmd::alloc_cmd RAWBUF_cmd> \
        rawbuf_input_type* rawbuf_name(array_count_type the_array_size) { \
            RAWBUF_T* that = (*this)(); \
            offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            rawbuf_input_type* rawbuf_data = this->writer->append(0, the_array_size, rawbuf_foffset); \
            if(sizeof(rawbuf_input_type) == 1) { \
                *((char*)rawbuf_data) = 0; \
            }\
            return rawbuf_data; \
        } \
    }; \
    template<typename RAWBUF_T> \
    struct rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        typedef typename RAWBUF_T::offset_type offset_type;\
        typedef typename RAWBUF_T::array_count_type array_count_type; \
        rawbuf_input_type* rawbuf_name() { \
            RAWBUF_T* that = (*this)(); \
            array_count_type rawbuf_size; \
            rawbuf_input_type* rawbuf_data = rawbuf_get_array_pointer<RAWBUF_T, rawbuf_input_type>(rawbuf_size, that, \
                (typename RAWBUF_T::offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)); \
            if(rawbuf_size == 0) { \
                if(rawbuf_data != 0){ \
                     this->invalidate(RAW_BUF_ERROR_MSG("Zero length array!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                } \
                return 0; \
            } \
            rawbuf_input_type* array_end = rawbuf_data + rawbuf_size; \
            if(this->end() < ((char*)(array_end)) || array_end <= rawbuf_data){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array out of bound!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                return 0; \
            } \
            return rawbuf_data; \
        } \
        rawbuf_input_type* rawbuf_name(array_count_type& rawbuf_size) { \
            RAWBUF_T* that = (*this)(); \
            rawbuf_input_type* rawbuf_data = rawbuf_get_array_pointer<RAWBUF_T, rawbuf_input_type>(rawbuf_size, that, \
                (typename RAWBUF_T::offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)); \
            if(rawbuf_size == 0) { \
                if(rawbuf_data != 0){ \
                     this->invalidate(RAW_BUF_ERROR_MSG("Zero length array!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                } \
                return 0; \
            } \
            rawbuf_input_type* array_end = rawbuf_data + rawbuf_size; \
            if(this->end() < ((char*)(array_end)) || array_end <= rawbuf_data){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array out of bound!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                return 0; \
            } \
            return rawbuf_data; \
        } \
        template<typename RAWBUF_M> /*RAWBUF_M can only be c_str*/ \
        rawbuf_input_type* rawbuf_name(){ \
            RAWBUF_T* that = (*this)(); \
            array_count_type rawbuf_size; \
            rawbuf_input_type* rawbuf_data = rawbuf_get_array_pointer<RAWBUF_T, rawbuf_input_type>(rawbuf_size, that, \
                (typename RAWBUF_T::offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)); \
            if(rawbuf_data == 0) {return rawbuf_data;} \
            rawbuf_input_type* array_end = rawbuf_data + rawbuf_size; \
            if(this->end() < ((char*)(array_end)) || array_end <= rawbuf_data){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array out of bound!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                return 0; \
            } \
            if(sizeof(rawbuf_input_type) != 1 || ((char*)(rawbuf_data + rawbuf_size))[-1] == '\0'){ \
                return rawbuf_data; \
            } \
            return 0; \
        } \
        template<typename RAWBUF_M> /*RAWBUF_M can only be c_str*/ \
        rawbuf_input_type* rawbuf_name(array_count_type& rawbuf_size){ \
            RAWBUF_T* that = (*this)(); \
            rawbuf_input_type* rawbuf_data = rawbuf_get_array_pointer<RAWBUF_T, rawbuf_input_type>(rawbuf_size, that, \
                (typename RAWBUF_T::offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)); \
            if(rawbuf_data == 0) {return rawbuf_data;} \
            rawbuf_input_type* array_end = rawbuf_data + rawbuf_size; \
            if(this->end() < ((char*)(array_end)) || array_end <= rawbuf_data){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array out of bound!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                return 0; \
            } \
            if(sizeof(rawbuf_input_type) != 1 || ((char*)(rawbuf_data + rawbuf_size))[-1] == '\0'){ \
                return rawbuf_data; \
            } \
            return 0; \
        } \
        template <rawbuf_cmd::visit_array_cmd RAWBUF_cmd> \
        array_count_type rawbuf_name() { \
            RAWBUF_T* that = (*this)(); \
            return that->template rawbuf_name<RAWBUF_cmd>(); \
        } \
        template<rawbuf_cmd::check_cmd RAWBUF_cmd> \
        const char* check(){ \
            this->rawbuf_name(); \
            if(!(*this)){ \
                return this->error_msg(); \
            } \
            return ((rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1>*)this)->template check<RAWBUF_cmd>(RAWBUF_EMPTY); \
        } \
    };\

#define ADD_FIELD_ARRAY_REQUIRED(rawbuf_input_type, rawbuf_name, real_size) \
    ADD_IMPL_INIT(ARGS_LIST(rawbuf_input_type), rawbuf_name, 0) \
    RAW_BUF_INCREASER(required_counter, RAW_BUF_JOIN(rawbuf_tag_required_, rawbuf_name) ); \
    rawbuf_input_type RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name)[real_size]; \
    template <typename RAWBUF_T> \
    struct members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), true > : public members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false >{ \
        typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1 > next_type; \
        typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false > parent_type; \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            rawbuf_input_type* rawbuf_data = this_instance.RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name); \
            if(func_instance(rawbuf_data, real_size)){\
                for(size_t i = 0; i < real_size; ++i){ \
                    func_instance(rawbuf_data + i); \
                }\
            }\
            next_type::iterate(this_instance, func_instance); \
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_name(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            rawbuf_input_type* rawbuf_data = this_instance.RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name); \
            if(func_instance(rawbuf_data, real_size, #rawbuf_name)){\
                for(size_t i = 0; i < real_size; ++i){ \
                    func_instance(rawbuf_data + i, 0); \
                }\
            }\
            next_type::iterate_with_name(this_instance, func_instance); \
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_depth(RAWBUF_This& this_instance, RAWBUF_Func& func_instance, int rawbuf_depth){ \
            rawbuf_input_type* rawbuf_data = this_instance.RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name); \
            if(func_instance(rawbuf_data, real_size, #rawbuf_name, rawbuf_depth)){\
                for(size_t i = 0; i < real_size; ++i){ \
                    func_instance(rawbuf_data + i, 0, rawbuf_depth + 1); \
                }\
            }\
            next_type::iterate_with_depth(this_instance, func_instance, rawbuf_depth); \
        } \
        template<typename RAWBUF_M> \
        static void copy(const RAWBUF_M& the_src, const rawbuf_writer<RAWBUF_T>& rawbuf_packet){ \
            const RAWBUF_T& rawbuf_src = the_src; \
            memcpy(rawbuf_packet()->RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name),  rawbuf_src.RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name), sizeof(rawbuf_input_type) * real_size) ; \
            next_type::copy(rawbuf_src, rawbuf_packet); \
        } \
        static const rawbuf_uint32 required_alignment = (parent_type::required_alignment > rawbuf::rawbuf_alignment<ARGS_LIST(rawbuf_input_type)>::result)  ?  parent_type::required_alignment : rawbuf::rawbuf_alignment<ARGS_LIST(rawbuf_input_type)>::result; \
    }; \
    rawbuf_input_type* rawbuf_name() { \
       return this->RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name); \
    } \
    template<typename RAWBUF_M> /*RAWBUF_M can only be c_str*/ \
    rawbuf_input_type* rawbuf_name() { \
        if(sizeof(rawbuf_input_type) == 1 && strnlen((char*)(this->RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name)), real_size) == real_size){ \
             return 0; \
        }\
        return this->RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name); \
    } \
    template <rawbuf_cmd::visit_array_cmd RAWBUF_cmd> \
    size_t rawbuf_name() const{ \
        return real_size; \
    } \
    template<typename RAWBUF_T> \
    struct rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        template <typename RAWBUF_M> \
        rawbuf_input_type* rawbuf_name(const RAWBUF_M* rawbuf_src, size_t the_array_size) { \
            if(the_array_size * sizeof(RAWBUF_M) <= real_size * sizeof(rawbuf_input_type)){ \
                return (rawbuf_input_type*)memcpy((*this)()->RAW_BUF_JOIN(rawbuf_data_required_, rawbuf_name),  rawbuf_src, the_array_size * sizeof(RAWBUF_M)) ; \
            } \
            return 0; \
        } \
        template <typename RAWBUF_M, size_t the_array_size> \
        rawbuf_input_type* rawbuf_name(const RAWBUF_M (&the_src)[the_array_size]) { \
            return this->rawbuf_name(the_src, the_array_size); \
        }\
    }; \
    template<typename RAWBUF_T> \
    struct rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        rawbuf_input_type* rawbuf_name() const{ \
            RAWBUF_T* that = (*this)(); \
            return that->rawbuf_name(); \
        } \
        template<rawbuf_cmd::check_cmd RAWBUF_cmd> \
        const char* check(){ \
            return ((rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1>*)this)->template check<RAWBUF_cmd>(RAWBUF_EMPTY); \
        } \
    };\


#define ADD_PACKET_ANY(rawbuf_input_type, rawbuf_name) ADD_PACKET_COMMON(ARGS_LIST(rawbuf_input_type), rawbuf_name, ADD_IMPL_INIT_ANY)

#define ADD_PACKET(rawbuf_input_type, rawbuf_name) ADD_PACKET_COMMON(ARGS_LIST(rawbuf_input_type), rawbuf_name, ADD_IMPL_INIT)

#define ADD_PACKET_COMMON(rawbuf_input_type, rawbuf_name, init_func) \
    init_func(ARGS_LIST(rawbuf_input_type), rawbuf_name, 1) \
    RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) ); \
    template <typename RAWBUF_T> \
    struct members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), true > : public members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false >{ \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1 > next_type; \
            typedef typename type_indexer<RAWBUF_Func,RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
            local_type* rawbuf_data = this_instance.rawbuf_name(); \
            if(func_instance(rawbuf_data) && rawbuf_data != 0 ){ \
                rawbuf_data->iterate(func_instance); \
            }\
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate(this_instance, func_instance); \
            }\
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_name(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1 > next_type; \
            typedef typename type_indexer<RAWBUF_Func, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
            local_type* rawbuf_data = this_instance.rawbuf_name(); \
            if(func_instance(rawbuf_data, #rawbuf_name) && rawbuf_data != 0 ){ \
                rawbuf_data->iterate_with_name(func_instance); \
            }\
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate_with_name(this_instance, func_instance); \
            }\
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_depth(RAWBUF_This& this_instance, RAWBUF_Func& func_instance, int rawbuf_depth){ \
            typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1 > next_type; \
            typedef typename type_indexer<RAWBUF_Func, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
            local_type* rawbuf_data = this_instance.rawbuf_name(); \
            if(func_instance(rawbuf_data, #rawbuf_name, rawbuf_depth) && rawbuf_data != 0){ \
                rawbuf_data->iterate_with_depth(func_instance, rawbuf_depth + 1); \
            }\
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate_with_depth(this_instance, func_instance, rawbuf_depth); \
            }\
        } \
        template<typename RAWBUF_M> \
        static void copy(const RAWBUF_M& the_src, const rawbuf_writer<RAWBUF_T>& rawbuf_packet){ \
            typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1 > next_type; \
            typedef typename type_indexer<int, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
            const RAWBUF_T& rawbuf_src = the_src; \
            const local_type* ptr = rawbuf_src.rawbuf_name(); \
            if(ptr != 0){ \
                rawbuf_packet->rawbuf_name(*ptr); \
            } else{ \
                RAWBUF_T* that = rawbuf_packet(); \
                that->_.field_offset[ 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)] = 0; \
            } \
            next_type::copy(rawbuf_src, rawbuf_packet); \
        } \
    }; \
    rawbuf_input_type* rawbuf_name() const { \
        if(this->_.real_optional_fields_count > (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)){ \
            const offset_type *rawbuf_foffset = this->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            if(*rawbuf_foffset != 0 ){ \
                return (rawbuf_input_type*)((char*)rawbuf_foffset + *rawbuf_foffset); \
            }\
        }\
        return 0; \
    } \
    template<typename RAWBUF_T> \
    struct rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        typedef typename RAWBUF_T::offset_type offset_type; \
        typedef typename RAWBUF_T::template type_indexer<int,RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
        template<typename RAWBUF_M> \
        bool rawbuf_name(const local_type* rawbuf_src) { \
            RAWBUF_T* that = (*this)(); \
            offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            const char* dp = (const char*)(rawbuf_src);\
            if(dp < (char*)rawbuf_foffset || dp > this->writer->end()) { \
                return false; \
            } \
            size_t diff = dp - (char*)rawbuf_foffset; \
            if(sizeof(offset_type) <= 2 && diff > size_t((offset_type)(-1)) ){ \
                return false; \
            } \
            *rawbuf_foffset = (offset_type)diff; \
            return true; \
        } \
        template<typename RAWBUF_M> \
        rawbuf_writer<RAWBUF_M> rawbuf_name(const RAWBUF_M& the_src) { \
            RAWBUF_T* that = (*this)(); \
            offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            rawbuf_writer<RAWBUF_M> rawbuf_data; \
            rawbuf_data.writer = this->writer; \
            char* dp; \
            if(*rawbuf_foffset == 0){ \
                dp = (char*)(this->writer->template alloc<RAWBUF_M, offset_type>(rawbuf_foffset));\
                if(sizeof(offset_type) <= 2 && dp == 0){ \
                    rawbuf_data.writer = 0; \
                    return rawbuf_data; \
                } \
            } \
            else{ \
                dp = ((char*)rawbuf_foffset) + *rawbuf_foffset; /*re-assignment case*/\
                if(((local_type*)dp)->_.real_optional_fields_count < RAWBUF_M::optional_fields_count){ /*Assgined from older version? */\
                    dp = (char*)(this->writer->template alloc<RAWBUF_M, offset_type>(rawbuf_foffset));\
                    if(sizeof(offset_type) <= 2 && dp == 0){ \
                        rawbuf_data.writer = 0; \
                        return rawbuf_data; \
                    } \
                }\
                else{ \
                    ((local_type*)dp)->_.real_optional_fields_count = RAWBUF_T::optional_fields_count; /*Unknown field will be discarded. */\
                } \
            } \
            rawbuf_data.offset = dp - this->writer->data_ptr; \
            const local_type &rawbuf_src = the_src; \
            RAWBUF_M::template members_iterator<RAWBUF_M, 0, true>::copy(rawbuf_src, rawbuf_data); \
            return rawbuf_data; \
        } \
        template <rawbuf_cmd::alloc_cmd RAWBUF_cmd> \
        rawbuf_writer<local_type> rawbuf_name() { \
            RAWBUF_T* that = (*this)(); \
            offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            rawbuf_writer<typename RAWBUF_T::template type_indexer<int,RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type> rawbuf_data; \
            rawbuf_data.writer = this->writer; \
            char* dp; \
            if(*rawbuf_foffset == 0){ \
                dp = (char*)(this->writer->template alloc<local_type, offset_type>(rawbuf_foffset));\
                if(sizeof(offset_type) <= 2 && dp == 0){ \
                    rawbuf_data.writer = 0; \
                    return rawbuf_data; \
                } \
            } \
            else{ \
                dp = ((char*)rawbuf_foffset) + *rawbuf_foffset; \
            } \
            rawbuf_data.offset = dp - this->writer->data_ptr; \
            return rawbuf_data; \
        } \
    }; \
    template<typename RAWBUF_T> \
    struct rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        rawbuf_reader<typename RAWBUF_T::template type_indexer<int,RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type > rawbuf_name() { \
            typedef typename RAWBUF_T::template type_indexer<int,RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
            rawbuf_reader<local_type> real_result;  \
            RAWBUF_T* that = (*this)(); \
            typedef typename RAWBUF_T::offset_type offset_type; \
            if(that->_.real_optional_fields_count <= (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)){real_result.reset();  return real_result;} /*   The field was not known by the creator.*/\
            offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            if(*rawbuf_foffset == 0) {real_result.reset();  return real_result;}                                                                    /*   The field was not assigned.*/\
            local_type* rawbuf_data = (rawbuf_input_type*)((char*)rawbuf_foffset + *rawbuf_foffset ); \
            if(this->begin() >= (char*)(rawbuf_data) || this->end() < (char*)(rawbuf_data + 1)){                                                         /* 1.The offset should not be out of bound.*/\
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                real_result.invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                return real_result; \
            } \
            if(!real_result.init(rawbuf_data, this->end())){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional field reader init failed!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
            } \
            return real_result; \
        } \
        template<typename rawbuf_cmd::check_cmd RAWBUF_cmd> \
        const char* check(){ \
            typedef typename RAWBUF_T::template type_indexer<int,RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
            rawbuf_reader<local_type> result_further = this->rawbuf_name(); \
            bool local_result = bool(result_further); \
            if(local_result){ \
                const char* rawbuf_data = rawbuf_has_error(result_further); \
                if(rawbuf_data) { \
                    this->invalidate(rawbuf_data);\
                    return rawbuf_data; \
                } \
            } \
            else{ \
                if(!(*this)){ \
                    return this->error_msg(); \
                } \
            } \
            return ((rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1>*)this)->template check<RAWBUF_cmd>(RAWBUF_EMPTY); \
        } \
    };\


#define ADD_PACKET_ARRAY_ANY(rawbuf_input_type, rawbuf_name, size_estimation) ADD_PACKET_ARRAY_COMMON(ARGS_LIST(rawbuf_input_type), rawbuf_name, size_estimation, ADD_IMPL_INIT_ANY, true, ARRAY_VISIT_FUNC_ANY)

#define ADD_PACKET_ARRAY(rawbuf_input_type, rawbuf_name, size_estimation) ADD_PACKET_ARRAY_COMMON(ARGS_LIST(rawbuf_input_type), rawbuf_name, size_estimation, ADD_IMPL_INIT, false, ARRAY_VISIT_FUNC)

#define ARRAY_VISIT_FUNC(rawbuf_type, rawbuf_name) \
    rawbuf_packet_iterator<rawbuf_type> rawbuf_name() const{ \
        rawbuf_type* rawbuf_data = rawbuf_get_array_pointer<rawbuf_struct_type, rawbuf_type>(this, \
                (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)); \
        rawbuf_packet_iterator<rawbuf_type> func_result; \
        if(rawbuf_data != 0){ \
            func_result.data_ptr = rawbuf_data; \
            func_result.data_real_size = rawbuf_get_packet_size(rawbuf_data); \
            return func_result; \
        }\
        func_result.data_ptr = 0; \
        return func_result; \
    } \
    rawbuf_packet_iterator<rawbuf_type> rawbuf_name(array_count_type& rawbuf_size) const{ \
        rawbuf_type* rawbuf_data = rawbuf_get_array_pointer<rawbuf_struct_type, rawbuf_type>(rawbuf_size, this, \
                (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)); \
        rawbuf_packet_iterator<rawbuf_type> func_result; \
        if(rawbuf_size != 0){ \
            func_result.data_ptr = rawbuf_data; \
            func_result.data_real_size = rawbuf_get_packet_size(rawbuf_data); \
            return func_result; \
        }\
        func_result.data_ptr = 0; \
        return func_result; \
    } \
    ARRAY_VISIT_FUNC_ANY(rawbuf_type, rawbuf_name)

#define ARRAY_VISIT_FUNC_ANY(rawbuf_type, rawbuf_name) \
    template <typename RAWBUF_T> \
    rawbuf_packet_iterator<RAWBUF_T> rawbuf_name() const{\
        rawbuf_type* rawbuf_data = rawbuf_get_array_pointer<rawbuf_struct_type, rawbuf_type>(this, \
                (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)); \
        rawbuf_packet_iterator<RAWBUF_T> func_result; \
        if(rawbuf_data != 0){ \
            func_result.data_ptr = rawbuf_data; \
            func_result.data_real_size = rawbuf_get_packet_size(rawbuf_data); \
            return func_result; \
        }\
        func_result.data_ptr = 0; \
        return func_result; \
    } \
    template <typename RAWBUF_T> \
    rawbuf_packet_iterator<RAWBUF_T> rawbuf_name(array_count_type& rawbuf_size) const{\
        rawbuf_type* rawbuf_data = rawbuf_get_array_pointer<rawbuf_struct_type, rawbuf_type>(rawbuf_size, this, \
                (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)); \
        rawbuf_packet_iterator<RAWBUF_T> func_result; \
        if(rawbuf_size != 0){ \
            func_result.data_ptr = rawbuf_data; \
            func_result.data_real_size = rawbuf_get_packet_size(rawbuf_data); \
            return func_result; \
        }\
        func_result.data_ptr = 0; \
        return func_result; \
    } \

#define ADD_PACKET_ARRAY_COMMON(rawbuf_input_type, rawbuf_name, size_estimation, init_func, is_rawbuf_ref, visit_func) \
    init_func(ARGS_LIST(rawbuf_input_type), rawbuf_name, size_estimation) \
    RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) ); \
    template <typename RAWBUF_T> \
    struct members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), true > : public members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false >{ \
        typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1 > next_type; \
        typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false > parent_type; \
        static const rawbuf_uint32 sizer = parent_type::sizer + sizeof(array_count_type); \
        static const rawbuf_uint32 aligned_1x = parent_type::aligned_1x + 1; \
        static const rawbuf_uint32 aligned_2x = parent_type::aligned_2x + 0; \
        static const rawbuf_uint32 aligned_4x = parent_type::aligned_4x + 0; \
        static const rawbuf_uint32 aligned_8x = parent_type::aligned_8x + 0; \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            typename RAWBUF_This::array_count_type rawbuf_size; \
            typedef typename type_indexer<RAWBUF_Func, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
            local_type* rawbuf_data = this_instance.template rawbuf_name<local_type>(rawbuf_size)();\
            if(rawbuf_size != 0) {\
                if(func_instance(rawbuf_data, rawbuf_size)){ \
                    size_t data_real_size = rawbuf_get_packet_size(rawbuf_data); \
                    char* pdata = (char*)rawbuf_data; \
                    for(array_count_type i = 0; i < rawbuf_size; ++i, pdata+=data_real_size){ \
                        bool rawbuf_data = func_instance(((local_type*)pdata), 0); /*{}*/\
                        if(rawbuf_data){ \
                            ((local_type*)pdata)->iterate(func_instance); \
                        }\
                    }\
                } \
            }else{ \
                func_instance((local_type*)0, 0); \
            } \
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate(this_instance, func_instance); \
            }\
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_name(RAWBUF_This& this_instance, RAWBUF_Func& func_instance){ \
            typename RAWBUF_This::array_count_type rawbuf_size; \
            typedef typename type_indexer<RAWBUF_Func, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
            local_type* rawbuf_data = this_instance.template rawbuf_name<local_type>(rawbuf_size)();\
            if(rawbuf_size != 0) {\
                if(func_instance(rawbuf_data, rawbuf_size, #rawbuf_name)){ \
                    size_t data_real_size = rawbuf_get_packet_size(rawbuf_data); \
                    char* pdata = (char*)rawbuf_data; \
                    for(array_count_type i = 0; i < rawbuf_size; ++i, pdata+=data_real_size){ \
                        bool rawbuf_data = func_instance(((local_type*)pdata), 0); /*{}*/\
                        if(rawbuf_data){ \
                            ((local_type*)pdata)->iterate_with_name(func_instance); \
                        }\
                    }\
                } \
            }else{ \
                func_instance((local_type*)0, 0, #rawbuf_name); \
            } \
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate_with_name(this_instance, func_instance, #rawbuf_name); \
            }\
        } \
        template<typename RAWBUF_This, typename RAWBUF_Func> \
        static void iterate_with_depth(RAWBUF_This& this_instance, RAWBUF_Func& func_instance, int rawbuf_depth){ \
            typename RAWBUF_This::array_count_type rawbuf_size; \
            typedef typename type_indexer<RAWBUF_Func, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
            local_type* rawbuf_data = this_instance.template rawbuf_name<local_type>(rawbuf_size)();\
            if(rawbuf_size != 0) {\
                if(func_instance(rawbuf_data, rawbuf_size, #rawbuf_name, rawbuf_depth)){ \
                    size_t data_real_size = rawbuf_get_packet_size(rawbuf_data); \
                    char* pdata = (char*)rawbuf_data; \
                    for(array_count_type i = 0; i < rawbuf_size; ++i, pdata+=data_real_size){ \
                        if(func_instance(((local_type*)pdata), 0, rawbuf_depth + 1)){ \
                            ((local_type*)pdata)->iterate_with_depth(func_instance, rawbuf_depth + 2); \
                        }\
                    }\
                } \
            }else{ \
                func_instance((local_type*)0, 0, #rawbuf_name, rawbuf_depth); \
            } \
            if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name) + 1){ \
                next_type::iterate_with_depth(this_instance, func_instance, rawbuf_depth); \
            }\
        } \
        template<typename RAWBUF_M> \
        static void copy(const RAWBUF_M& the_src, const rawbuf_writer<RAWBUF_T>& rawbuf_packet){ \
            typename RAWBUF_T::array_count_type rawbuf_size; \
            typedef typename type_indexer<RAWBUF_M, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
            rawbuf_input_type* rawbuf_data = the_src.template rawbuf_name<local_type>(rawbuf_size)();\
            if(rawbuf_size != 0){ \
                 rawbuf_packet->rawbuf_name(rawbuf_data, rawbuf_size); \
            } else{ \
                RAWBUF_T* that = rawbuf_packet(); \
                that->_.field_offset[1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)] = 0; \
            } \
            next_type::copy(the_src, rawbuf_packet); \
        } \
    }; \
    visit_func(rawbuf_input_type, rawbuf_name) \
    template <rawbuf_cmd::visit_array_cmd RAWBUF_cmd> /*get_count*/\
    array_count_type rawbuf_name() const{ \
        array_count_type rawbuf_size; \
        typedef typename type_indexer<int, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
        this->rawbuf_name<local_type>(rawbuf_size); \
        return rawbuf_size; \
    } \
    template<typename RAWBUF_T> \
    struct rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        typedef typename RAWBUF_T::array_count_type array_count_type; \
        typedef typename RAWBUF_T::offset_type offset_type; \
        typedef typename RAWBUF_T::template type_indexer<int,RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
        template <typename RAWBUF_M> \
        rawbuf_writer_iterator<local_type> rawbuf_name(const RAWBUF_M* the_src, array_count_type the_array_size) { \
            rawbuf_writer_iterator<local_type> result; \
            const local_type* rawbuf_src = the_src; \
            if(0 == the_array_size) {\
                result.writer = 0; \
                return result; \
            }\
            result = this->rawbuf_name<alloc>(the_array_size); \
            if(sizeof(offset_type) <= 2 && !result){ \
                return result; \
            } \
            size_t data_real_size = rawbuf_get_packet_size(rawbuf_src); \
            char* pdata = (char*)rawbuf_src; \
            for(array_count_type i = 0; i < the_array_size; ++i, pdata+=data_real_size, ++result){ \
                local_type::template members_iterator<local_type, 0, true>::copy(*((local_type*)pdata), result); \
            }\
            result -= the_array_size; \
            return result; \
        } \
        template <rawbuf_cmd::alloc_cmd RAWBUF_cmd> \
        rawbuf_writer_iterator<local_type> rawbuf_name(array_count_type the_array_size) { \
            rawbuf_writer_iterator<local_type> result; \
            result.offset = 0; /* To avoid a gcc 4.4 warning bug : https://gcc.gnu.org/bugzilla/show_bug.cgi?id=40146 */\
            if(0 == the_array_size){ \
                result.writer = 0; \
                return result; \
            } \
            RAWBUF_T* that = (*this)(); \
            typename RAWBUF_T::offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name); \
            result.writer = this->writer; \
            char* dp; \
            dp = (char*)(this->writer->template alloc<local_type, offset_type, array_count_type>(the_array_size, rawbuf_foffset)); \
            result.offset = dp - this->writer->data_ptr; \
            return result; \
        } \
    }; \
    template<typename RAWBUF_T> \
    struct rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)> : public rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 >{\
        typedef typename RAWBUF_T::template type_indexer<int,RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>::type local_type; \
        typedef typename RAWBUF_T::offset_type offset_type;\
        rawbuf_reader_iterator<local_type> rawbuf_name(typename RAWBUF_T::array_count_type &rawbuf_size) { \
            local_type* rawbuf_data = rawbuf_get_array_pointer<RAWBUF_T, local_type>(rawbuf_size, (*this)(), \
                (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, rawbuf_name)); \
            rawbuf_reader_iterator<local_type> real_result; \
            if(rawbuf_size == 0){ \
                real_result.reset(); \
                if(rawbuf_data != 0){ \
                     this->invalidate(RAW_BUF_ERROR_MSG("Zero length array!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                } \
                return real_result; \
            } \
            if((this->end()) <= (char*)rawbuf_data || (char*)rawbuf_data <= this->begin()){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array begin out of bound!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                real_result.reset(); \
                return real_result; \
            } \
            size_t data_real_size = rawbuf_get_packet_size(rawbuf_data); \
            char* array_end = ((char*)rawbuf_data) + rawbuf_size*data_real_size; \
            if(((char*)this->end()) < array_end || array_end <= (char*)rawbuf_data ){ \
                this->invalidate(RAW_BUF_ERROR_MSG("Optional array end out of bound!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                real_result.reset(); \
                return real_result; \
            } \
            if(!real_result.init(rawbuf_data, this->end())){ \
                this->invalidate(RAW_BUF_ERROR_MSG("First element invalid!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                return real_result; \
            }\
            real_result.element_size = data_real_size; \
            return real_result; \
        } \
        rawbuf_reader_iterator<local_type> rawbuf_name() { \
            typename RAWBUF_T::array_count_type rawbuf_size;\
            return this->rawbuf_name(rawbuf_size); \
        } \
        template <rawbuf_cmd::visit_array_cmd RAWBUF_cmd> \
        array_count_type rawbuf_name() { \
            typename RAWBUF_T::array_count_type rawbuf_size;\
            this->rawbuf_name(rawbuf_size); \
            return rawbuf_size;\
        } \
        template<rawbuf_cmd::check_cmd RAWBUF_cmd> \
        const char* check(){ \
            array_count_type rawbuf_size;\
            rawbuf_reader_iterator<local_type> it = this->rawbuf_name(rawbuf_size); \
            if(!(*this)){ \
                return this->error_msg(); \
            } \
            if(!it){ \
                return it.error_msg(); \
            } \
            if(rawbuf_size == 0){ \
                return 0; \
            } \
            const char* rawbuf_result = rawbuf_has_error(it);\
            if(rawbuf_result){\
                this->invalidate(rawbuf_result);\
                return rawbuf_result; \
            }\
            size_t real_optional_fields_count = (*it)._.real_optional_fields_count; \
            for(array_count_type i = 1 ; i < rawbuf_size; ++i){ \
                ++it;\
                if(!it){ \
                    const char* tmp_result = it.error_msg(); \
                    this->invalidate(tmp_result);\
                    return tmp_result;  \
                } \
                if( (real_optional_fields_count != (*it)._.real_optional_fields_count) ){ \
                    this->invalidate(RAW_BUF_ERROR_MSG("Optional_fields_count is not same!" RAW_BUF_INFO(ARGS_LIST(rawbuf_input_type), rawbuf_name))); \
                    return this->error_msg();  \
                } \
                rawbuf_result = rawbuf_has_error(it); \
                if(rawbuf_result) { \
                    this->invalidate(rawbuf_result);\
                    return rawbuf_result; \
                } \
            } \
            return ((rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) + 1>*)this)->template check<RAWBUF_cmd>(RAWBUF_EMPTY); \
        } \
    };\

#define ADD_IMPL_INIT(rawbuf_input_type, rawbuf_name, count) \
    RAW_BUF_INCREASER(member_counter, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) ); \
    template <typename RAWBUF_T> \
    struct members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false >{ \
        typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 > prev_type; \
        static const bool is_required = (count == 0); \
        static const rawbuf_uint32 tmp_aligned_coefficient = is_required ? 1: 0; \
        static const rawbuf_uint32 sizer         = (count)*rawbuf::rawbuf_property<ARGS_LIST(rawbuf_input_type)>::size_result + prev_type::sizer;\
        static const rawbuf_uint32 aligned_1x    = prev_type::aligned_1x + tmp_aligned_coefficient*rawbuf::rawbuf_property<ARGS_LIST(rawbuf_input_type)>::aligned_1x; \
        static const rawbuf_uint32 aligned_2x    = prev_type::aligned_2x + tmp_aligned_coefficient*rawbuf::rawbuf_property<ARGS_LIST(rawbuf_input_type)>::aligned_2x; \
        static const rawbuf_uint32 aligned_4x    = prev_type::aligned_4x + tmp_aligned_coefficient*rawbuf::rawbuf_property<ARGS_LIST(rawbuf_input_type)>::aligned_4x; \
        static const rawbuf_uint32 aligned_8x    = prev_type::aligned_8x + tmp_aligned_coefficient*rawbuf::rawbuf_property<ARGS_LIST(rawbuf_input_type)>::aligned_8x; \
        static const rawbuf_uint32 required_alignment = ((rawbuf::rawbuf_alignment<ARGS_LIST(rawbuf_input_type)>::result > prev_type::required_alignment ) && (is_required)) ? rawbuf::rawbuf_alignment<ARGS_LIST(rawbuf_input_type)>::result : prev_type::required_alignment; \
    }; \
    template<typename RAWBUF_T>\
    struct type_indexer<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>{ \
        typedef rawbuf_input_type type; \
    }; \

#define ADD_IMPL_INIT_ANY(rawbuf_input_type, rawbuf_name, count) \
    RAW_BUF_INCREASER(member_counter, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) ); \
    template <typename RAWBUF_T> \
    struct members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name), false >{ \
        typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name) - 1 > prev_type; \
        static const bool is_required = false; \
        static const rawbuf_uint32 sizer         = prev_type::sizer;\
        static const rawbuf_uint32 aligned_1x    = prev_type::aligned_1x ; \
        static const rawbuf_uint32 aligned_2x    = prev_type::aligned_2x ; \
        static const rawbuf_uint32 aligned_4x    = prev_type::aligned_4x ; \
        static const rawbuf_uint32 aligned_8x    = prev_type::aligned_8x ; \
        static const rawbuf_uint32 required_alignment = prev_type::required_alignment; \
    }; \
    template<typename RAWBUF_T>\
    struct type_indexer<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, rawbuf_name)>{ \
        typedef rawbuf_input_type type; \
    }; \
    


#endif
