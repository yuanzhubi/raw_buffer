#if !defined(RAWBUFFER_CONTAINER_H_)

template<typename ValueType_t, bool check_result = rawbuf::is_rawbuf_struct<ValueType_t>::result>
DEF_PACKET_BEGIN(rawbuf_array)
	typedef ValueType_t value_type;
	typedef rawbuf_array<ValueType_t> this_type;
	ADD_FIELD_ARRAY(value_type, value_data, 12)
public:
	value_type& [](rawbuf_array::array_size_type index) const {
		return this->value_data()[index];
	}
	this_type::array_size_type size() const {
		return this->value_data<this_type::get_count>();
	}
DEF_PACKET_END(rawbuf_array)

template<typename ValueType_t>
DEF_PACKET_BEGIN(rawbuf_array<ValueType_t, true> )
	typedef ValueType_t value_type;
	typedef rawbuf_array<ValueType_t> this_type;
	ADD_PACKET_ARRAY_ANY(value_type, value_data, 12)
public:
	value_type& [](rawbuf_array::array_size_type index) const {
		return this->value_data()()[index];
	}
	this_type::array_size_type size() const {
		return this->value_data<this_type::get_count>();
	}
DEF_PACKET_END(rawbuf_array)

template <typename RawbufTreeNode_t, bool check_result = rawbuf::is_rawbuf_struct<ValueType_t>::result >
struct rawbuf_tree;

template <typename RawbufTreeNode_t>
DEF_PACKET_BEGIN(rawbuf_tree<RawbufTreeNode_t, true>)
	ADD_PACKET_ANY(rawbuf_tree<RawbufTreeNode_t>, left)
	ADD_PACKET_ANY(rawbuf_tree<RawbufTreeNode_t>, right)
	ADD_PACKET(RawbufTreeNode_t, value)
DEF_PACKET_END(rawbuf_tree<RawbufTreeNode_t, true>)

template <typename RawbufTreeNode_t>
DEF_PACKET_BEGIN(rawbuf_tree<RawbufTreeNode_t, false>)
	ADD_PACKET_ANY(rawbuf_tree<RawbufTreeNode_t>, left)
	ADD_PACKET_ANY(rawbuf_tree<RawbufTreeNode_t>, right)
	ADD_FIELD_REQUIRED(RawbufTreeNode_t, value)
DEF_PACKET_END(rawbuf_tree<RawbufTreeNode_t, false>)

#endif
