#ifndef __MPL_NUMBERS_H
#define __MPL_NUMBERS_H

namespace mpl {

    //Compute the number of 1-bits in a word, or byte, etc.
    template <uint32_t x>
    struct pop {
    private:
        static const uint32_t x1 = x - ((x >> 1) & 0x55555555);
        static const uint32_t x2 = (x1 & 0x33333333) + ((x1 >> 2) & 0x33333333);
        static const uint32_t x3 = (x2 + (x2 >> 4)) & 0x0F0F0F0F;
        static const uint32_t x4 = x3 + (x3 >> 8);
        static const uint32_t x5 = x4 + (x4 >> 16);
    public:
        static const uint8_t value = x5 & 0x0000003F;
    };


    //Compute the number of leading zeros in a word.
    template <uint32_t x>
    struct nlz {
    private:
        static const uint32_t x1 = x | (x >> 1);
        static const uint32_t x2 = x1 | (x1 >> 2);
        static const uint32_t x3 = x2 | (x2 >> 4);
        static const uint32_t x4 = x3 | (x3 >> 8);
        static const uint32_t x5 = x4 | (x4 >>16);
    public:
        static const uint8_t value = pop<~x5>::value;
    };

}


#endif //__MPL_NUMBERS_H