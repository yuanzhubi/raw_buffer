#if !defined(RAW_BUFFER_HEADER_H_)
#define RAW_BUFFER_HEADER_H_

#include <string.h>
#include <stdlib.h>
#include <new>
#include "rawbuffer_alignment.h"
#include "rawbuffer_varint.h" 

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
    template <typename RAWBUF_T, typename RAWBUF_M >
    RAWBUF_T* append(const RAWBUF_T* src, RAWBUF_M* offset_pointer){
        static const size_t align_count = rawbuf::rawbuf_alignment<RAWBUF_T>::result;
        const size_t new_offset = RAW_BUF_ALIGN(this->data_size, align_count);
        const size_t realsize = 1*sizeof(RAWBUF_T);
        const size_t offset_diff = new_offset - (rawbuf_uint32)((char*)offset_pointer - this->data_ptr);

        if(sizeof(RAWBUF_M) <= 2 && offset_diff > size_t((RAWBUF_M)(-1)) ){
            return 0;
        }
        this->data_size = realsize + new_offset;
        *offset_pointer = (RAWBUF_M)offset_diff;

        if(this->data_size > this->data_capacity ){
            size_t new_capacity = this->data_size << 1;
            this->data_capacity = new_capacity;
            this->data_ptr = (char*)realloc(this->data_ptr, new_capacity);
        }
        if(src != 0){
             *(RAWBUF_T*)(this->data_ptr + new_offset) = *src;
        }
        return (RAWBUF_T*)(this->data_ptr + new_offset);
    }

    template <typename RAWBUF_T, typename RAWBUF_M >
    char* append_varint(RAWBUF_T src, RAWBUF_M* offset_pointer) {
        const size_t new_offset_count = this->data_size;
        const size_t new_offset = (new_offset_count + sizeof(RAWBUF_T) + 1);
        const size_t offset_diff = this->data_ptr - (char*)offset_pointer + this->data_size;

        if (sizeof(RAWBUF_M) <= 2 && offset_diff > size_t((RAWBUF_M)(-1))) {
            return 0;
        }

        size_t tmp_data_size = new_offset;
        *offset_pointer = (RAWBUF_M)offset_diff;

        if (tmp_data_size > this->data_capacity) {
            size_t new_capacity = tmp_data_size << 1;
            this->data_capacity = new_capacity;
            this->data_ptr = (char*)realloc(this->data_ptr, new_capacity);
        }
        
        char *result = rawbuf::rawbuf_varint<RAWBUF_T>::encode(src, this->data_ptr + new_offset_count);
        this->data_size = result - this->data_ptr;
        return result;
    }

    template <typename RAWBUF_T, typename RAWBUF_M, typename RAWBUF_A > /*count|values*/
    RAWBUF_T* append(const RAWBUF_T* src, RAWBUF_A count, RAWBUF_M* offset_pointer) {
        static const size_t align_count = rawbuf::rawbuf_alignment<RAWBUF_T>::result;

        const size_t new_offset_count = this->data_size;
        const size_t new_offset = RAW_BUF_ALIGN(new_offset_count + sizeof(RAWBUF_A) + 1, align_count);
        const size_t realsize = count * sizeof(RAWBUF_T);
        const size_t offset_diff = this->data_ptr - (char*)offset_pointer + this->data_size;

        if (sizeof(RAWBUF_M) <= 2 && offset_diff > size_t((RAWBUF_M)(-1))) {
            return 0;
        }

        size_t tmp_data_size = realsize + new_offset;
        *offset_pointer = (RAWBUF_M)offset_diff;

        if (tmp_data_size > this->data_capacity) {
            size_t new_capacity = tmp_data_size << 1;
            this->data_capacity = new_capacity;
            this->data_ptr = (char*)realloc(this->data_ptr, new_capacity);
        }

        RAWBUF_T* result = (RAWBUF_T*)RAW_BUF_ALIGN_TYPE(((size_t)(rawbuf::rawbuf_varint<RAWBUF_A>::encode(count, this->data_ptr + new_offset_count))), \
            rawbuf::rawbuf_alignment<ARGS_LIST(RAWBUF_T)>::result, size_t);
        this->data_size = (char*)(result) + realsize - this->data_ptr;

        if (src != 0) {
            memcpy(result, src, realsize);
        }       
        return result;
    }

    template <typename RAWBUF_T>
    static void init_rawbuf_struct(RAWBUF_T* p){
        p->set_optional_fields_count(RAWBUF_T::optional_fields_count);
        for(size_t i = RAWBUF_T::field_index_begin; i < RAWBUF_T::optional_fields_count + RAWBUF_T::field_index_begin; ++i){
            p->_.field_offset[i] = 0;
        }
    }

    template <typename RAWBUF_T, typename RAWBUF_M >
    RAWBUF_T* alloc(RAWBUF_M* offset_pointer, bool set_offset = true){
        static const size_t align_count = rawbuf::rawbuf_alignment<RAWBUF_T>::result;

        const size_t new_offset = RAW_BUF_ALIGN(this->data_size, align_count);
        const size_t realsize = 1*sizeof(RAWBUF_T);
        const size_t offset_diff = new_offset - (rawbuf_uint32)((char*)offset_pointer - this->data_ptr);

        if(sizeof(RAWBUF_M) <= 2 && offset_diff > size_t((RAWBUF_M)(-1)) ){
            return 0;
        }
        this->data_size = realsize + new_offset;
        if(set_offset){
            *offset_pointer = (RAWBUF_M)offset_diff;
        }
        if(this->data_size > this->data_capacity ){
            size_t new_capacity = this->data_size << 1;
            this->data_capacity = new_capacity;
            this->data_ptr = (char*)realloc(this->data_ptr, new_capacity);
        }
        RAWBUF_T* dp = (RAWBUF_T*)(this->data_ptr + new_offset);
        init_rawbuf_struct(dp);
        new(dp)RAWBUF_T;
        return dp;
    }

    template <typename RAWBUF_T, typename RAWBUF_M, typename RAWBUF_A > /*count|values*/
    RAWBUF_T* alloc(RAWBUF_A count, RAWBUF_M* offset_pointer){  
        RAWBUF_T* dp = this->append((RAWBUF_T*)0, count, offset_pointer);
        RAWBUF_T* old_dp = dp;
        for(size_t i = 0; i < count; ++i){
            init_rawbuf_struct(dp);
            new(dp++)RAWBUF_T;
        }
        return old_dp;
    }
private:
    void operator delete(void* p, size_t the_size); //destructor is not virtual!
};

struct rawbuf_writer_data {
    rawbuf_writer_proto* writer;
    size_t offset;
};

template <typename RAWBUF_T, bool has_rawbuf_writer_result = rawbuf::has_rawbuf_writer_extend<RAWBUF_T>::result >
struct rawbuf_writer : public rawbuf_writer_data {
    typedef typename RAWBUF_T::template rawbuf_writer_helper<RAWBUF_T, RAWBUF_T::fields_count + 1> helper_type;
    helper_type* operator->() const {
        return (helper_type*)this;
    }
    inline RAWBUF_T& operator *() const {return *(RAWBUF_T*)(this->writer->data_ptr + this->offset);}
    inline RAWBUF_T* operator()() const {return (RAWBUF_T*)(this->writer->data_ptr + this->offset);}
    inline RAWBUF_T* get() const {return (RAWBUF_T*)(this->writer->data_ptr + this->offset);}
    operator bool() const{return this->writer != 0;}
private:
    void operator delete(void* p, size_t the_size); //destructor is not virtual!
};

template <typename RAWBUF_T>
struct rawbuf_writer <RAWBUF_T, true> : public rawbuf_writer <RAWBUF_T, false>, public RAWBUF_T::rawbuf_writer_extend_type{
};

template <typename RAWBUF_T>
struct rawbuf_packet_iterator {
    typedef rawbuf_packet_iterator this_type;
    RAWBUF_T* data_ptr;
    size_t data_real_size;
    RAWBUF_T* operator()() const {
        return this->data_ptr;
    }
    RAWBUF_T* operator->() const {
        return this->data_ptr;
    }
    inline this_type  operator[] (size_t diff) const {
        this_type result;
        result.data_ptr = (RAWBUF_T*)((char*)(data_ptr) + data_real_size*diff);
        return result;
    }
    inline this_type  operator+  (size_t diff) const {
        this_type result;
        result.data_ptr = (RAWBUF_T*)((char*)(data_ptr) + data_real_size*diff);
        return result;
    }
    inline this_type  operator-  (size_t diff) const {
        this_type result;
        result.data_ptr = (RAWBUF_T*)((char*)(data_ptr) - data_real_size*diff);
        return result;
    }

    inline this_type operator-- ()  {
        this->data_ptr = (RAWBUF_T*)((char*)(this->data_ptr) - data_real_size*1);
        return *this;
    }
    inline this_type& operator-= (size_t diff) {
        this->data_ptr = (RAWBUF_T*)((char*)(this->data_ptr) - data_real_size*diff);
        return *this;
    }

    inline this_type& operator++ ()  {
        this->data_ptr = (RAWBUF_T*)((char*)(this->data_ptr) + data_real_size*1);
        return *this;
    }
    inline this_type& operator+= (size_t diff) {
        this->data_ptr = (RAWBUF_T*)((char*)(this->data_ptr) + data_real_size*diff);
        return *this;
    }

    inline this_type operator-- (int)          {this_type result = *this; --(*this); return result;}
    inline this_type operator++ (int)          {this_type result = *this; ++(*this); return result;}

    inline bool operator == (const this_type &rhs) const {return this->data_ptr == rhs.data_ptr;}
    inline bool operator != (const this_type &rhs) const {return this->data_ptr != rhs.data_ptr;}
};

template <typename RAWBUF_T>
struct rawbuf_writer_iterator : public rawbuf_writer<RAWBUF_T>{
    typedef rawbuf_writer_iterator this_type;
    static const size_t element_size = sizeof(RAWBUF_T);
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

    //rawbuf_packet_iterator<RAWBUF_T> operator()() const {rawbuf_packet_iterator<RAWBUF_T> result; result.data_ptr = (RAWBUF_T*)(this->writer->data_ptr + this->offset); result.data_real_size = this->element_size; return result;}
};

template <typename RAWBUF_T>
struct rawbuf_builder : public rawbuf_writer_proto, public rawbuf_writer<RAWBUF_T>{
    static const rawbuf_uint32 align_cost = rawbuf::max_align_cost<RAWBUF_T::aligned_1x, RAWBUF_T::aligned_2x, RAWBUF_T::aligned_4x, RAWBUF_T::aligned_8x,rawbuf::rawbuf_alignment<RAWBUF_T>::result>::result;
    static const rawbuf_uint32 init_capacity =  rawbuf::rawbuf_property<RAWBUF_T>::size_result + align_cost ;

    rawbuf_builder(rawbuf_uint32 init_capcitya = init_capacity){
        this->data_ptr =  (char*)malloc(init_capcitya);
        this->data_size = sizeof(RAWBUF_T);
        this->data_capacity = init_capcitya;      
        rawbuf_writer_proto::init_rawbuf_struct((RAWBUF_T*)(this->data_ptr));
        new(this->data_ptr)RAWBUF_T;
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
        this->_.error_msg = 0;
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

template <typename RAWBUF_T>
struct rawbuf_reader: public rawbuf_reader_proto{
    typedef typename RAWBUF_T::template rawbuf_reader_helper<RAWBUF_T> helper_type;
    helper_type* operator->() const {
        return (helper_type*)(this);
    }
    inline RAWBUF_T& operator *() const {return *(RAWBUF_T*)(this->data_ptr);}
    inline RAWBUF_T* operator()() const {return (RAWBUF_T*)(this->data_ptr);}
    inline RAWBUF_T* get() const {return (RAWBUF_T*)(this->data_ptr);}


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
        RAWBUF_T* that = this->get();
        if(!RAW_BUF_IS_ALIGNED_PTR(that, rawbuf::rawbuf_alignment<RAWBUF_T>::result)){
            this->invalidate(RAW_BUF_ERROR_MSG("Pointer alignment check failed!"));
            return false;
        }
        if(this->_.data_end < (char*)(that->_.field_offset + RAWBUF_T::field_index_begin)){
            this->invalidate(RAW_BUF_ERROR_MSG("Required fields out of bound!"));
            return false;
        }
        char* field_end = (char*)(that->_.field_offset + RAWBUF_T::field_index_begin + that->get_optional_fields_count());
        if(this->data_ptr >= field_end || this->_.data_end < field_end){
            this->invalidate(RAW_BUF_ERROR_MSG("Optional fields out of bound!"));
            return false;
        }
        return true;
    }
};

template <typename RAWBUF_T>
struct rawbuf_reader_iterator : public rawbuf_reader<RAWBUF_T>{
    typedef rawbuf_reader_iterator this_type;
    size_t element_size;
    bool optional_count_check(this_type& rhs){
        if(this->get()->get_optional_fields_count() != rhs.get()->get_optional_fields_count()){
            this->invalidate(RAW_BUF_ERROR_MSG("Invalid optional fields count!"));
            rhs.invalidate(RAW_BUF_ERROR_MSG("Invalid optional fields count!"));
            return false;
        }
        return true;
    }

    bool optional_count_check(size_t optional_count){
        if(this->get()->get_optional_fields_count() != optional_count){
            this->invalidate(RAW_BUF_ERROR_MSG("Invalid optional fields count!"));
            return false;
        }
        return true;
    }
    inline this_type  operator[] (size_t diff) const {this_type result = *this; result.data_ptr += diff * element_size; result.init_check() && result.optional_count_check(*this); return result;}
    //inline this_type  operator+  (size_t diff) const {this_type result = *this; result.data_ptr += diff * element_size; result.init_check() && result.optional_count_check(*this); return result;}
    //inline this_type  operator-  (size_t diff) const {this_type result = *this; result.data_ptr -= diff * element_size; result.init_check() && result.optional_count_check(*this); return result;}

    inline this_type& operator-- ()            {size_t optional_count = this->get()->get_optional_fields_count(); this->data_ptr -= 1    * element_size; this->init_check() && this->optional_count_check(optional_count); return *this;}
    inline this_type& operator-= (size_t diff) {size_t optional_count = this->get()->get_optional_fields_count(); this->data_ptr -= diff * element_size; this->init_check() && this->optional_count_check(optional_count); return *this;}

    inline this_type& operator++ ()            {size_t optional_count = this->get()->get_optional_fields_count(); this->data_ptr += 1    * element_size; this->init_check() && this->optional_count_check(optional_count); return *this;}
    inline this_type& operator+= (size_t diff) {size_t optional_count = this->get()->get_optional_fields_count(); this->data_ptr += diff * element_size; this->init_check() && this->optional_count_check(optional_count); return *this;}

    inline this_type operator-- (int)          {this_type result = *this; --(*this); return result;}
    inline this_type operator++ (int)          {this_type result = *this; ++(*this); return result;}

    inline bool operator == (const this_type &rhs) const {return this->data_ptr == rhs.data_ptr;}
    inline bool operator != (const this_type &rhs) const {return this->data_ptr != rhs.data_ptr;}
};

template <typename RAWBUF_T>
const char* rawbuf_has_error(rawbuf_reader<RAWBUF_T> &r){
    typedef typename RAWBUF_T::template rawbuf_reader_helper<RAWBUF_T, -1> init_helper_type;
    return ((init_helper_type&)r).template check<RAWBUF_T::check_packet>();
}

template <typename RAWBUF_T>
const char* rawbuf_has_error(void* buffer, size_t length){
    rawbuf_reader<RAWBUF_T> reader;
    reader.init(buffer, length);    
    if(!reader){
        return reader.error_msg();
    }
    return rawbuf_has_error(reader);
}

template <typename RAWBUF_T>
size_t rawbuf_get_packet_size(const RAWBUF_T* packet){
    size_t real_optional_fields_count = packet->get_optional_fields_count();
    const size_t estimate_size = sizeof(RAWBUF_T) - RAWBUF_T::optional_fields_count * sizeof(typename RAWBUF_T::offset_type) + real_optional_fields_count * sizeof(typename RAWBUF_T::offset_type);
    const size_t result = RAW_BUF_ALIGN(estimate_size, rawbuf::rawbuf_alignment<RAWBUF_T>::result);
    return result;
}

#endif
