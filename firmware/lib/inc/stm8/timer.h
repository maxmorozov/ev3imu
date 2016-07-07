#ifndef __STM8_TIMER_H
#define __STM8_TIMER_H

#include <stdint.h>
#include <mpl/math.h>

namespace stm8 {
    //Calculates timer constants
    //Period - timer period in microseconds
    //Freq - CPU frequency in MHz
    template <unsigned long Period, unsigned long Freq>
    struct timer_8bit {
        static const unsigned int max_counter = 255;
        static const unsigned int max_prescaller = 128;

		static_assert(Period * Freq <= max_counter * max_prescaller, "Too long period for this timer type");

        static const unsigned int prescaler = mpl::log2<mpl::clp2<(Period * Freq + max_counter - 1) / max_counter>::value>::value;
        static const unsigned int auto_reset = Period * Freq / mpl::pow2<prescaler>::value;
    };

    template <unsigned long Period, unsigned long Freq>
    struct timer_16bit {
        static const unsigned long max_counter = 65535;
        static const unsigned long max_prescaller = 32768;

		static_assert(Period * Freq <= max_counter * max_prescaller, "Too long period for this timer type");

        static const unsigned int prescaler = mpl::log2<mpl::clp2<(Period * Freq + max_counter - 1) / max_counter>::value>::value;
        static const unsigned int auto_reset = Period * Freq / mpl::pow2<prescaler>::value;

        static const uint8_t auto_reset_low = auto_reset & 0xff;
        static const uint8_t auto_reset_high = (auto_reset >> 8) & 0xff;
    };

}


#endif //__STM8_TIMER_H