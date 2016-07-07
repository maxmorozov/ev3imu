#ifndef __EV3_TIMER_H
#define __EV3_TIMER_H

#include <stm8/timer.h>

namespace ev3 {
    //Generates events after the specified number of milliseconds
    class Timer2 {
    public:
        Timer2() {
            TIM2()->EGR = 0;
            TIM2()->IER = TIM2_IER_UIE;
        }

        //Starts timer for the specified period in milliseconds
        template <unsigned long period>
        void start() {
            //Convert period to micro-seconds and clock frequency to MHz
            typedef stm8::timer_16bit<period * 1000UL, F_MASTER / 1000000UL> timer_cfg;

            TIM2()->PSCR = timer_cfg::prescaler;
            TIM2()->ARRH = timer_cfg::auto_reset_high;
            TIM2()->ARRL = timer_cfg::auto_reset_low;
            TIM2()->CNTRH = 0;
            TIM2()->CNTRL = 0;
            TIM2()->CR1 = TIM2_CR1_CEN | TIM2_CR1_OPM; //Start one pulse timer
        }

        void stop() {
            TIM2()->CR1 &= ~TIM2_CR1_CEN; //Stop the timer
        }

        void restart() {
            TIM2()->CR1 = TIM2_CR1_CEN | TIM2_CR1_OPM; //Start one pulse timer
        }

        void clear_isr() {
            TIM2()->SR1 &= ~TIM2_SR1_UIF;
        }
    };
}

#endif //__EV3_TIMER_H