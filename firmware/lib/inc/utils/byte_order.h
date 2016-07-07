#ifndef __UTILS_BYTE_ORDER_H
#define __UTILS_BYTE_ORDER_H

#include <stdint.h>
#include <utils/copy.h>
#include <utils/inline.h>
#include <string.h>

namespace utils {

    // Defines byte order in multibyte words.
    enum ByteOrder {
        LSB_FIRST = 0,  // in first address is LSB data
        MSB_FIRST = 1   // in first address is MSB data
    };

    // Defines byte order in multibyte words.
	enum Endianness {
		LittleEndian = LSB_FIRST,
		BigEndian    = MSB_FIRST
	};

	//Copies data as bytes without changing byte order
	//We using 8-bit size argument because IAR produces better code using this approach
	//When we use iterators, IAR produces good code only when the source buffer is
	//a static constant buffer
	struct ByteCopy {
		void operator() (uint8_t* target, const uint8_t* src, uint8_t size) {
			//copy(target, src, size);
            memcpy(target, src, size);
		}
	};

	//Copies data as two byte words with changing byte order
	struct WordCopy {
		INLINE void operator() (uint8_t* target, const uint8_t* src, uint8_t size) {
            while (size >= 2) {
                *target++ = src[1];
                *target++ = src[0];
                src += 2;
                size -= 2;
            }
		}
	};

	//Copies data as four byte words with changing byte order
	struct DwordCopy {
		void operator() (uint8_t* target, const uint8_t* src, uint8_t size) {
            while (size >= 4) {
                *target++ = src[3];
                *target++ = src[2];
                *target++ = src[1];
                *target++ = src[0];
                src += 4;
                size -= 4;
            }
		}
	};

    //Splits integer value to bytes in little-endian order in compile-time
    template <uint32_t value>
    struct little_endian {
        static const uint8_t b1 = value & 0xFF;
        static const uint8_t b2 = (value >> 8) & 0xFF;
        static const uint8_t b3 = (value >> 16) & 0xFF;
        static const uint8_t b4 = (value >> 24) & 0xFF;
    };

    //Container for integer value in little-endian format
    template <uint32_t value_>
    struct uint32_buffer {
        typedef little_endian<value_> value_type;

        uint8_t value[sizeof(uint32_t)];

        uint32_buffer() {
            value[0] = value_type::b1;
            value[1] = value_type::b2;
            value[2] = value_type::b3;
            value[3] = value_type::b4;
        }
    };

    template <int32_t value_>
    struct int32_buffer : uint32_buffer<uint32_t(value_)> { };

}

#ifdef __cplusplus
extern "C" {
#endif

    //Converts 16-bit integer to/from little-endian format
    int16_t swap_bytes(int16_t value);

    //Converts array of 16-bit integer to/from little-endian format
    extern "C" void swap_sample(int16_t (&data)[3]);
#ifdef __cplusplus
}
#endif


#endif //__UTILS_BYTE_ORDER_H