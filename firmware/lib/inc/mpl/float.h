#ifndef _MPL_FLOAT_H
#define _MPL_FLOAT_H

#include <stdint.h>
#include <mpl/if.h>
#include <mpl/numbers.h>

namespace mpl {

    namespace details {
        //Shift the argument left
        template <uint32_t arg, uint8_t shift>
        struct shift_left {
            static const uint32_t value = arg << shift;
        };

        //Shift the argument right with rounding
        template <uint32_t arg, uint8_t shift>
        struct shift_right {
            static const uint32_t round_check_mask = shift != 0 ? 1 << (shift - 1) : 0;
            static const uint32_t round_carry = (arg & round_check_mask) != 0;
            
            //Shift with rounding
            static const uint32_t value = (arg >> shift) + round_carry;
        };

        template <uint32_t arg, int bits>
        struct check_mantissa {
            static const bool value = nlz<arg>::value <= bits;
        };
    }
    

    //Assembles the float value from unsigned integer
    template <uint32_t int_value>
    struct uint_to_float {
    private:
        static const int exponent_bits = 8;
        static const int exponent_bias = 127;
        static const int leading_zeros = mpl::nlz<int_value>::value;
        //The input range is [1..4294967295]. 
        //Leading zeros may be in range [0..31]
        static const uint32_t exponent = exponent_bias + 31 - leading_zeros;
        //
        static const uint32_t mantissa = mpl::if_<
            mpl::details::check_mantissa<int_value, exponent_bits>, 
            mpl::details::shift_right<int_value, uint8_t(exponent_bits - leading_zeros)>, 
            mpl::details::shift_left<int_value, uint8_t(leading_zeros - exponent_bits)> 
        >::type::value;
        
        //When we shift the value right to calulate mantissa, it may be rounded.
        //Due to rounding the value 0xFFFFFFFF will be converted to 0x100000000,
        //so we need additional shift to fit the mantissa into 24 bit.
        //We just check 25 bit and increment the expoent if the bit is not zero.
        static const uint32_t mantissa_carry = mantissa >> 24;
        
        static_assert((mantissa_carry & ~1) == 0, "Only one bit expected");
        
    public:
        static const uint32_t value = ((exponent + mantissa_carry) << 23) | (mantissa & 0x7fffff);
    };

    //Special value for zero float
    template <>
    struct uint_to_float<0> {
    public:
        static const uint32_t value = 0;
    };

    //Assembles the float value from signed integer
    template <int32_t int_value>
    struct int_to_float {
        static const uint32_t minus_sign = 0x80000000;
        
        static const uint32_t value = int_value < 0 ? 
            uint_to_float<uint32_t(-int_value)>::value | minus_sign : 
            uint_to_float<uint32_t(int_value)>::value;
    };
    
}

#endif //_MPL_FLOAT_H
