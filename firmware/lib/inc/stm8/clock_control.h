#ifndef __STM8_CLOCK_CONTROL_H
#define __STM8_CLOCK_CONTROL_H

#include "stm8_target.h"
#include <stm8/config.h>

namespace stm8 {

	/*
	   CLK Clock Source.
	*/
	enum ClockSource {
	  ClockSourceHSI    = (uint8_t)0xE1, // Clock Source HSI.
	  ClockSourceLSI    = (uint8_t)0xD2, // Clock Source LSI.
	  ClockSourceHSE    = (uint8_t)0xB4  // Clock Source HSE.
	};

}


#endif //__STM8_CLOCK_CONTROL_H