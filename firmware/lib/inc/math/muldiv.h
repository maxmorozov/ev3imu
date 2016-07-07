#ifndef __MATH_MULDIV_H
#define __MATH_MULDIV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    //Signed multiplication of two 16-bit arguments with following division by 0x4000.
    //The result is clipped by range [-32767;32767]
    int16_t muldivs16x16_16(int16_t, int16_t);

    //Signed multiplication of two 16-bit arguments with following division by 0x8000.
    int16_t muldivs16x16_16x(int16_t, int16_t);

    //signed multiplication of two 16-bit arguments with 32-bit signed result
    int32_t muls16x16_32(int16_t, int16_t);

    //unsigned multiplication of two 16-bit arguments with 32-bit unsigned result
    uint32_t mulu16x16_32(uint16_t, uint16_t);

    //Multiplies the value by 2 with overflow control
    //Returns 32767/-32768 in case of overflow
    int16_t scale2(int16_t value);

    //Multiplies the value by 2 with overflow control and then converts the result
    //to little-endian format
    //Returns 32767/-32768 in case of overflow
    int16_t scale2le(int16_t value);

#ifdef __cplusplus
}
#endif


#endif //__MATH_MULDIV_H