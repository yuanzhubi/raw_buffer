#if !defined(RAW_BUFFER_HEADER_H_)
#define RAW_BUFFER_HEADER_H_

#include <string.h>
#include <stdlib.h>
#include "rawbuffer_alignment.h"

struct rawbuf_proto{
    char* data_ptr;
    size_t data_size;
    size_t data_capacity;
	char* data() const {return this->data_ptr;}
	char* begin() const {return this->data_ptr;}
	char* end() const {return this->data_ptr + this->data_size;}
	size_t size() const {return this->data_size;}
	size_t capacity() const {return this->data_capacity;}

	void reserve(size_t acapacity) {
		if(acapacity < this->data_size){
			return;
		}		
		if(acapacity <= this->data_capacity){
			this->data_capacity = acapacity;
			return;
		}
		this->data_ptr =  (char*)realloc(this->data_ptr, acapacity);
		this->data_capacity = acapacity;
	}

private:
    void operator delete(void* p, size_t the_size); //destructor is not virtual!
};

struct rawbuf_writer_proto : public rawbuf_proto {
public:
    template <typename T, typename M >
    T* append(const T* src, M* offset_pointer){
        const static size_t align_count = rawbuf::rawbuf_alignment<T>::result;
        const size_t new_offset = RAW_BUF_ALIGN(this->data_size, align_count);
        const size_t realsize = 1*sizeof(T);
        const size_t offset_diff = new_offset - (rawbuf_uint)((char*)offset_pointer - this->data_ptr);

        if(sizeof(M) <= 2 && offset_diff > size_t((M)(-1)) ){
            return 0;
        }
        this->data_size = realsize + new_offset;
        *offset_pointer = (M)offset_diff;

        if(this->data_size > this->data_capacity ){
            size_t new_capacity = this->data_size << 1;
            this->data_capacity = new_capacity;
            this->data_ptr = (char*)realloc(this->data_ptr, new_capacity);
        }
        if(src != 0){
             *(T*)(this->data_ptr + new_offset) = *src;
        }
        return (T*)(this->data_ptr + new_offset);
    }

	template <typename T, typename M, typename A > /*count|values*/
    T* append(const T* src, A count, M* offset_pointer){
        const static size_t align_count = rawbuf::rawbuf_alignment<T>::result;

        const size_t new_offset_count = RAW_BUF_ALIGN(this->data_size, rawbuf::rawbuf_property<A>::alignment_result);
        const size_t new_offset = RAW_BUF_ALIGN(new_offset_count + sizeof(A), align_count);
        const size_t realsize = count * sizeof(T);
        const size_t offset_diff = new_offset_count - (rawbuf_uint)((char*)offset_pointer - this->data_ptr);

        if(sizeof(M) <= 2 && offset_diff > size_t((M)(-1)) ){
            return 0;
        }

        this->data_size = realsize + new_offset;
        *offset_pointer = (M)offset_diff;

        if(this->data_size > this->data_capacity){
            size_t new_capacity = this->data_size << 1;
            this->data_capacity  = new_capacity;
            this->data_ptr = (char*)realloc(this->data_ptr, new_capacity);
        }

        *(A*)(this->data_ptr + new_offset_count) = count;
        if(src != 0){
             memcpy(this->data_ptr + new_offset, src, realsize);
        }
        return (T*)(this->data_ptr + new_offset);
    }

    template <typename T>
    static void init_rawbuf_struct(T* p){
        p->_.real_optional_fields_count = T::optional_fields_count;
        for(size_t i = 1; i <= T::optional_fields_count; ++i){
            p->_.field_offset[i] = 0;
        }
    }

    template <typename T, typename M >
    T* alloc(M* offset_pointer, bool set_offset = true){
        const static size_t align_count = rawbuf::rawbuf_alignment<T>::result;

        const size_t new_offset = RAW_BUF_ALIGN(this->data_size, align_count);
        const size_t realsize = 1*sizeof(T);
        const size_t offset_diff = new_offset - (rawbuf_uint)((char*)offset_pointer - this->data_ptr);

        if(sizeof(M) <= 2 && offset_diff > size_t((M)(-1)) ){
            return 0;
        }
        this->data_size = realsize + new_offset;
		if(set_offset){
			*offset_pointer = (M)offset_diff;
		}
        if(this->data_size > this->data_capacity ){
            size_t new_capacity = this->data_size << 1;
            this->data_capacity = new_capacity;
            this->data_ptr = (char*)realloc(this->data_ptr, new_capacity);
        }
        T* dp = (T*)(this->data_ptr + new_offset);
        init_rawbuf_struct(dp);
        return dp;
    }

	template <typename T, typename M, typename A >	/*count|sizeof(T)|values*/
    T* alloc(A count, M* offset_pointer){
		typedef typename T::offset_type offset_type;

        const size_t new_offset_count = RAW_BUF_ALIGN(this->data_size, rawbuf::rawbuf_property<A>::alignment_result);
		const size_t new_offset_size = RAW_BUF_ALIGN(new_offset_count + sizeof(A), rawbuf::rawbuf_alignment<offset_type>::result);
        const size_t new_offset = RAW_BUF_ALIGN(new_offset_size + sizeof(offset_type), rawbuf::rawbuf_alignment<T>::result);
        const size_t realsize = count * sizeof(T);
        const size_t offset_diff = new_offset_count - (rawbuf_uint)((char*)offset_pointer - this->data_ptr);

        if(sizeof(M) <= 2 && offset_diff > size_t((M)(-1)) ){
            return 0;
        }

        this->data_size = realsize + new_offset;
        *offset_pointer = (M)offset_diff;

        if(this->data_size > this->data_capacity){
            size_t new_capacity = this->data_size << 1;
            this->data_capacity  = new_capacity;
            this->data_ptr = (char*)realloc(this->data_ptr, new_capacity);
        }

        *(A*)(this->data_ptr + new_offset_count) = count;
		*(offset_type*)(this->data_ptr + new_offset_size) = sizeof(T);
        T* dp = (T*)(this->data_ptr + new_offset);
        for(size_t i = 0; i < count; ++i,++dp){
            init_rawbuf_struct(dp);
        }
        return (T*)(this->data_ptr + new_offset);
    }
private:
    void operator delete(void* p, size_t the_size); //destructor is not virtual!
};

template <typename T>
struct rawbuf_writer{
    rawbuf_writer_proto* writer;
    size_t offset;

	typedef typename T::template rawbuf_writer_helper<T> helper_type;
    helper_type* operator->() const {
		return (helper_type*)this;
	}
	inline T& operator *() const {return *(T*)(this->writer->data_ptr + this->offset);}
    inline T* operator()() const {return (T*)(this->writer->data_ptr + this->offset);}
    inline T* get() const {return (T*)(this->writer->data_ptr + this->offset);}
	operator bool() const{return this->writer != 0;}
private:
    void operator delete(void* p, size_t the_size); //destructor is not virtual!
};

template <typename T>
struct rawbuf_packet_iterator {
    typedef rawbuf_packet_iterator this_type;
    T* data_ptr;
	size_t data_real_size;
	T* operator()() const {
		return this->data_ptr;
	}
	T* operator->() const {
		return this->data_ptr;
	}
    inline this_type  operator[] (size_t diff) const {
		this_type result;
		result.data_ptr = (T*)((char*)(data_ptr) + data_real_size*diff);
		return result;
	}
    inline this_type  operator+  (size_t diff) const {
		this_type result;
		result.data_ptr = (T*)((char*)(data_ptr) + data_real_size*diff);
		return result;
	}
    inline this_type  operator-  (size_t diff) const {
		this_type result;
		result.data_ptr = (T*)((char*)(data_ptr) - data_real_size*diff);
		return result;
	}

    inline this_type operator-- ()  {
		this->data_ptr = (T*)((char*)(this->data_ptr) - data_real_size*1);
		return *this;
	}
    inline this_type& operator-= (size_t diff) {
		this->data_ptr = (T*)((char*)(this->data_ptr) - data_real_size*diff);
		return *this;
	}

	inline this_type& operator++ ()  {
		this->data_ptr = (T*)((char*)(this->data_ptr) + data_real_size*1);
		return *this;
	}
    inline this_type& operator+= (size_t diff) {
		this->data_ptr = (T*)((char*)(this->data_ptr) + data_real_size*diff);
		return *this;
	}

	inline this_type operator-- (int)          {this_type result = *this; --(*this); return result;}
	inline this_type operator++ (int)          {this_type result = *this; ++(*this); return result;}

    inline bool operator == (const this_type &rhs) const {return this->data_ptr == rhs.data_ptr;}
    inline bool operator != (const this_type &rhs) const {return this->data_ptr != rhs.data_ptr;}
};

template <typename T>
struct rawbuf_writer_iterator : public rawbuf_writer<T>{
    typedef rawbuf_writer_iterator this_type;
    static const size_t element_size = sizeof(T);
    inline this_type  operator[] (size_t diff) const {this_type result = *this; result.offset += diff * element_size; return result;}
    //inline this_type  operator+  (size_t diff) const {this_type result = *this; result.offset += diff * element_size; return result;}
    //inline this_type  operator-  (size_t diff) const {this_type result = *this; result.offset -= diff * element_size; return result;}

    inline this_type& operator-- ()            {this->offset -= 1    * element_size; return *this;}
    inline this_type& operator-= (size_t diff) {this->offset -= diff * element_size; return *this;}

    inline this_type& operator++ ()            {this->offset += 1    * element_size; return *this;}
    inline this_type& operator+= (size_t diff) {this->offset += diff * element_size; return *this;}

	inline this_type operator-- (int)          {this_type result = *this; this->offset -= 1*element_size; return result;}
	inline this_type operator++ (int)          {this_type result = *this; this->offset += 1*element_size; return result;}

    inline bool operator == (const this_type &rhs) const {return this->offset == rhs.offset;}
    inline bool operator != (const this_type &rhs) const {return this->offset != rhs.offset;}

	//rawbuf_packet_iterator<T> operator()() const {rawbuf_packet_iterator<T> result; result.data_ptr = (T*)(this->writer->data_ptr + this->offset); result.data_real_size = this->element_size; return result;}
};

template <typename T>
struct rawbuf_builder : public rawbuf_writer_proto, public rawbuf_writer<T>{
	static const rawbuf_uint align_cost = rawbuf::max_align_cost<T::aligned_1x, T::aligned_2x, T::aligned_4x, T::aligned_8x,rawbuf::rawbuf_alignment<T>::result>::result;
	static const rawbuf_uint init_capacity =  rawbuf::rawbuf_property<T>::size_result + align_cost ;

    rawbuf_builder(rawbuf_uint init_capcitya = init_capacity){
        this->data_ptr =  (char*)malloc(init_capcitya);
        this->data_size = sizeof(T);
        this->data_capacity = init_capcitya;
        rawbuf_writer_proto::init_rawbuf_struct((T*)(this->data_ptr));

        this->writer = this;
        this->offset = 0;
    }

    ~rawbuf_builder(){
        free(this->data_ptr);
    }
private:
    rawbuf_builder(const rawbuf_builder &);                 //disable copy
    rawbuf_builder& operator = (const rawbuf_builder &);    //disable assignment
};

struct rawbuf_reader_proto{
public:
    char* begin() const {return this->data_ptr;}
    char* end() const {return this->_.data_end;}
	operator bool() const {return this->data_ptr != 0;}
	void reset(){
		this->data_ptr = 0;
	}
	void invalidate(const char* msg) {
		this->data_ptr = 0;
		this->_.error_msg = msg;
	}
	const char* error_msg() const {
        return this->_.error_msg;
	}

protected:
	char* data_ptr;
	union unnamed_union{
        char* data_end;
        const char* error_msg;
	}_;
};

template <typename T>
struct rawbuf_reader: public rawbuf_reader_proto{
    typedef typename T::template rawbuf_reader_helper<T> helper_type;
    helper_type* operator->() const {
		return (helper_type*)(this);
	}
	inline T& operator *() const {return *(T*)(this->data_ptr);}
    inline T* operator()() const {return (T*)(this->data_ptr);}
    inline T* get() const {return (T*)(this->data_ptr);}


    bool init(void* data, size_t size){
		this->data_ptr = (char*)data;
		this->_.data_end = ((char*)data) + size;
		return this->init_check();
	}

    bool init(void* data, void* data_end){
		this->data_ptr = (char*)data;
		this->_.data_end = ((char*)data_end);
		return this->init_check();
	}

protected:
    bool init_check(){
		T* that = this->get();
		if(!RAW_BUF_IS_ALIGNED_PTR(that, rawbuf::rawbuf_alignment<T>::result)){
            this->invalidate(RAW_BUF_ERROR_MSG("Pointer alignment check failed!"));
            return false;
        }
        if(this->_.data_end < (char*)(that->_.field_offset + 1)){
            this->invalidate(RAW_BUF_ERROR_MSG("Required fields out of bound!"));
            return false;
        }
        char* field_end = (char*)(that->_.field_offset + 1 + that->_.real_optional_fields_count);
        if(this->data_ptr >= field_end || this->_.data_end < field_end){
            this->invalidate(RAW_BUF_ERROR_MSG("Optional fields out of bound!"));
            return false;
        }
        return true;
    }
};

template <typename T>
struct rawbuf_reader_iterator : public rawbuf_reader<T>{
    typedef rawbuf_reader_iterator this_type;
    size_t element_size;
	bool optional_count_check(this_type& rhs){
		if(this->get()->_.real_optional_fields_count != rhs.get()->_.real_optional_fields_count){
			this->invalidate(RAW_BUF_ERROR_MSG("Invalid optional fields count!"));
			rhs.invalidate(RAW_BUF_ERROR_MSG("Invalid optional fields count!"));
			return false;
		}
		return true;
	}

	bool optional_count_check(size_t optional_count){
		if(this->get()->_.real_optional_fields_count != optional_count){
			this->invalidate(RAW_BUF_ERROR_MSG("Invalid optional fields count!"));
			return false;
		}
		return true;
	}
	inline this_type  operator[] (size_t diff) const {this_type result = *this; result.data_ptr += diff * element_size; result.init_check() && result.optional_count_check(*this); return result;}
    //inline this_type  operator+  (size_t diff) const {this_type result = *this; result.data_ptr += diff * element_size; result.init_check() && result.optional_count_check(*this); return result;}
    //inline this_type  operator-  (size_t diff) const {this_type result = *this; result.data_ptr -= diff * element_size; result.init_check() && result.optional_count_check(*this); return result;}

	inline this_type& operator-- ()            {size_t optional_count = this->get()->_.real_optional_fields_count; this->data_ptr -= 1    * element_size; this->init_check() && this->optional_count_check(optional_count); return *this;}
    inline this_type& operator-= (size_t diff) {size_t optional_count = this->get()->_.real_optional_fields_count; this->data_ptr -= diff * element_size; this->init_check() && this->optional_count_check(optional_count); return *this;}

    inline this_type& operator++ ()            {size_t optional_count = this->get()->_.real_optional_fields_count; this->data_ptr += 1    * element_size; this->init_check() && this->optional_count_check(optional_count); return *this;}
    inline this_type& operator+= (size_t diff) {size_t optional_count = this->get()->_.real_optional_fields_count; this->data_ptr += diff * element_size; this->init_check() && this->optional_count_check(optional_count); return *this;}

	inline this_type operator-- (int)          {this_type result = *this; --(*this); return result;}
	inline this_type operator++ (int)          {this_type result = *this; ++(*this); return result;}

    inline bool operator == (const this_type &rhs) const {return this->data_ptr == rhs.data_ptr;}
    inline bool operator != (const this_type &rhs) const {return this->data_ptr != rhs.data_ptr;}
};

template <typename T>
const char* rawbuf_has_error(rawbuf_reader<T> &r){
	typedef typename T::template rawbuf_reader_helper<T, -1> init_helper_type;
	return ((init_helper_type&)r).template check<T::check_packet>();
}

template <typename T>
const char* rawbuf_has_error(void* buffer, size_t length){
	rawbuf_reader<T> reader;
	reader.init(buffer, length);	
	if(!reader){
		return reader.error_msg();
	}
	return rawbuf_has_error(reader);
}

#endif
