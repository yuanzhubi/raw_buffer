#if !defined(RAWBUFFER_CONTAINER_H_)
#define RAWBUFFER_CONTAINER_H_

template <typename T, bool is_raw_buf_test_result = rawbuf::is_rawbuf_struct<T>::result >
DEF_PACKET_BEGIN(rawbuf_queue_node)
    ADD_FIELD_REQUIRED(T, value)
    ADD_PACKET_ANY(rawbuf_queue_node<T>, next)
DEF_PACKET_END_FINAL

template<typename T>
DEF_PACKET_BEGIN(rawbuf_queue_node<ARGS_LIST(T, true)>)
    ADD_PACKET(T, value)
    ADD_PACKET_ANY(rawbuf_queue_node<T>, next)
DEF_PACKET_END_FINAL

template <typename T, bool is_raw_buf_test_result = rawbuf::is_rawbuf_struct<T>::result >
DEF_PACKET_BEGIN(rawbuf_queue)
    ADD_FIELD_REQUIRED(rawbuf_uint32, size)
    ADD_PACKET_ANY(rawbuf_queue_node<T>, begin)
    struct rawbuf_writer_extend_type {
        size_t real_offset;
    };

    rawbuf_queue() {
        this->size(0);
    }

    template<typename RAWBUF_T>
    struct rawbuf_writer_helper<RAWBUF_T, 2> : protected rawbuf_writer_helper<RAWBUF_T, 1> { //We disable all the pevious method
    public:
        void push_back(const T& src) {
            rawbuf_uint32* pqueue_count = (*this)()->size();
            if (*pqueue_count == 0) {
                *pqueue_count = 1;
                rawbuf_writer<rawbuf_queue_node<T> > data_writer = this->template begin<RAWBUF_T::alloc>();
                data_writer->value(src);
                this->real_offset = (char*)(data_writer()) - this->writer->data_ptr;
            }
            else {
                (*pqueue_count)++;
                typedef typename rawbuf_writer<rawbuf_queue_node<T> >::helper_type local_helper_type;
                size_t tmp_offset = this->offset;
                this->offset = this->real_offset;
                rawbuf_writer<rawbuf_queue_node<T> > data_writer = ((local_helper_type*)(this))->template next<RAWBUF_T::alloc>();
                data_writer->value(src);
                this->real_offset = (char*)(data_writer()) - this->writer->data_ptr;
                this->offset = tmp_offset;
            }
        }
    };
DEF_PACKET_END_FINAL

template<typename T>
struct rawbuf_queue<T, true> : public rawbuf_queue<T, false> {
    template<typename RAWBUF_T, int N>
    struct rawbuf_writer_helper : public rawbuf_queue<T, false>::template rawbuf_writer_helper<RAWBUF_T, 2> {
        rawbuf_writer<T> alloc_back() {
            rawbuf_uint32* pqueue_count = (*this)()->size();
            if (*pqueue_count == 0) {
                *pqueue_count = 1;
                rawbuf_writer<rawbuf_queue_node<T> > data_writer = this->template begin<RAWBUF_T::alloc>();
                this->real_offset = (char*)(data_writer()) - this->writer->data_ptr;
                return data_writer->template value<rawbuf_cmd::alloc>();
            }
            (*pqueue_count)++;
            typedef typename rawbuf_writer<rawbuf_queue_node<T> >::helper_type local_helper_type;
            size_t tmp_offset = this->offset;
            this->offset = this->real_offset;
            rawbuf_writer<rawbuf_queue_node<T> > data_writer = ((local_helper_type*)(this))->template next<RAWBUF_T::alloc>();
            this->real_offset = (char*)(data_writer()) - this->writer->data_ptr;
            this->offset = tmp_offset;
            return data_writer->template value<rawbuf_cmd::alloc>();
        }
    };
};


#endif
