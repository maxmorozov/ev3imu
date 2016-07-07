#ifndef __EV3_UART_SPEED_H
#define __EV3_UART_SPEED_H

namespace ev3 {
    //Calculates the actual EV3 UART speed for the 
    //specified reference value
	template <uint32_t speed>
	struct uart_speed
	{
	private:
		static const uint32_t EV3_UART_CLOCK = 132000000;
		static const uint32_t EV3_UART_PRESCALLER = EV3_UART_CLOCK / speed / 16;
	public:
		static const uint32_t reference = speed;
		static const uint32_t value = EV3_UART_CLOCK / EV3_UART_PRESCALLER / 16;
	};
}

#endif //__EV3_UART_SPEED_H