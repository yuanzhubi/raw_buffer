#if !defined(RAWBUF_VARINT)
#define RAWBUF_VARINT
#include "rawbuffer_config.h"
#include "rawbuffer_util.h"

namespace rawbuf {
    template<typename T, bool is_unsigned = (T(-1) > T(0))>
    struct rawbuf_varint {
        inline static char* encode(T src, char* buf_begin) {
            unsigned char* buf = (unsigned char*)buf_begin;
            while (src >= (1<<7)) {
                *buf = (((unsigned char)src) | (1 << 7));
                src >>= 7;
                ++buf;
            }
            *buf = (unsigned char)src;
            return ((char*)buf) + 1;
        }

        inline static char* decode(T& dest, const char* buf_begin) {
            unsigned char* buf = (unsigned char*)buf_begin;
            dest = *buf++;
            if (dest < (1 << 7)) {
                return (char*)buf;
            }
            dest -= 1 << 7;
            dest += ((rawbuf_uint32)(*buf++)) << 7;

            if (dest < (1 << 14)) {
                return (char*)buf;
            }
            dest -= 1 << 14;
            dest += ((rawbuf_uint32)(*buf++)) << 14;

            if (sizeof(T) == 2) {
                return (char*)buf;      //uint16_t
            }

            if (dest < (1 << 21)) {
                return (char*)buf;
            }
            dest -= 1 << 21;
            dest += ((rawbuf_uint32)(*buf++)) << 21;

            if (dest < (1 << 28)) {
                return (char*)buf;
            }
            dest -= 1 << 28;
            dest += ((rawbuf_uint32)(*buf++)) << 28;

            return (char*)buf;
        }
    };

    //We can help the compiler to be more clever for using more registers and more parallel branches, 
    //which enables for cpu eager evaluation. 
    template <>
    inline char* rawbuf_varint<rawbuf_uint64, true>::decode(rawbuf_uint64& dest, const char* buf_begin) {
        unsigned char* buf = (unsigned char*)buf_begin;
        do {
            dest = *buf++;
            if (dest < (1 << 7)) {
                return (char*)buf;
            }
            dest -= 1 << 7;
            dest += ((rawbuf_uint32)(*buf++)) << 7;

            if (dest < (1 << 14)) {
                return (char*)buf;
            }
            dest -= 1 << 14;
            dest += ((rawbuf_uint32)(*buf++)) << 14;

            if (dest < (1 << 21)) {
                return (char*)buf;
            }
            dest -= 1 << 21;
            dest += ((rawbuf_uint32)(*buf++)) << 21;

            if (dest < (1 << 28)) {
                return (char*)buf;
            }
        } while (false);

        rawbuf_uint64 p1 = 0;
        do {
            p1 = *buf++;
            if (dest < (1 << 7)) {
                break;
            }
            p1 -= 1 << 7;
            p1 += ((rawbuf_uint32)(*buf++)) << 7;

            if (dest < (1 << 14)) {
                break;
            }
            p1 -= 1 << 14;
            p1 += ((rawbuf_uint32)(*buf++)) << 14;

            if (dest < (1 << 21)) {
                break;
            }
            p1 -= 1 << 21;
            p1 += ((rawbuf_uint32)(*buf++)) << 21;

            if (dest < (1 << 28)) {
                break;
            }
            p1 -= 1 << 28;
            p1 += ((rawbuf_uint64)(*buf++)) << 28;
        } while (false);
        dest += p1 << 28;
        return (char*)buf;
    }

    template <typename T>
    struct rawbuf_varint<T, false> {
        inline static char* encode(T the_src, char* buf_begin) {
            typedef typename rawbuf::rawbuf_unsigned<T>::type unsigned_type;
            unsigned_type src = (unsigned_type)((the_src << 1) ^ (the_src >> (8 * sizeof(T) - 1)));
            return rawbuf_varint<unsigned_type>::encode(src, buf_begin);
        }

        inline static char* decode(T& the_dest, char* buf_begin) {
            typedef typename rawbuf::rawbuf_unsigned<T>::type unsigned_type;
            unsigned_type &dest = (unsigned_type &)the_dest;
            char* result = rawbuf_varint<unsigned_type>::decode(dest, buf_begin);
            the_dest = (T)((dest >> 1) ^ (-(T)(dest & 1)));
            return result;
        }
    };
} 

#endif