template<typename T
, typename Array_count_type = rawbuf_config::array_count_type
, bool is_raw_buf_test_result = rawbuf::is_rawbuf_struct<T>::result
>
struct test_type2 : public rawbuf_cmd {
private:
	test_type2();
	test_type2(const test_type2&);
	test_type2& operator =(const test_type2&);
public:
	typedef test_type2 rawbuf_struct_type;
	static char (*member_counter(...))[1];
	static char (*required_counter(...))[1];
	static char (*optional_counter(...))[1];
	template<typename RAWBUF_T, rawbuf_int RAWBUF_N = -1, bool is_define_finished = true> struct members_iterator {
		static const rawbuf_uint sizer = 0;
		static const rawbuf_uint aligned_1x = 0;
		static const rawbuf_uint aligned_2x = 0;
		static const rawbuf_uint aligned_4x = 0;
		static const rawbuf_uint aligned_8x = 0;
		static const rawbuf_uint required_alignment = 0;
	};
	template<typename RAWBUF_T, rawbuf_int RAWBUF_N> struct type_indexer {
		typedef void* type;
	};
	template<typename RAWBUF_T, rawbuf_int RAWBUF_N = MAX_FIELDS_COUNT - 1> struct rawbuf_writer_helper : public rawbuf_writer_helper<RAWBUF_T, RAWBUF_N-1> { };
	template<typename RAWBUF_T> struct rawbuf_writer_helper<RAWBUF_T, -1> : public rawbuf_writer<RAWBUF_T> {
		template<typename RAWBUF_M> void copy(const RAWBUF_M& rawbuf_src) {
			RAWBUF_T::template members_iterator<RAWBUF_T, 0, true>::copy(rawbuf_src, *this);
		}
	};
	template<typename RAWBUF_T, rawbuf_int RAWBUF_N = MAX_FIELDS_COUNT - 1> struct rawbuf_reader_helper : public rawbuf_reader_helper<RAWBUF_T, RAWBUF_N-1> {
		template<rawbuf_cmd::check_cmd cmd> const char* check() {
			return 0;
		}
	};
	template<typename RAWBUF_T> struct rawbuf_reader_helper<RAWBUF_T, -1> : public rawbuf_reader<RAWBUF_T> {
		template<rawbuf_cmd::check_cmd cmd> const char* check() {
			return ((rawbuf_reader_helper<RAWBUF_T, 0>&)(*this)).check<cmd>();
		}
	};
	typedef Array_count_type array_count_type;
public:
	RAW_BUF_INCREASER(member_counter, RAW_BUF_JOIN(rawbuf_tag_, a) );
	template <typename RAWBUF_T> struct members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, a), false > {
	    typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, a) - 1 > prev_type;
	    static const bool is_required = (1 == 0);
	    static const rawbuf_uint tmp_aligned_coefficient = is_required ? 1: 0;
	    static const rawbuf_uint sizer = (1)*rawbuf::rawbuf_property<T>::size_result + prev_type::sizer;
	    static const rawbuf_uint aligned_1x = prev_type::aligned_1x + tmp_aligned_coefficient*rawbuf::rawbuf_property<T>::aligned_1x;
	    static const rawbuf_uint aligned_2x = prev_type::aligned_2x + tmp_aligned_coefficient*rawbuf::rawbuf_property<T>::aligned_2x;
	    static const rawbuf_uint aligned_4x = prev_type::aligned_4x + tmp_aligned_coefficient*rawbuf::rawbuf_property<T>::aligned_4x;
	    static const rawbuf_uint aligned_8x = prev_type::aligned_8x + tmp_aligned_coefficient*rawbuf::rawbuf_property<T>::aligned_8x;
	    static const rawbuf_uint required_alignment = ((rawbuf::rawbuf_alignment<T>::result > prev_type::required_alignment ) && (is_required)) ? rawbuf::rawbuf_alignment<T>::result : prev_type::required_alignment;
	};
	template<typename RAWBUF_T> struct type_indexer<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, a)> {
	    typedef T type;
	};
	RAW_BUF_INCREASER(optional_counter, RAW_BUF_JOIN(rawbuf_tag_optional_, a) );
template <typename RAWBUF_T> struct members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, a), true > : public members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, a), false > {
	    typedef members_iterator<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, a) + 1 > next_type;
	template<typename RAWBUF_This, typename RAWBUF_Func> static void iterate(RAWBUF_This& this_instance, RAWBUF_Func& func_instance) {
		func_instance(this_instance.a());
		if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, a) + 1) {
			next_type::iterate(this_instance, func_instance);
		}
	} template<typename RAWBUF_This, typename RAWBUF_Func> static void iterate_with_name(RAWBUF_This& this_instance, RAWBUF_Func& func_instance) {
		func_instance(this_instance.a(), "a");
		if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, a) + 1) {
			next_type::iterate_with_name(this_instance, func_instance);
		}
	} template<typename RAWBUF_This, typename RAWBUF_Func> static void iterate_with_depth(RAWBUF_This& this_instance, RAWBUF_Func& func_instance, int rawbuf_depth) {
		func_instance(this_instance.a(), "a", rawbuf_depth);
		if(next_type::is_required || this_instance._.real_optional_fields_count > RAW_BUF_JOIN(rawbuf_tag_optional_, a) + 1) {
			next_type::iterate_with_depth(this_instance, func_instance, rawbuf_depth);
		}
	} template<typename RAWBUF_M> static void copy(const RAWBUF_M& the_src, const rawbuf_writer<RAWBUF_T>& rawbuf_packet) {
		const RAWBUF_T& rawbuf_src = the_src;
		const T* ptr = rawbuf_src.a();
		if(ptr != 0) {
			rawbuf_packet->a(*ptr);
		} else {
			RAWBUF_T* that = rawbuf_packet();
			that->_.field_offset[1 + RAW_BUF_JOIN(rawbuf_tag_optional_, a)] = 0;
		}
		next_type::copy(rawbuf_src, rawbuf_packet);
	}
	};
	T* a() const {
		if(this->_.real_optional_fields_count > (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, a)) {
			const offset_type *rawbuf_foffset = this->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, a);
			if(*rawbuf_foffset != 0 ) {
				return (T*)((char*)rawbuf_foffset + *rawbuf_foffset );
			}
		}
		return 0;
} template<typename RAWBUF_T> struct rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, a)> : public rawbuf_writer_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, a) - 1 > {
	template<typename RAWBUF_M> bool a(const T* rawbuf_src) {
		RAWBUF_T* that = (*this)();
		offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, a);
		const char* dp = (const char*)(rawbuf_src);
		if(dp < (char*)rawbuf_foffset || dp > this->writer->end()) {
			return false;
		}
		size_t diff = dp - (char*)rawbuf_foffset;
		if(sizeof(offset_type) <= 2 && diff > size_t((offset_type)(-1)) ) {
			return false;
		} *rawbuf_foffset = (offset_type)diff;
		return true;
	} template <typename RAWBUF_M> T* a(const RAWBUF_M& the_src) {
		const T& rawbuf_src = the_src;
		RAWBUF_T* that = (*this)();
		typename RAWBUF_T::offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, a);
		if(*rawbuf_foffset != 0) {
			T* result = (T*)(((char*)rawbuf_foffset) + *rawbuf_foffset );
			*result = rawbuf_src;
			return result;
		}
		return this->writer->append(&rawbuf_src, rawbuf_foffset);
	}
	};
template<typename RAWBUF_T> struct rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, a)> : public rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, a) - 1 > {
	T* a() {
		RAWBUF_T* that = (*this)();
		typedef typename RAWBUF_T::offset_type offset_type;
		if(that->_.real_optional_fields_count <= (offset_type)RAW_BUF_JOIN(rawbuf_tag_optional_, a)) {
			return 0;
		}
		offset_type *rawbuf_foffset = that->_.field_offset + 1 + RAW_BUF_JOIN(rawbuf_tag_optional_, a);
		if(*rawbuf_foffset == 0) return 0;
		T* result = (T*)((char*)rawbuf_foffset + *rawbuf_foffset );
		if(this->begin() >= (char*)(result+1) || this->end() < (char*)(result + 1)) {
			this->invalidate(RAW_BUF_ERROR_MSG("Optional field out of bound!" RAW_BUF_INFO(T, a)));
			return 0;
		}
		if(!RAW_BUF_IS_ALIGNED_PTR(result, rawbuf::rawbuf_alignment<T>::result)) {
			this->invalidate(RAW_BUF_ERROR_MSG("Optional field is not aligned!" RAW_BUF_INFO(T, a)));
			return 0;
		}
		return result;
	} template<rawbuf_cmd::check_cmd cmd> const char* check() {
		this->a();
		if(!(*this)) {
			return this->error_msg();
		}
		return ((rawbuf_reader_helper<RAWBUF_T, RAW_BUF_JOIN(rawbuf_tag_, a) + 1>&)(*this)).check<cmd>();
	}
	};
public:
	static const rawbuf_uint fields_count = RAW_BUF_INDEXER(member_counter);
	static const rawbuf_uint optional_fields_count = RAW_BUF_INDEXER(optional_counter);
	static const rawbuf_uint required_fields_count = RAW_BUF_INDEXER(required_counter);
	typedef members_iterator<int, fields_count - 1> prev_type;
	static const rawbuf_uint fields_alignment = rawbuf::rawbuf_alignment<offset_type>::result;
	static const rawbuf_uint alignment = (fields_alignment > prev_type::required_alignment) ? fields_alignment : prev_type::required_alignment;
	union {
		offset_type real_optional_fields_count;
		offset_type field_offset[1 + optional_fields_count];
	} _;
	static const rawbuf_uint sizer = prev_type::sizer;
	static const rawbuf_uint aligned_1x = prev_type::aligned_1x + (alignment == 1 ? 0:1);
	static const rawbuf_uint aligned_2x = prev_type::aligned_2x + (alignment == 2 ? 0:1);
	static const rawbuf_uint aligned_4x = prev_type::aligned_4x + (alignment == 4 ? 0:1);
	static const rawbuf_uint aligned_8x = prev_type::aligned_8x + (alignment == 8 ? 0:1);
	template<typename RAWBUF_T> struct members_iterator<RAWBUF_T, RAW_BUF_INDEXER(member_counter), true> {
	    static const bool is_required = false;
	template<typename RAWBUF_This, typename RAWBUF_M> static void output(const RAWBUF_This&, RAWBUF_M&, int rawbuf_depth) { } template<typename RAWBUF_This, typename RAWBUF_Func> static void iterate(RAWBUF_This &, RAWBUF_Func& ) { } template<typename RAWBUF_This, typename RAWBUF_Func> static void iterate_with_name(RAWBUF_This &,RAWBUF_Func&) { } template<typename RAWBUF_This, typename RAWBUF_Func> static void iterate_with_depth(RAWBUF_This &, RAWBUF_Func&, int) { } template<typename RAWBUF_M> static void copy(const RAWBUF_M& , const rawbuf_writer<RAWBUF_T>&) { } };
	template<typename RAWBUF_M> void output(RAWBUF_M& ost) {
		rawbuf::json_output<RAWBUF_M> output_functor(ost);
		members_iterator<int, 0>::iterate_with_depth(*this, output_functor, 0);
	} template<typename RAWBUF_Func> void iterate(RAWBUF_Func& func_instance) {
		members_iterator<int, 0 >::iterate(*this, func_instance);
	} template<typename RAWBUF_Func> void iterate_with_name(RAWBUF_Func& func_instance) {
		members_iterator<int, 0 >::iterate_with_name(*this, func_instance);
	} template<typename RAWBUF_Func> void iterate_with_depth(RAWBUF_Func& func_instance, int rawbuf_depth = 0) {
		members_iterator<int, 0 >::iterate_with_depth(*this, func_instance, rawbuf_depth);
	}
};
