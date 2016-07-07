#ifndef __UTILS_HIRES_TIMER_H
#define __UTILS_HIRES_TIMER_H

#include <stm8/timer.h>

namespace utils {
    //Timer with 1 microsecond resolution
    class HiResTimer {
    private:
        volatile uint16_t counter;

        union Time {
            uint32_t time;
            struct {
                uint16_t high;
                uint8_t medium;
                uint8_t low;
            };
        };

    public:
        void start() {
            //Convert period to micro-seconds and clock frequency to MHz
            typedef stm8::timer_16bit<0xffff, F_MASTER / 1000000> timer_cfg;

            TIM2()->PSCR = timer_cfg::prescaler;
            TIM2()->ARRH = timer_cfg::auto_reset_high;
            TIM2()->ARRL = timer_cfg::auto_reset_low;

            TIM2()->EGR = TIM2_EGR_UG; //apply the prescaler
            TIM2()->SR1 &= ~TIM2_SR1_UIF;

            TIM2()->IER = TIM2_IER_UIE;
            TIM2()->CR1 = TIM2_CR1_CEN;
        }

        //This method should be called from ISR to reset interrupt flag
        void handle_isr() {
            TIM2()->SR1 &= ~TIM2_SR1_UIF;
            ++counter;
        }

        //returns the current time in microseconds
        uint32_t time() const {
            Time result;
            result.high = counter;
            result.medium = TIM2()->CNTRH;
            result.low = TIM2()->CNTRL;
            return result.time;
        }
    };
}

#endif //__UTILS_HIRES_TIMER_H