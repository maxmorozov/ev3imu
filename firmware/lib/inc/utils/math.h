#ifndef __UTILS_MATH_H
#define __UTILS_MATH_H

namespace utils {

    //Calculates nearest 2^p that is not less than N
    //Rounds up to 2^p
	inline uint32_t next_pow2(uint32_t n) {
        n = n - 1;
	    n |= n >> 1;
	    n |= n >> 2;
	    n |= n >> 4;
	    n |= n >> 8;
	    n |= n >> 16;
		return n + 1;
	}

	inline uint16_t next_pow2(uint16_t n) {
        n = n - 1;
	    n |= n >> 1;
	    n |= n >> 2;
	    n |= n >> 4;
	    n |= n >> 8;
		return n + 1;
	}

	inline uint8_t next_pow2(uint8_t n) {
        n = n - 1;
	    n |= n >> 1;
	    n |= n >> 2;
	    n |= n >> 4;
		return n + 1;
	}

    inline uint32_t pow2(uint32_t n) {
        return 1 << n;
    }

    //Calculate number of set bits in 32-bit word
    inline uint32_t pop_32(uint32_t x) {
       x = x - ((x >> 1) & 0x55555555);
       x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
       x = (x + (x >> 4)) & 0x0F0F0F0F;
       x = x + (x >> 8);
       x = x + (x >> 16);
       return x & 0x3F;
    }

    //Calculate number of set bits in byte
    inline uint8_t pop(uint8_t x) {
       x = x - ((x >> 1) & 0x55);
       x = (x & 0x33) + ((x >> 2) & 0x33);
       x = (x + (x >> 4)) & 0x0F;
       return x & 0x3F;
    }

    //Calculates binary logarithm for 32-bit argument
    inline int32_t log2_32(uint32_t x) {
       x = x | (x >> 1);
       x = x | (x >> 2);
       x = x | (x >> 4);
       x = x | (x >> 8);
       x = x | (x >>16);
       return pop(x) - 1;
    }

    //Calculates binary logarithm for 8-bit argument
    inline int8_t log2(uint8_t x) {
       x = x | (x >> 1);
       x = x | (x >> 2);
       x = x | (x >> 4);
       return pop(x) - 1;
    }

    //Checks if the given argument is a power of 2
	inline bool is_pow2(uint8_t value) {
		return !(value & (value - 1));
	}

}

#endif //__UTILS_MATH_H