// you can define your own output template for logging

#if !defined(RAW_BUF_OUPUT_TEMPLATE)
#define RAW_BUF_OUPUT_TEMPLATE

#define RAW_BUF_DEFAULT_MAX_JSON_DEPTH 32
namespace rawbuf{
    template <typename M, int max_depth = RAW_BUF_DEFAULT_MAX_JSON_DEPTH>
    struct json_output{
	private:
		json_output& operator = (const json_output& arg);
		void prefix_init(int depth){
            if(this->current_depth == depth){              
                this->the_stream << ", " ;              
            } else if(this->current_depth > depth){

                this->the_stream << this->ender_stack[depth] << ",\n" ;
            } 
			this->current_depth = depth;
        }
	public:
        M& the_stream;
        int current_depth;
        char ender_stack[max_depth];
        //bool is_begin;

        json_output(M& streamref):the_stream(streamref), current_depth(-1){
            the_stream << '{';
        }

        ~json_output(){
            while(this->current_depth > 0){
                this->the_stream << this->ender_stack[--this->current_depth];
            }
            this->the_stream << "}\n";
        }

        //rawbuf_packet
        template<typename T>
        bool operator()(T* arg, const char* name, int depth, typename rawbuf::result_check<rawbuf::is_rawbuf_struct<T>::result>::type *p = 0) {(void)p;
            prefix_init(depth);
            if(name != 0){
                this->the_stream << '"' << name << "\" : ";
            }
            if(arg == 0){
                this->the_stream << "null";
            } else{
				this->the_stream << '{' ;
                this->ender_stack[this->current_depth] = '}';
            }
            return true;
        }

        //value
        template<typename T>
        bool operator()(T* arg, const char* name, int depth, typename rawbuf::result_check<!rawbuf::is_rawbuf_struct<T>::result>::type *p = 0) {(void)p;
            prefix_init(depth);
            if(name != 0){
                this->the_stream << '"' << name << "\" : ";
            }
            if(arg == 0){
               this->the_stream << "null";              //null
            } else if(rawbuf::is_num<T>::result){
                if(sizeof(T) != 1){
                    this->the_stream << *arg;               //numeric
                } else{
                    if(*((char*)arg) == '\''){
                         this->the_stream << "\\'" ;        // ' -> \'
                    } else{
                        if(*((char*)arg) != '\\'){
                            this->the_stream << '\'' << *arg << '\'' ;        //character
                        } else{
                            this->the_stream << "'\\\\'";
                        }
                    }
                }
            } else{
                this->the_stream << '"' << *arg << '"';//others, using "" to wrap
            }
            return true;
        }

        //array or other container
        bool operator()(void* arg, size_t the_size, const char* name, int depth) {(void)the_size;
            prefix_init(depth);
            if(name != 0){
                this->the_stream << '"' << name << "\" : ";
            }
            if(arg == 0){
                this->the_stream << "null";
            } else{
                this->the_stream << '[' ;
                this->ender_stack[this->current_depth] = ']';
            }
            return true;
        }

        //char array, and we will escape some character and put '\0' at end 
        bool operator()(char* arg, size_t the_size, const char* name, int depth) {
            prefix_init(depth);
            if(name != 0){
                this->the_stream << '"' << name << "\" : ";
            }
            if(arg == 0 ){
                this->the_stream << "null";
            } else {
                this->the_stream <<'"';
				char old_ch = arg[the_size - 1];
				arg[the_size - 1] = '\0';
                char* parg = arg;
                while(*parg != 0){
                    if(*parg != '"'){
                        if(*parg != '\\'){
                            this->the_stream << *parg;
                        } else{
                            this->the_stream << "\\\\";
                        }

                    } else{
                        this->the_stream << "\\\"";
                    }
                    ++parg;
					arg[the_size - 1] = old_ch;
                }
                this->the_stream <<'"';
            }
            return false;
        }
    };
}


/*
#define RAW_BUF_OUPUT_TEMPLATE(input_type, name) \
    template<typename This, typename M> \
    static void output(const This& this_instance, M& ost, const char* begin, const char* sep, const char* end){ \
        input_type* result = this_instance.name();\
        ost << #name " : " ; \
        if(result != 0){ \
            ost << *result ; \
        } \
        if(RAW_BUF_JOIN(rawbuf_tag_, name) + 1 < optional_field_count ){ \
            ost << sep; \
            members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 >::output(this_instance, ost, begin, sep, end); \
        } \
    }

#endif


#if !defined(RAW_BUF_OUPUT_TEMPLATE_PACKET)

#define RAW_BUF_OUPUT_TEMPLATE_PACKET( input_type, name) \
    template<typename This, typename M> \
    static void output(const This& this_instance, M& ost, const char* begin, const char* sep, const char* end){ \
        input_type* result = this_instance.name();\
        ost << #name " : " ; \
        if(result != 0){ \
            result->output(ost, begin, sep, end); \
        } \
        if(RAW_BUF_JOIN(rawbuf_tag_, name) + 1 < optional_field_count ){ \
            ost << sep; \
            members_iterator<T, RAW_BUF_JOIN(rawbuf_tag_, name) + 1 >::output(this_instance, ost, begin, sep, end); \
        } \
    }

#endif */
#endif
