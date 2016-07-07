#ifndef __STM8_PORT_MPL_H
#define __STM8_PORT_MPL_H

#include "stm8_target.h"
#include <stm8/config.h>
#include <stm8/port_defs.h>

namespace stm8 {
	namespace details {

        //Helper class to keep integer constants
        template <uint16_t value_>
        struct tag {
            static const uint16_t value = value_;
        };

        template <typename Config>
        struct extract_DDR {
            static const uint8_t value = Config::xDDR;
        };

        template <typename Config>
        struct extract_CR1 {
            static const uint8_t value = Config::xCR1;
        };

        template <typename Config>
        struct extract_CR2 {
            static const uint8_t value = Config::xCR2;
        };

        template <typename Config>
        struct extract_ODR {
            static const uint8_t value = Config::xODR;
        };

        //Defines a mask generator for the pin based on appropriated configured flag
        //from the pin configuration
        template <template <typename> class Extractor>
        struct gen_mask {
            template<typename State, typename Item>
            struct apply {
                typedef typename Item::config pin_traits;
				static const uint8_t value = (Extractor<pin_traits>::value << Item::pin) | State::value;
				typedef tag<value> type;
            };
        };

        //pin mask in register
        template<typename State, typename Item>
        struct pin2mask {
			static const uint8_t mask = State::value | (1 << Item::pin);
            typedef tag<mask> type;
        };

        template <typename Item>
        struct is_output {
            typedef typename Item::config pin_traits;

            static const bool value = pin_traits::direction == Output;
        };

        template <typename Item>
        struct is_input {
            typedef typename Item::config pin_traits;

            static const bool value = pin_traits::direction == Input;
        };

        template <typename Item>
        struct accepts_interrupts {
            typedef typename Item::config pin_traits;

            static const bool value = pin_traits::extIntMode != InterruptsDisabled;
        };
        
        //Filtering ports by port name
        template<Port port>
        struct port_filter {
            template <typename Item>
            struct predicate {
				static const bool value = Item::port_name == port;
            };
        };

        //Extracts port ID from the Pin type
        template <typename PinType>
        struct get_port_name
        {
            static const Port value = PinType::port_name;
        };

        //Compares ports by their port IDs
		template <typename Left, typename Right>
		struct port_comparator {
            static const int value = (int)get_port_name<Left>::value - (int)get_port_name<Right>::value;
		};
	}
}


#endif //__STM8_PORT_MPL_H