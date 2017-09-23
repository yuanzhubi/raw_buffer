#include "rawbuffer_util.h"
#include "rawbuffer_alignment.h"
#include "rawbuffer_allocator.h"
#include "rawbuffer_output_template.h"

#include <stddef.h>

struct rawbuf_immutable_config{
	const static int field_index_begin = 1;
};

struct rawbuf_config : public rawbuf_immutable_config{
    typedef rawbuf_uint offset_type;
	typedef rawbuf_uint array_count_type;
   // const static rawbuf_uint fields_alignment =  rawbuf::rawbuf_alignment<offset_type>::result; // The real alignment_size will be max(rawbuf::rawbuf_alignment<offset_type>::result, alignment_size). You can redefine both of these property in your packet definition.
};


struct rawbuf_cmd : public rawbuf_config{
    
	enum alloc_cmd{
        alloc = 0
    };

    enum visit_array_cmd{
		get_count = 0
    };

	enum check_cmd{
		check_packet = 0
	};

	//visit_c_string_cmd
	typedef int c_str;

	//get sizeof(element)
	template <typename T>
	class get_element_size{
	};

	//We can "reference" a packet with higher address.
	typedef int add_ref;
};

template<typename T>
inline typename T::array_count_type* rawbuf_get_array_count_pointer(const T* that, typename T::offset_type filed_tag){
	if(that->_.real_optional_fields_count > filed_tag ){
		const typename T::offset_type *foffset = that->_.field_offset + 1 + filed_tag; 
		if(*foffset != 0){ 
			return (typename T::array_count_type*)((char*)foffset + *foffset); 
		}
	}
	return 0;
} 

#include "rawbuffer_interface.h"
