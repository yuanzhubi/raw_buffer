#if !defined(RAW_BUF_REQUIRED_H_)
#define RAW_BUF_REQUIRED_H_

#define RAW_BUF_REQUIRE_CHECK_optional_member(input_type, name, count)
#define RAW_BUF_REQUIRE_CHECK_optional_struct(input_type, name, count)

#define RAW_BUF_REQUIRE_CHECK_required_member(input_type, name, count) \
    RAW_BUF_INCREASER(required_counter, RAW_BUF_JOIN(required_, name) ); \
    template<typename T> \
    struct required_members_iterator<T, RAW_BUF_JOIN(required_, name)>{ \
        typedef required_members_iterator<T, RAW_BUF_JOIN(required_, name) + 1 > next_type; \
        template<typename This> \
        static const char* check(const This& this_instance){ \
            if(this_instance.field_offset[RAW_BUF_JOIN(rawbuf_tag_, name)] == 0){ \
                return "Required field empty: " RAW_BUF_STRING(input_type) " " RAW_BUF_STRING(name) ", defined at " __FILE__ ":" RAW_BUF_STRING(__LINE__)  ; \
            }\
            return next_type::check(this_instance); \
        } \
        template<typename This, typename Func> \
        static void iterator(This& this_instance, Func& func_instance){ \
            func_instance(this_instance.name()); \
            next_type::iterator(this_instance, func_instance); \
        } \
    };

#define RAW_BUF_REQUIRE_CHECK_required_struct(input_type, name, count) \
    RAW_BUF_INCREASER(required_counter, RAW_BUF_JOIN(required_, name) ); \
    template<typename T> \
    struct required_members_iterator<T, RAW_BUF_JOIN(required_, name)>{ \
        typedef required_members_iterator<T, RAW_BUF_JOIN(required_, name) + 1 > next_type; \
        template<typename This> \
        static const char* check(const This& this_instance){ \
            const input_type* instance = this_instance.name(); \
            if(instance == 0){ \
                return "Required field empty: " RAW_BUF_STRING(input_type) " " RAW_BUF_STRING(name) ", defined at " __FILE__ ":" RAW_BUF_STRING(__LINE__)  ; \
            }\
            const char* result = instance->check_required(); \
            if(result != 0){ \
                return result; \
            } \
            return next_type::check(this_instance); \
        } \
        template<typename This, typename Func> \
        static void iterator(This& this_instance, Func& func_instance){ \
            input_type* instance = this_instance.name(); \
            func_instance(instance); \
            if(instance != 0){ \
                instance->iterate_required(func_instance); \
            }\
            next_type::iterator(this_instance, func_instance); \
        } \
    };

#endif
