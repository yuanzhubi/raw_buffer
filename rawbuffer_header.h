#if !defined(RAW_BUFFER_HEADER_H_)
#define RAW_BUFFER_HEADER_H_

#include "rawbuffer_alignment.h"


namespace rawbuf{

    template <typename T>
    struct encode_information{
        T* ptr;
        rawbuf_uint max_hole_size;
        rawbuf_uint max_hole_offset;
    };

    enum auto_alloc_t{
        auto_alloc = 1
    };

    struct top_header{
        rawbuf_uint data_size;

        rawbuf_uint data_capacity; //When the struct encoding is finished, this field is useless.
        union to8bytes{             //When the struct encoding is finished, this field is useless.
            encode_information<void>* data_ref;
            char padder[8];
        }_;
        // So in the header, you have 4+8=12 bytes to save your extended information after encoding.

        template <typename T >
        void set_value(rawbuf_uint* offset_pos, const T* src = 0){
            typedef T type;
            static const bool rawbuf_struct = is_rawbuf_struct<T>::result;
            static const rawbuf_uint align_count = rawbuf_struct ? 4 : rawbuf_property<T>::alignment_result;
            if(*offset_pos == 0){
                rawbuf_uint new_offset = RAW_BUF_ALIGN(this->data_size, align_count);
                if(new_offset + sizeof(type) <= this->data_capacity ){
                    if(!rawbuf_struct){
                        type* new_data = (type *)(((char*)this) + new_offset);
                        *new_data = *src;
                    }
                    this->data_size = new_offset + sizeof(type);
                    *offset_pos = new_offset;
                }
                else{ //reallocated at most 16 more elements or 256 bytes
                    static const rawbuf_uint max_cost = ((256 < sizeof(type))? sizeof(type) : 256);
                    static const rawbuf_uint real_cost = ((max_cost > 16 * sizeof(type)) ? (16 * sizeof(type)): max_cost);
                    rawbuf_uint new_capacity = new_offset + real_cost;
                    rawbuf_uint offset_pos_offset = (char*)offset_pos - (char*)this ;
                    top_header* that_ptr = (top_header*)realloc(this, new_capacity);
                    if(!rawbuf_struct){
                        type* new_data = (type *)(((char*)that_ptr) + new_offset);
                        *new_data = *src;
                    }
                    rawbuf_uint* new_offset_pos = ((rawbuf_uint*)(((char*)that_ptr) + offset_pos_offset));
                    *new_offset_pos = new_offset;
                    that_ptr->data_size = new_offset + sizeof(type);
                    that_ptr->data_capacity  = new_capacity;
                    that_ptr->_.data_ref->ptr = that_ptr;
                }
                return;
            }
            if(!rawbuf_struct){
                type* new_data = (type *)(((char*)this) + *offset_pos);
                *new_data = *src;
            }
        }
    };

    //Force padding 16x for struct
    template<typename T, int padding_factor = 16, rawbuf_uint padding_count = (RAW_BUF_ALIGN((sizeof(T)), padding_factor) - sizeof(T)) >
    struct padding_x : public T {
        char padding[padding_count];
    };

    template<typename T, int padding_factor >
    struct padding_x<T, padding_factor, 0> : public T {
    };

    template <typename T>
    struct with_header :
        //public padding_x<top_header>,  we do not need manually padding now as sizeof(top_header) == 16
        public top_header,
        public T {
    };

    template <typename T>
    struct auto_handler :public encode_information<with_header<T> >{
        static const rawbuf_uint align_cost = max_align_cost<T::aligned_1x, T::aligned_2x, T::aligned_4x - 1, T::aligned_8x,T::aligned_16x, T::alignment>::result;
        static const rawbuf_uint init_capacity = sizeof(rawbuf::with_header<T>) + T::sizer + align_cost;

        auto_handler(){
            encode_information<with_header<T> >* that = ((encode_information<with_header<T> >*)this);
            that->ptr = ((with_header<T>*)malloc(init_capacity));
            that->ptr->data_size = sizeof(top_header) + sizeof(T) ; //  sizeof(top_header) + sizeof(T) <= sizeof(with_header<T>) to save padding
            that->ptr->data_capacity = init_capacity;
            that->ptr->_.data_ref = (encode_information<void>*)(this);
            rawbuf_uint *dest = that->ptr->field_offset;
            for(rawbuf_uint i = 0; i < T::field_count; ++i){
                dest[i] = 0;
            }
            ((encode_information<with_header<T> >*)this)->ptr->this_offset = sizeof(top_header);
        }
        ~auto_handler(){
            free(this->ptr);
        }
        T* operator->() const {return ((encode_information<with_header<T> >*)this)->ptr;}
        operator T*()   const {return ((encode_information<with_header<T> >*)this)->ptr;}
        rawbuf_uint size() const {return ((encode_information<with_header<T> >*)this)->ptr->data_size;}
    private:
        auto_handler(const auto_handler &);
        auto_handler& operator = (const auto_handler &);
    };
}

#endif
