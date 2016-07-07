#ifndef __STM8_EXT_INT_H
#define __STM8_EXT_INT_H

#include <mpl/type_list.h>
#include <mpl/is_same.h>
#include <mpl/min_max.h>
#include <mpl/filter.h>
#include <mpl/sort.h>
#include <mpl/group_by.h>
#include <mpl/map.h>
#include <mpl/length.h>

#include "stm8_target.h"
#include <stm8/config.h>
#include <stm8/details/port_mpl.h>

namespace stm8 {

    namespace details {
        //Defines offset of the port interrupt sensitivity bits in the combined 16-bit
        //control register
        template <Port port>
        struct ext_int_traits {
        };

        template <>
        struct ext_int_traits<Port_A> {
			//Offset in the 16-bit combied interrupt control register
            static const uint8_t offset = 0;
        };
        template<>
        struct ext_int_traits<Port_B> {
            static const uint8_t offset = 2;
        };
        template <>
        struct ext_int_traits<Port_C> {
            static const uint8_t offset = 4;
        };
        template <>
        struct ext_int_traits<Port_D> {
            static const uint8_t offset = 6;
        };
        template <>
        struct ext_int_traits<Port_E> {
            static const uint8_t offset = 8;
        };

	    //Helps to configure external interrupt control registers
	    template <Port port, InterruptMode mode>
	    struct interrupt_port_traits {
            static const uint8_t offset = ext_int_traits<port>::offset;

            static const uint16_t mask = 0x3u << offset;
            static const uint16_t value = (mode << offset) & mask;
	    };

	    template <Port port>
	    struct interrupt_port_traits<port, InterruptsDisabled> {
            static const uint16_t mask = 0x3u << ext_int_traits<port>::offset;
            static const uint16_t value = 0;
	    };

        //Combines interrupt sensitivity values of each port from the list
        template<typename State, typename Item>
        struct int_mask_combiner {
            static const uint16_t value = Item::value | State::value;
            typedef tag<value> type;
        };

        //Extracts port ID from the Pin type
        template <typename PinType>
        struct interrupt_mode_extractor
        {
            static const InterruptMode value = PinType::config::extIntMode;
        };

        //Compares ports by their interrupt modes
        template <typename Left, typename Right>
        struct interrupt_mode_comparator
        {
            static const int value = int(interrupt_mode_extractor<Left>::value) - int(interrupt_mode_extractor<Right>::value);
        };

		//Checks if all pins of the same port have the same interrupt sensitivity.
		//Returns the port configuration
        template <typename TypeList>
        struct port_checker {
            typedef typename mpl::min_element<TypeList, interrupt_mode_comparator>::type min_type;
            typedef typename mpl::max_element<TypeList, interrupt_mode_comparator>::type max_type;

            static_assert(mpl::is_same<min_type, max_type>::value, "All pins should have the same interrupt mode");

            //Output port
            static const InterruptMode mode = min_type::config::extIntMode;
            typedef interrupt_port_traits<min_type::port_name, mode> type;
        };
	}

	//Configures external interrupts using information from pin list
    template <typename PinsTypeList>
    class ExtIntConfigurer {
    private:
        typedef typename mpl::filter<PinsTypeList, details::is_input>::type input_pins;
        typedef typename mpl::filter<input_pins, details::accepts_interrupts>::type input_pins_with_interrupts;


        typedef typename mpl::sort<input_pins_with_interrupts, details::port_comparator>::type sorted_pins;

        typedef typename mpl::group_by<sorted_pins, details::port_comparator>::type grouped_input_pins;
        typedef typename mpl::map<grouped_input_pins, details::port_checker>::type port_int_configs;

        static const uint16_t EXTI_xCR = mpl::fold<port_int_configs, details::tag<0>, details::int_mask_combiner>::type::value;

        static const uint8_t EXTI_CR1 = EXTI_xCR & 0xFF;
        static const uint8_t EXTI_CR2 = (EXTI_xCR >> 8) & 0xFF;

    public:

        static void configure() {
            if (mpl::length<port_int_configs>::value != 0) {
                EXTI()->CR1 = EXTI_CR1;
                EXTI()->CR2 = EXTI_CR2;
            }
        }
    };


}

#endif //__STM8_EXT_INT_H