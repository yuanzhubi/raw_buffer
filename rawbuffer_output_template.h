// you can define your own output template for logging

#if !defined(RAW_BUF_OUPUT_TEMPLATE)

#define RAW_BUF_OUPUT_TEMPLATE(is_required, input_type, name) \
    template<typename This, typename M> \
    static void output(const This& this_instance, M& ost, const char* begin, const char* sep, const char* end){ \
        input_type* result = this_instance.name();\
        ost << #is_required " " #name " : " ; \
        if(result != 0){ \
            ost << *result ; \
        } \
        if(RAW_BUF_JOIN(rawbuf_tag_, name) + 1 < field_count ){ \
            ost << sep; \
            members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 >::output(this_instance, ost, begin, sep, end); \
        } \
    }

#endif // RAW_BUF_OUPUT_TEMPLATE


#if !defined(RAW_BUF_OUPUT_TEMPLATE_STRUCT)

#define RAW_BUF_OUPUT_TEMPLATE_STRUCT(is_required, input_type, name) \
    template<typename This, typename M> \
    static void output(const This& this_instance, M& ost, const char* begin, const char* sep, const char* end){ \
        input_type* result = this_instance.name();\
        ost << #is_required " " #name " : " ; \
        if(result != 0){ \
            result->output(ost, begin, sep, end); \
        } \
        if(RAW_BUF_JOIN(rawbuf_tag_, name) + 1 < field_count ){ \
            ost << sep; \
            members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 >::output(this_instance, ost, begin, sep, end); \
        } \
    }

#endif // RAW_BUF_OUPUT_TEMPLATE_STRUCT
